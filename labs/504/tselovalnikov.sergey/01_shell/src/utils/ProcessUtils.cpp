#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <sstream>

#include "ProcessUtils.h"
#include "FileUtils.h"

using namespace std;

namespace utils {

    string listOfAllProcess() {
        vector <string> files;
        int ret = getdir("/proc/", files);
        if (ret != 0) {
            cerr << "Error: " << ret << " reading proc directory" << endl;
            return "";
        }
        sort(files.begin(), files.end());
        ostringstream result;
        result << setw(10) << "PID" << "                 " << "CMD" << endl;
        for (int i = 0; i < files.size(); i++) {
            string pid = files[i];
            if (utils::isNumber(pid)) {
                const char *cmdLinePath = ("/proc/" + pid + "/cmdline").c_str();
                ifstream cmdline(cmdLinePath);
                string cmd;
                getline(cmdline, cmd);
                if (!cmd.empty()) {
                    result << setw(10) << pid << "                 " << cmd << endl;
                }
            }
        }
        return result.str();
    }
}