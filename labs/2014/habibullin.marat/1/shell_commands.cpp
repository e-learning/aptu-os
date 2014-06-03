#include "shell_commands.h"

#include <iostream>
#include <unistd.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <dirent.h>
#include <fstream>
#include <sys/signal.h>
#include <map>
#include <iomanip>

using std::endl;
using std::cout;
using std::string;

typedef void (*DirEntHandler)(dirent *);

const char* PROC_PATH = "/proc";
const char* SEPARATOR = ":";

int SendSig(string pid, string sig);
string GetCurDir();
void TriverseDir(string const& dir_path, DirEntHandler handler);
void PrintEntry(dirent * entry);
void PrintIfPs(dirent * entry);
string GetPsName(string const& status_path);

void Run(string command) {
    system(command.c_str());
}

void Pwd() {
    cout << GetCurDir() << endl;
}

void Ls() {
    TriverseDir(GetCurDir(), &PrintEntry);
}

void Ps() {
    cout << "  " << "PID" << "    " << "CMD" << endl;
    TriverseDir(string(PROC_PATH), &PrintIfPs);
}

void Kill(std::string pid, std::string sig) {
    int result = SendSig(pid, sig);
    if(result == 1) {
        cout << "Incorrect signal" << endl;
    }
    if(result == -1) {
        cout << "Failed to send signal" << endl;
    }
}

int SendSig(string pid, string sig) {
    std::map<string, int> all_sigs{
        {"SIGABRT", SIGABRT},
        {"SIGALRM", SIGALRM},
        {"SIGFPE", SIGFPE},
        {"SIGHUP", SIGHUP},
        {"SIGILL", SIGILL},
        {"SIGINT", SIGINT},
        {"SIGKILL", SIGKILL},
        {"SIGPIPE", SIGPIPE},
        {"SIGQUIT", SIGQUIT},
        {"SIGSEGV", SIGSEGV},
        {"SIGTERM", SIGTERM},
        {"SIGUSR1", SIGUSR1},
        {"SIGUSR2", SIGUSR2},
        {"SIGCHLD", SIGCHLD},
        {"SIGCONT", SIGCONT},
        {"SIGSTOP", SIGSTOP},
        {"SIGTSTP", SIGTSTP},
        {"SIGTTIN", SIGTTIN},
        {"SIGTTOU", SIGTTOU},
        {"SIGBUS", SIGBUS},
        {"SIGPOLL", SIGPOLL},
        {"SIGPROF", SIGPROF},
        {"SIGSYS", SIGSYS},
        {"SIGTRAP", SIGTRAP},
        {"SIGURG", SIGURG},
        {"SIGVTALRM", SIGVTALRM},
        {"SIGXCPU", SIGXCPU},
        {"SIGXFSZ", SIGXFSZ}
    };
    std::map<string, int>::const_iterator sig_to_send = all_sigs.find(sig);
    if(sig_to_send != all_sigs.end()) {
        return kill(atoi(pid.c_str()), (*sig_to_send).second);
    }
    return 1;
}

string GetCurDir() {
    char* buff = (char*) malloc(PATH_MAX * sizeof(char));
    string path(getcwd(buff, PATH_MAX));
    free(buff);
    return path;
}

void TriverseDir(string const& dir_path, DirEntHandler handler) {
    DIR *proc_dir = opendir(dir_path.c_str());
    if (proc_dir != NULL) {
        dirent *current_entry;
        while ((current_entry = readdir(proc_dir))) {
            handler(current_entry);
        }
    }
    closedir(proc_dir);
}

void PrintEntry(dirent * entry) {
    // is prints . and .. too. Consider filtering them out
    cout << entry->d_name << endl;
}

void PrintIfPs(dirent * entry) {
    int id = atoi(entry->d_name);
    if (id > 0) {
        string status_path = string(PROC_PATH) + "/" + string(entry->d_name) + "/" + "status";
        cout << ' ' << std::setw(4) << entry->d_name << "    " << GetPsName(status_path) << endl;
    }
}

string GetPsName(string const& status_path) {
    std::fstream status_file(status_path.c_str(), std::ios::in);
    string ps_name;
    std::getline(status_file, ps_name);
    size_t name_beg = ps_name.find(SEPARATOR, 0);
    return ps_name.substr(name_beg + 2, ps_name.size());
}
