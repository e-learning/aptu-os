#include "utils.h"

#include <string>
#include <algorithm>
#include <functional>

using std::find_if;
using std::not1;
using std::ptr_fun;
using std::isspace;

string &utils::ltrim(string &src){
    src.erase(src.begin(), find_if(src.begin(), src.end(), not1(ptr_fun<int, int>(isspace))));
    return src;
}

string &utils::rtrim(string &src){
    src.erase(find_if(src.rbegin(), src.rend(), not1(ptr_fun<int, int>(isspace))).base(), src.end());
    return src;
}

string &utils::trim(string &src){
    return ltrim(rtrim(src));
}
