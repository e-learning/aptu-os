#include <iostream>

#include "LsCmd.h"
#include "../utils/StringUtils.h"
#include "../utils/FileUtils.h"

string LsCmd::exec(vector<string> &args) {
    string dir = args.size() > 1 ? args[1] : ".";
    vector<string> files;
    int ret = utils::getdir(dir, files);
    if (ret != 0) {
        cerr << "Error: " << ret << " reading directory " << dir << endl;
        return "";
    }
    sort(files.begin(), files.end());
    return utils::join(files, " ");
}
