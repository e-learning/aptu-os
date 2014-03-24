#include "filesystem.hpp"
#include "filebuf.hpp"

filesystem::filesystem(const string &path, bool try_read_metadata)
    : m_path(path), m_block_size(0), m_blocks_count(0)
{
    m_path = m_path.back() == '/' ? m_path : m_path + "/";
    read_config();
    if (try_read_metadata) {
        read_metadata();
        if (is_formatted())
            read_bitmap();
    }
}

void filesystem::cmd_init()
{
    for (size_t i = 0; i < m_blocks_count; ++i) {
        ofstream out(m_path + to_string(i), ios::trunc);
        out.flush();
    }
}

void filesystem::cmd_format()
{
    cmd_init();
    m_root = directory("/");
    m_bitmap.assign(m_blocks_count, '\0');
}

void filesystem::cmd_import(const string &from_path, const string &to_path)
{
    ifstream in(from_path, ios::binary);
    if (!in.is_open())
        throw error("can not open source file");

    auto splits = file::split_path(to_path);
    string &filename = splits.second;
    if (filename.empty())
        throw error("empty destination filename");
    if (filename.size() > 10)
        throw error("destination filename is too long");

    directory *root = find_last(splits.first);
    if (!root)
        throw error("destination dir not found");

    block_num start_block = next_free_block_num();
    if (start_block == NO_FREE_BLOCKS)
        throw error("no free blocks are available");

    ofilebuf buf(this);
    ostream out(&buf);
    out << in.rdbuf();
    if (!buf.is_good || !in.good())
        throw error("I/O error while reading/writing file");

    in.seekg(0, in.end);
    bytes size = in.tellg();
    file f(filename, start_block, size);
    root->add_child_file(f);
}

void filesystem::cmd_export(const string &from_path, const string &to_path)
{
    ofstream out(to_path, ios::binary | ios::trunc);
    if (!out.is_open())
        throw error("can not open destination file");

    auto splits = file::split_path(from_path);
    directory *root = find_last(splits.first);
    if (!root)
        throw error("source file not found");

    file *fmatch = root->find_child_file(splits.second);
    if (!fmatch)
        throw error("source file not found");

    ifilebuf buf(this, *fmatch);
    istream in(&buf);
    out << in.rdbuf();
    if (!buf.is_good || !out.good())
        throw error("I/O error while readin/writing file");
}

string filesystem::cmd_ls(const string &path)
{
    auto splits = file::split_path(path);
    directory *root = find_last(splits.first);
    if (!root)
        throw error("file or dir not found");

    directory *dmatch = nullptr;
    if (splits.second.empty())
        dmatch = root;
    else
        dmatch = root->find_child_dir(splits.second);

    if (dmatch)
        return dmatch->info();

    file *fmatch = root->find_child_file(splits.second);
    if (fmatch)
        return fmatch->info();

    throw error("file or dir not found");
}

void filesystem::cmd_mkdir(const string &path)
{
    auto splits = file::split_path(path);
    directory *root = &m_root;
    bool found_place = false;

    for (const string &dir: splits.first) {
        if (!found_place) {
            directory *last_root = root;
            root = root->find_child_dir(dir);
            if (!root) {
                found_place = true;
                root = last_root;
            }
        }
        if (found_place) {
            if (dir.size() > 10)
                throw error("destination filename is too long");
            if (root->find_child_file(dir))
                throw error("file with the same name already exists");
            root = root->add_child_dir(directory(dir));
        }
    }

    if (splits.second.empty())
        throw error("empty destination filename");
    if (splits.second.size() > 10)
        throw error("destination filename is too long");
    if (root->find_child_file(splits.second))
        throw error("file with the same name already exists");

    root->add_child_dir(directory(splits.second));
}

void filesystem::cmd_move(const string &from_path, const string &to_path)
{
    auto roots = find_roots(from_path, to_path);
    directory *from_root = get<0>(roots);
    directory *to_root   = get<1>(roots);
    string &from_name    = get<2>(roots);
    string &to_name      = get<3>(roots);

    directory *dmatch = from_root->find_child_dir(from_name);
    if (dmatch) {
        if (dmatch->is_parent(to_root))
            throw error("can not move dir into itself");
        directory nd(*dmatch);
        nd.set_name(to_name);
        to_root->add_child_dir(nd);
        from_root->remove_child_dir(dmatch->name());
        return;
    }

    file *fmatch = from_root->find_child_file(from_name);
    if (fmatch) {
        file nf(*fmatch);
        nf.set_name(to_name);
        nf.update_ctime();
        to_root->add_child_file(nf);
        from_root->remove_child_file(fmatch->name());
        return;
    }

    throw error("source file or dir not found");
}

