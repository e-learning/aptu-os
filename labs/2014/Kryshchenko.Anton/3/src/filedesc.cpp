#include "filedesc.h"
#include <stdexcept>

using std::string;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::runtime_error;

string to_string(int id, string const & root) {
    return root + "/" + "Block" + std::to_string(id);
}

Block read_block_desc(int id, string const & root) {
    Block bd;
    ifstream block(to_string(id, root), ios::binary);
    block.read((char *) &bd, sizeof(Block));
    if (block) {
        return bd;
    } else {
        throw runtime_error("Cannot read descriptor ");
    }
}


string to_string(FileDesc const & f, string const & root) {
    return root + "/" + "File" + std::to_string(f.id);
}

FileDesc read_desc(int id, string const & root) {
    FileDesc fd;
    ifstream block(to_string(id, root), ios::binary);
    block.read((char *) &fd, sizeof(FileDesc));
    if (block) {
        return fd;
    } else {
        throw runtime_error("Cannot read descriptor");
    }
}

void write_desc(FileDesc const & d, string const & root, vector<bool> & bitmap, FileDesc & root_desc) {

    if (d.id == 0) {
        root_desc = d;
    }

    ofstream block(to_string(d.id, root), ios::binary);
    block.write((char *) &d, sizeof(FileDesc));
    if (!block) {
        throw runtime_error("Cannot write descriptor");
    }
    bitmap[d.id] = true;
}

string FileDesc::get_info() {
    string result = "";
    result += (is_directory ? "D " : "F ") + string(name) + " ; ";
    if  (!is_directory) {
        result += std::to_string(block_count) + " blocks ; ";
        result += "modified time: " + string(ctime(&modified_time));
    }
    return result;
}

void FileDesc::clear(vector<bool> & bitmap, string const & root) {
    if (first_block != -1) {
        Block block_desc = read_block_desc(first_block, root);
        while (block_desc.next_id != -1 && block_desc.id < bitmap.size()) {
            bitmap[block_desc.id] = false;
            block_desc = read_block_desc(block_desc.next_id, root);

        }
        if (block_desc.id < bitmap.size()) {
            bitmap[block_desc.id] = false;
        }

    }
}


void FileDesc::remove(vector<bool> & bitmap, string const & root, FileDesc & root_desc) {
    if (prev_file_id == -1) {
        if (next_file_id == -1) {
            FileDesc parent = read_desc(parent_id, root);
            parent.first_file_id = -1;
            write_desc(parent, root, bitmap, root_desc);
        } else {
            FileDesc parent = read_desc(parent_id, root);
            FileDesc next = read_desc(next_file_id, root);
            parent.first_file_id = next.id;
            next.parent_id = parent.id;
            next.prev_file_id = -1;
            write_desc(parent, root, bitmap, root_desc);
            write_desc(next, root, bitmap, root_desc);
        }
    } else {
        if (next_file_id == -1) {
            FileDesc prev = read_desc(prev_file_id, root);
            prev.next_file_id = -1;
            write_desc(prev, root, bitmap, root_desc);
        } else {
            FileDesc prev = read_desc(prev_file_id, root);
            FileDesc next = read_desc(next_file_id, root);
            next.prev_file_id = prev.id;
            prev.next_file_id = next.id;
            write_desc(next, root, bitmap, root_desc);
            write_desc(prev, root, bitmap, root_desc);

        }
    }
    bitmap[id] = false;

}
