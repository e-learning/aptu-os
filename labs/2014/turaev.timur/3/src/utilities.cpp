#include "utilities.hpp"

using std::ifstream;
using std::ofstream;
using std::fstream;
using std::string;

namespace utils {
    std::string pathAppend(string const &path, string const &folder) {
        std::string resultPath(path);
        if (resultPath.back() != '/')
            resultPath += '/';
        return resultPath + folder;
    }

    void write_file_to_block_with_meta(ifstream &inputStream, string const &blockFile, size_t size, File const &file) {
        ofstream outputStream(blockFile, std::ios_base::binary | std::ios_base::trunc);
        if (!outputStream.is_open())
            throw std::runtime_error("I/O error occurs while writing to block");
        size_t next = 0;
        outputStream.write(reinterpret_cast<char const *>(&next), sizeof(next));
        outputStream.write((char const *) &file.name, 11 * sizeof(char));
        outputStream.write((char const *) &file.size, sizeof(file.size));
        outputStream.write((char const *) &file.modified_time, sizeof(file.modified_time));

        size_t metadata_size = sizeof(next) + 11 * sizeof(char) + sizeof(file.size) + sizeof(file.modified_time);
        writeFile(inputStream, outputStream, size - metadata_size);
    }

    void write_file_to_block(ifstream &inputStream, string const &blockFile, size_t size) {
        ofstream outputStream(blockFile, std::ios_base::binary | std::ios_base::trunc);
        if (!outputStream.is_open())
            throw std::runtime_error("I/O error occurs while writing to block");

        size_t next = 0;
        outputStream.write(reinterpret_cast<char const *>(&next), sizeof(next));
        writeFile(inputStream, outputStream, size - sizeof(next));
    }

    void writeFile(ifstream &input, ofstream &output, size_t size) {
        char *buffer = new char [size];
        input.read(buffer, size);
        output.write(buffer, input.gcount());
        delete[] buffer;
    }

    void copy_block_to_block(string const &blockFileIn, string const &blockFileOut, size_t size) {
        ifstream inputStream(blockFileIn, std::ios_base::binary);
        ofstream outputStream(blockFileOut, std::ios_base::binary | std::ios_base::trunc);
        writeFile(inputStream, outputStream, size);
    }

    void read_from_block(ofstream &outputStream, string const &blockFile, size_t size, bool firstBlock) {
        ifstream inputStream(blockFile, std::ios_base::binary);
        if (!inputStream.is_open())
            throw std::runtime_error("I/O error occurs while reading from block");

        if (firstBlock) {
            File f;
            readMetadata(&f, inputStream);
        } else {
            readNextBlockNumber(inputStream);
        }
        size_t metadataSize = inputStream.tellg();
        writeFile(inputStream, outputStream, size - metadataSize);
    }

    string tts(time_t time) {
        char buffer[128];
        strftime(buffer, 128, "%D %T", localtime(&time));
        return string(buffer);
    }

    size_t readMetadata(File *file, ifstream &in) {
        size_t nextBlock;
        in.read(reinterpret_cast<char *>(&nextBlock), sizeof(nextBlock));
        in.read((char *) &file->name, 11 * sizeof(char));
        in.read((char *) &file->size, sizeof(file->size));
        in.read((char *) &file->modified_time, sizeof(file->modified_time));
        return nextBlock;
    }

    size_t readNextBlockNumber(ifstream& in) {
        size_t nextBlock;
        in.read(reinterpret_cast<char *>(&nextBlock), sizeof(nextBlock));
        return nextBlock;
    }

    void overwrite_nextBlockNumber(const string &path, size_t blockNumber) {
        fstream out(path, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
        out.seekp(0);
        out.write(reinterpret_cast<char const *>(&blockNumber), sizeof(blockNumber));
    }

    void overwrite_metadata(File const &file, string const &path) {
        fstream out(path, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
        out.seekp(0);
        size_t next = 0;
        out.write(reinterpret_cast<char const *>(&next), sizeof(next));
        out.write((char const *) &file.name, 11 * sizeof(char));
        out.write((char const *) &file.size, sizeof(file.size));
        out.write((char const *) &file.modified_time, sizeof(file.modified_time));
    }
}