void filesystem::cmd_copy(const string &from_path, const string &to_path)
{
    auto roots = find_roots(from_path, to_path);
    directory *from_root = get<0>(roots);
    directory *to_root   = get<1>(roots);
    string &from_name    = get<2>(roots);
    string &to_name      = get<3>(roots);

    directory *dmatch = from_root->find_child_dir(from_name);
    if (dmatch)
        return copy_dir(dmatch, to_name, from_root, to_root);

    file *fmatch = from_root->find_child_file(from_name);
    if (fmatch)
        return copy_file(fmatch, to_name, to_root);

    throw error("source file or dir not found");
}

tuple<directory*, directory*, string, string>
filesystem::find_roots(const string &from_path, const string &to_path)
{
    auto from_splits = file::split_path(from_path);
    directory *from_root = find_last(from_splits.first);
    if (!from_root)
        throw error("source file or dir not found");

    auto to_splits = file::split_path(to_path);
    if (to_splits.second.empty())
        throw error("empty destination filename");
    if (to_splits.second.size() > 10)
        throw error("destination filename is too long");

    directory *to_root = find_last(to_splits.first);
    if (!to_root)
        throw error("destination file or dir not found");

    if (from_splits.second.compare(to_splits.second) == 0
            && to_root == from_root)
        throw error("can not copy/move into the same place");

    if (to_root->find_child_dir(to_splits.second)
            || to_root->find_child_file(to_splits.second))
        throw error("destination file or dir already exists");

    return make_tuple(from_root, to_root, from_splits.second, to_splits.second);
}

void filesystem::copy_dir(directory *d, const string &name,
        directory *from, directory *to)
{
    directory *nd = to->add_child_dir(directory(name));
    for(file &f: d->files())
        copy_file(&f, f.name(), nd);

    for(directory &cd: d->dirs())
        copy_dir(&cd, cd.name(), d, nd);
}

void filesystem::copy_file(file *f, const string &filename,
        directory *to)
{
    block_num start_block = next_free_block_num();
    if (start_block == NO_FREE_BLOCKS)
        throw error("no free blocks are available");

    ifilebuf ibuf(this, *f);
    istream in(&ibuf);
    ofilebuf obuf(this);
    ostream out(&obuf);

    out << in.rdbuf();
    if (!ibuf.is_good || !obuf.is_good)
        throw error("I/O error while reading/writing file");

    file nf(filename, start_block, f->size());
    to->add_child_file(nf);
}

block_num filesystem::next_free_block_num() const
{
    if (!is_formatted())
        return NO_FREE_BLOCKS;
    bitmap::const_iterator match = find(m_bitmap.begin(), m_bitmap.end(), 0);
    return (match == end(m_bitmap))
            ? NO_FREE_BLOCKS
            : match - begin(m_bitmap);
}

directory *filesystem::find_last(vector<string> path)
{
    directory *out = &m_root;
    for (const string &dir: path) {
        out = out->find_child_dir(dir);
        if (!out)
            return nullptr;
    }
    return out;
}

void filesystem::read_config()
{
    ifstream in(m_path + CONFIG_FILENAME);
    if (!in.is_open())
        throw error("config file can not be opened");

    in >> m_block_size >> m_blocks_count;
    if (!is_valid())
        throw error("incorrect configuration");
}

void filesystem::read_metadata()
{
    ifstream in(m_path + METADATA_FILENAME);
    if (in.is_open())
        in >> m_root;
}

void filesystem::read_bitmap()
{
    ifstream in(m_path + BITMAP_FILENAME, ios::binary);
    if (!in.is_open())
        throw error("bitmap file can not be opened");
    m_bitmap.assign(istream_iterator<char>(in),
                    istream_iterator<char>());
    if (m_bitmap.size() != m_blocks_count)
        throw error("bitmap file size differs from blocks count");
}

void filesystem::write_metadata()
{
    ofstream out(m_path + METADATA_FILENAME, ios::trunc);
    out << m_root;
}

void filesystem::write_bitmap()
{
    ofstream out(m_path + BITMAP_FILENAME, ios::trunc | ios::binary);
    ostream_iterator<char> out_it(out);
    copy(m_bitmap.begin(), m_bitmap.end(), out_it);
}
