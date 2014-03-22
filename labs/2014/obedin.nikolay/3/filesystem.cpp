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

void filesystem::init()
{
    for (size_t i = 0; i < m_blocks_count; ++i) {
        ofstream out(m_path + to_string(i), ios::trunc);
        out.flush();
    }
}

void filesystem::format()
{
    init();
    m_root = directory("/");
    m_bitmap.assign(m_blocks_count, '\0');
}

void filesystem::import(const string &from_path, const string &to_path)
{
    ifstream in(from_path, ios::binary);
    if (!in.is_open())
        throw error("can not open source file");

    auto splits = file::split_path(to_path);
    string &filename = splits.second;
    if (filename.empty())
        throw error("empty destination filename");

    directory *root = find_last(splits.first);
    if (!root)
        throw error("destination dir not found");

    block_num start_block = next_free_block_num();
    if (start_block == NO_FREE_BLOCKS)
        throw error("no free blocks are available");

    ofilebuf buf(this);
    ostream out(&buf);
    out << in.rdbuf();
    if (!out.good())
        throw error("I/O error while writing file");

    in.seekg(0, in.end);
    bytes size = in.tellg();
    file f(filename, start_block, size);
    root->add_child(f);
}

string filesystem::ls(const string &path)
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
