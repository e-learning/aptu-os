#pragma once

#include <string>
#include "Context.h"
#include "INode.h"
#include "utils/FileUtils.h"
#include "utils/StringUtils.h"

using namespace std;

class FileSystem {
public:
    FileSystem(Context *context) {
        ctx = context;
        free.assign(ctx->block_count, true);
        free[0] = false; // root

        int next_super = 0;
        read_blocks(next_super);
    }

    int sizeofnode(INode node) {
        int bytes = sizeof(int) * 2 + 10 + 1 + sizeof(int) * 3 + sizeof(int) * node.block_count;
        return bytes;
    }

    void read_blocks(int next_super) {
        ifstream ifs(ctx->root + "/" + to_string(next_super), ios_base::binary);
        BinReader binReader(ifs);
        next_super = binReader.readInt();
        int node_count = binReader.readInt();
        for (int i = 0; i < node_count; i++) {
            INode *node = INode::from_stream(ifs);
            inodes.push_back(*node);
            for (int j = 0; j < node->block_count; j++) {
                free[node->blocks[j]] = false;
            }
        }
        if (next_super != 0) {
            read_blocks(next_super);
        }
    }

    void saveINodes() {
        int block = 0;
        save_nodes_to(block, 0);
    }

    void save_nodes_to(int block, int start_id) {
        int available_block_space = ctx->block_size - 2 * sizeof(int);
        clear_block(ctx, block);
        ofstream ofs(ctx->root + "/" + to_string(block), ios_base::out | ios_base::in | ios_base::binary);

        int filled_space = 0;

        vector<INode> to_write;
        int stop_at = -1;
        for(int i = start_id; i < inodes.size(); i++) {
            int node_size = sizeofnode(inodes[i]);
            if(filled_space + node_size <= available_block_space) {
                to_write.push_back(inodes[i]);
                filled_space+=node_size;
            } else {
                stop_at = i;
                break;
            }
        }
        BinWriter binWriter(ofs);
        if(stop_at == -1) {
            binWriter.putInt(block);
            binWriter.putInt(to_write.size());
            for (int i = 0; i < to_write.size(); i++) {
                INode::to_stream(ofs, to_write[i]);
            }
        } else {
            vector<int> free_bl;
            find_free(1, free_bl);
            binWriter.putInt(free_bl[0]);
            binWriter.putInt(to_write.size());
            for (int i = 0; i < to_write.size(); i++) {
                INode::to_stream(ofs, to_write[i]);
            }
            save_nodes_to(free_bl[0], stop_at);
        }
    }

    void parse_path(string fsfile, vector<string> &args) {
        utils::split(fsfile, '/', args);
    }

    void move(string src, string dst) {
        INode node = findINode(src);
        vector<string> src_path;
        parse_path(src, src_path);
        vector<string> dst_path;
        parse_path(dst, dst_path);

        if (src_path.size() != dst_path.size()) {
            throw runtime_error("Renamed file should be in the same place");
        }

        for (int i = 0; i < inodes.size(); i++) {
            if (node.id == inodes[i].id) {
                inodes[i].filename = dst_path[dst_path.size() - 1];
                break;
            }
        }
        saveINodes();
    }

    void import_file(string hostfile, string fsfile) {
        ifstream ifs(hostfile, std::ifstream::binary);
        INode node;
        node.id = inodes.size();

        vector<string> path;
        parse_path(fsfile, path);
        if (path.size() == 0) {
            throw new runtime_error("Incorrect file name");
        }
        if (path.size() == 1) {
            node.parent = -1;
        } else {
            string last = path[path.size() - 1];
            string subpath = fsfile.substr(0, fsfile.length() - last.length() - 1);
            INode parnode = findINode(subpath);
            node.parent = parnode.id;
        }

        node.filename = path[path.size() - 1];
        node.is_directory = false;
        node.files = 0;
        vector<int> node_ids(0);

        int bytes = utils::filesize(hostfile);
        node.bytes = bytes;
        node.block_count = bytes % ctx->block_size == 0 ? bytes / ctx->block_size : bytes / ctx->block_size + 1;
        vector<int> to_fill;
        find_free(node.block_count, to_fill);
        node.blocks = to_fill;

        char *content = new char[bytes];
        ifs.read(content, bytes);
        int offset = 0;
        for (int i = 0; i < to_fill.size(); i++) {
            ofstream ofs(ctx->root + "/" + to_string(to_fill[i]), ios::out | ios::in | ios::binary);
            if (offset + ctx->block_size > bytes)
                ofs.write(content + offset, bytes - offset);
            else
                ofs.write(content + offset, ctx->block_size);
            offset += ctx->block_size;
        }
        delete[] content;

        inodes.push_back(node);
        saveINodes();
    }

