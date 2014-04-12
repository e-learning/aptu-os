#include "Const.h"
#include "utility.h"
#include <sstream>
#include <ctime>
#include <climits>
size_t get_filerest (std::fstream & fs)
{
    size_t curpos = fs.tellg();
    fs.seekg(0, fs.end);
    size_t endpos = fs.tellg();
    fs.seekg(curpos, fs.beg);

    return endpos - curpos;
}

size_t get_filesize_by_name (std::string const & filename)
{
    std::fstream filefs (filename.c_str(),
                         std::fstream::in | std::fstream::out | std::fstream::binary);
    if (filefs.fail()) {
        return INT_MAX;
    }
    filefs.seekg(0, filefs.end);
    size_t filesize = filefs.tellg();
    filefs.close();
    return filesize;
}

size_t get_path(std::string const & filepath, std::vector<std::string> & vpath)
{
    vpath.clear();
    std::stringstream ss(filepath);
    std::string item;
    while (std::getline(ss, item, '/')) {
        vpath.push_back(item);
    }
    if (vpath[0] != "") {
        return ERROR_INCORRECT_PATH;
    }
    vpath.erase(vpath.begin(), vpath.begin() + 1);
    return SUCCESS;
}

size_t get_currenttime()
{
    long int t = static_cast<long int>(time(NULL));
    return static_cast<size_t>(t);
}

bool src_isPartOf_dst (std::vector<std::string> const & vpathSrc, std::vector<std::string> const & vpathDst)
{
    for (size_t i = 0; i < vpathSrc.size(); ++i) {
        if (vpathSrc[i] != vpathDst[i]) {
            return false;
        }
    }
    return true;
}

void print_error (size_t errorCode)
{
    std::string message = "";

    if (errorCode == ERROR_INCORRECT_PATH) {
        message = "ERROR: Incorrect path !";
    }
    else if (errorCode == ERROR_FEW_ARGS) {
        message = "ERROR: Few arguments !";
    }
    else if (errorCode == ERROR_READ_CONFIG) {
        message = "ERROR: Couldn't read config !";
    }
    else if (errorCode == ERROR_NOT_ENOUGH_SPACE) {
        message = "ERROR: No enough space !";
    }
    else if (errorCode == ERROR_INVALID_NAME) {
        message = "ERROR: Invalid name !";
    }
    else if (errorCode == ERROR_NO_FREE_BLOCK) {
        message = "ERROR: No free blocks !";
    }

    std::cout << message << std::endl;
}










