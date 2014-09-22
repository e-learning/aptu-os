#include <unistd.h>
#include <wait.h>
#include <iostream>
#include "ExecCmd.h"
#include "../utils/StringUtils.h"

const int PIPE_READ = 0;
const int PIPE_WRITE = 1;

void ExecCmd::exec(vector<string> &args) {
    pid_t pid = fork();
    if (pid != 0) {
        // parent
        int status = 0;
        waitpid(pid, &status, 0);
    } else {
        // child
        vector<char *> argv;
        transformArgs(args, argv);
        argv.push_back(NULL);
        int res = execvp(argv[0], argv.data());
        if(res != 0) {
            char errmsg[256];
            snprintf(errmsg, sizeof(errmsg), "exec '%s' failed", argv[0]);
            cout << "Args: ";
            for(int i = 0; i < argv.size(); i++) {
                cout << argv.data()[i] << " ";
            }
            perror(errmsg);
            cout << endl << flush;
        }
        fflush(stdout);
        exit(0);
    }
}

void ExecCmd::transformArgs(vector<string> &args, vector<char *> &argv) {
    transform(args.begin(), args.end(), back_inserter(argv), [](string c) {
        return const_cast< char* >(c.c_str());
    });
}