    void find_childes(int id, vector<INode> &vector) {
        for (INode node : inodes) {
            if (node.parent == id) {
                vector.push_back(node);
                find_childes(node.id, vector);
            }
        }
    }

    void freeblocks(vector<int> blocks) {
        for (int block_id : blocks) {
            free[block_id] = true;
            vector<char> zeroes(ctx->block_size, 0);
            ofstream block(ctx->root + "/" + to_string(block_id));
            block.write(zeroes.data(), zeroes.size());
        }
    }

    void rm(INode &node) {
        vector<INode> nodes;
        find_childes(node.id, nodes);
        for (INode node : nodes) {
            rm(node);
        }
        freeblocks(node.blocks);
        remove_from_inodes(node);
    }

    void remove_from_inodes(INode &node) {
        for (int i = 0; i < inodes.size(); i++) {
            if (inodes[i] == node) {
                inodes.erase(inodes.begin() + i);
                break;
            }
        }
    }

    void rm(string path) {
        INode file = findINode(path);
        rm(file);
        saveINodes();
    }


    void export_file(string fsfile, string hostfile) {
        INode file = findINode(fsfile);
        char *content = new char[file.bytes];
        int offset = 0;
        for (int i = 0; i < file.block_count; i++) {
            ifstream ifs(ctx->root + "/" + to_string(file.blocks[i]));
            if (offset + ctx->block_size > file.bytes) {
                ifs.read(content + offset, file.bytes - offset);
            } else {
                ifs.read(content + offset, ctx->block_size);
            }
            offset += ctx->block_size;
        }

        ofstream ofs(hostfile, ios::out | ios::binary);
        ofs.write(content, file.bytes);
        delete[] content;
    }

    INode findINode(string fullpath) {
        vector<string> paths;
        parse_path(fullpath, paths);
        INode parent;
        if (paths.size() == 0) {
            throw runtime_error("Empty directory! '" + fullpath + "'");
        }
        for (int i = 0; i < paths.size(); i++) {
            int parid = -1;
            if (i != 0) {
                parid = parent.id;
            }
            parent = findChildOf(parid, paths[i]);
        }
        return parent;
    }

    INode findChildOf(int parent_id, string name) {
        for (INode node : inodes) {
            if (node.parent == parent_id && node.filename == name) {
                return node;
            }
        }
        throw runtime_error("Node " + name + " not found!");
    }

    INode getNode(string fsfile) {
        for (INode node : inodes) {
            if (node.filename == fsfile) {
                return node;
            }
        }
        throw runtime_error("Node " + fsfile + " not found!");
    }

    INode getNodeById(int id) {
        for (INode node : inodes) {
            if (node.id == id) {
                return node;
            }
        }
        throw runtime_error("Node witn id=" + to_string(id) + " not found!");
    }

    void find_direct_childes(int parent_id, vector<INode> &nodes) {
        for (INode node : inodes) {
            if (node.parent == parent_id) {
                nodes.push_back(node);
            }
        }
    }

    bool hasNode(string name, int parent_id) {
        for (INode node : inodes) {
            if (node.parent == parent_id && node.filename == name) {
                return true;
            }
        }
        return false;
    }

    INode make_dir(int id, string name) {
        INode node;
        node.id = inodes.size();
        node.parent = id;
        node.filename = name;
        node.is_directory = true;
        node.bytes = 0;
        node.block_count = 0;
        vector<int> blocks;
        node.blocks = blocks;
        inodes.push_back(node);
        return node;
    }


