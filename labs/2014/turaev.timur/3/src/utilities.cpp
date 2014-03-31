#include "utilities.hpp"

using std::ifstream;
using std::ofstream;
using std::string;

namespace utils {
std::string pathAppend(string const &path, string const &folder)
{
    std::string resultPath(path);
    if (resultPath.back() != '/')
        resultPath += '/';
    return resultPath + folder;
}

void writeFile(ifstream &input, ofstream &output, size_t size)
{
    char *buffer = new char[size];
    input.read(buffer, size);
    output.write(buffer, input.gcount());
    delete[] buffer;
}

void write_to_block(ifstream &inputStream, string const &blockFile, size_t size)
{
    ofstream outputStream(blockFile, std::ios_base::binary | std::ios_base::trunc);
    if (!outputStream.is_open())
        throw std::runtime_error("I/O error occurs while writing to block");

    writeFile(inputStream, outputStream, size);
}

void write_to_block(string const &blockFileIn, string const &blockFileOut, size_t size)
{
    ifstream inputStream(blockFileIn, std::ios_base::binary);
    ofstream outputStream(blockFileOut, std::ios_base::binary | std::ios_base::trunc);
    writeFile(inputStream, outputStream, size);
}

void read_from_block(ofstream &outputStream, string const &blockFile, size_t size)
{
    ifstream inputStream(blockFile, std::ios_base::binary);
    if (!inputStream.is_open())
        throw std::runtime_error("I/O error occurs while reading from block");

    writeFile(inputStream, outputStream, size);
}

string tts(time_t time)
{
    char buffer[128];
    strftime(buffer, 128, "%D %T", localtime(&time));
    return string(buffer);
}
}