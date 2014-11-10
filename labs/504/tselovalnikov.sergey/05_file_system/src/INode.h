#pragma once

#include <iostream>
#include <vector>
#include "utils/BinReader.h"
#include "utils/BinWriter.h"

using namespace std;

class INode {
public:
    int id;
    int parent;
    string filename;
    bool is_directory;
    int files = 0;
    int bytes;
    int block_count;
    vector<int> blocks;

    friend bool operator==(const INode& left, const INode& right);

    static INode* from_stream(ifstream& ifs) {
        BinReader binReader(ifs);
        INode *node = new INode();
        node->id = binReader.readInt();
        node->parent = binReader.readInt();
        node->filename = binReader.readStr(10);
        node->is_directory = binReader.readBool();
        node->files = binReader.readInt();
        vector<int> node_ids;
        for(int i = 0; i < node->files; i++) {
            node_ids.push_back(binReader.readInt());
        }
        node->bytes = binReader.readInt();
        node->block_count = binReader.readInt();
        vector<int> blocks;
        for (int i = 0; i < node->block_count; ++i) {
            blocks.push_back(binReader.readInt());
        }
        node->blocks = blocks;
        return node;
    }

    static void to_stream(ofstream& ofs, INode& node) {
        BinWriter binWriter(ofs);
        binWriter.putInt(node.id);
        binWriter.putInt(node.parent);
        binWriter.putStr(node.filename, 10);
        binWriter.putBool(node.is_directory);
        binWriter.putInt(node.files);
        binWriter.putInt(node.bytes);
        binWriter.putInt(node.block_count);
        for(int i = 0; i < node.block_count; i++) {
            binWriter.putInt(node.blocks[i]);
        }
    }
};