    void mkdir(string path) {
        vector<string> parts;
        parse_path(path, parts);

        INode parent;
        for (int i = 0; i < parts.size(); i++) {
            int parent_id = -1;
            if (i != 0) {
                parent_id = parent.id;
            }
            bool find = false;
            for (INode node : inodes) {
                if (node.parent == parent_id && node.filename == parts[i]) {
                    parent = node;
                    find = true;
                }
            }
            if (!find) {
                parent = make_dir(parent_id, parts[i]);
            }
        }
        saveINodes();
    }

    void find_free(int count, vector<int> &to_fill) {
        int j = 0;
        for (int i = 0; i < count; i++) {
            for (; j < free.size(); j++) {
                if (free[j]) {
                    free[j] = false;
                    to_fill.push_back(j);
                    break;
                }
            }
        }
    }

    void copy_block(int block, int to_block) {
        ifstream ifs(ctx->root + "/" + to_string(block), std::ifstream::binary);
        char *content = new char[ctx->block_size];
        ifs.read(content, ctx->block_size);
        ofstream ofs(ctx->root + "/" + to_string(to_block), ios::out | ios::in | ios::binary);
        ofs.write(content, ctx->block_size);
        delete[] content;
    }

    void copy_recursive(int parent_id, INode node) {
        if (node.is_directory) {
            INode to_node = INode(node);
            to_node.id = inodes.size();
            to_node.parent = parent_id;
            inodes.push_back(to_node);
            vector<INode> childes;
            find_direct_childes(node.id, childes);
            for (INode child_node : childes) {
                copy_recursive(to_node.id, child_node);
            }
        } else {
            INode to_node = INode(node);
            to_node.id = inodes.size();
            to_node.parent = parent_id;
            inodes.push_back(to_node);
            vector<int> to_fill;
            find_free(to_node.block_count, to_fill);
            to_node.blocks = to_fill;
            for (int i = 0; i < node.block_count; i++) {
                int from_block = node.blocks[i];
                int to_block = to_node.blocks[i];
                copy_block(from_block, to_block);
            }
        }
    }

    void copy(string from, string to) {
        vector<string> to_path;
        parse_path(to, to_path);
        string to_name = to_path[to_path.size() - 1];
        INode node = findINode(from);
        if (!node.is_directory) {
            INode to_node = INode(node);
            to_node.id = inodes.size();
            inodes.push_back(to_node);
            to_node.filename = to_name;
            vector<int> to_fill;
            find_free(to_node.block_count, to_fill);
            to_node.blocks = to_fill;
            for (int i = 0; i < node.block_count; i++) {
                int from_block = node.blocks[i];
                int to_block = to_node.blocks[i];
                copy_block(from_block, to_block);
            }
        } else {
            INode to_node = INode(node);
            to_node.filename = to_name;
            to_node.id = inodes.size();
            inodes.push_back(to_node);
            vector<INode> childes;
            find_direct_childes(node.id, childes);
            for (INode child_node : childes) {
                copy_recursive(to_node.id, child_node);
            }
        }
        saveINodes();
    }

    void file_info(string file_name) {
        if(file_name == "/") {
            for (INode nd : inodes) {
                if (nd.parent == -1) {
                    cout << nd.filename << " " << (nd.is_directory ? 'd' : 'f') << endl;
                }
            }
            return;
        }
        INode node = findINode(file_name);
        if (node.is_directory) {

            for (INode nd : inodes) {
                if (nd.parent == node.id) {
                    cout << nd.filename << " " << (nd.is_directory ? 'd' : 'f') << endl;
                }
            }
        } else {
            cout << node.filename << " " << node.bytes << endl;
        }

    }

    static void init(Context *context) {
        for (size_t i = 0; i < context->block_count; i++) {
            clear_block(context, i);
        }
    }

    static void clear_block(Context *context, size_t i) {
        vector<char> zeroes(context->block_size, 0);
        ofstream block(context->root + "/" + to_string(i), ios_base::out | ios_base::binary);
        block.write(zeroes.data(), zeroes.size());
        block.flush();
    }

protected:
    vector<bool> free;
    Context *ctx;
    vector<INode> inodes;
};
