#include "File.hpp"
#include "utilities.hpp"

#include <sstream>
#include <iomanip>

using std::endl;

string File::get_info() const
{
    std::stringstream ss;
    ss << "F ";
    ss << std::setw(11) << name << " | ";
    ss << std::setw(11) << size << " bytes"
       << " | ";
    ss << std::setw(6) << blocks.size() << " blocks"
       << " | ";
    ss << utils::tts(modified_time);
    return ss.str();
}

void File::load(ifstream &s)
{
    s >> name;
    s >> size;
    s >> modified_time;
    int size_in_blocks = 0;
    s >> size_in_blocks;
    blocks.assign(size_in_blocks, 0);
    for (int i = 0; i < size_in_blocks; ++i) {
        s >> blocks[i];
    }
}

void File::save(ofstream &s) const
{
    s << name << " ";
    s << size << " ";
    s << modified_time << " ";
    s << blocks.size() << " ";
    for (size_t i = 0; i < blocks.size(); ++i) {
        s << blocks[i] << " ";
    }
    s << endl;
}

void File::addUsedBlock(size_t block)
{
    blocks.push_back(block);
}
