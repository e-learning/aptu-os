#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <backward/strstream>
#include <fcntl.h>
#include "Terminal.h"
#include "utils/StringUtils.h"

const int PIPE_READ = 0;
const int PIPE_WRITE = 1;

Terminal::Terminal() {
    cmdFactory = new CmdFactory();
}

string Terminal::read() {
    cout << "$ ";
    string command;
    while(command.empty()) {
        getline(cin, command);
        utils::trim(command);
    }
    return command;
}

void Terminal::print(string str) {
    cout << str << endl;
}

string Terminal::eval(string line) {
    if (hasPipe(line)) {
        return execWithPipe(line);
    } else {
        return exec(line);
    }
}

bool Terminal::hasPipe(string line) {
    return line.find('|') != string::npos;
}

bool Terminal::hasRedirectOut(string line) {
    return line.find('>') != string::npos || line.find('<') != string::npos;
}

void Terminal::run() {
    // Simple REPL
    while (true) {
        string command = read();
        string result = eval(command);
        print(result);
    }
}

string Terminal::execWithPipe(string line) {
    int fileDescriptor[2] = {0, 0};
    int pipe_pos = line.rfind('|');
    string firstCommand = line.substr(0, pipe_pos);
    string secondCommand = line.substr(pipe_pos + 1);

    cout << firstCommand << endl << secondCommand << endl;

    pipe(fileDescriptor);
    pid_t pid = fork();
    if (pid == 0) {
        dup2(fileDescriptor[PIPE_WRITE], STDOUT_FILENO);
        close(fileDescriptor[PIPE_READ]);
        close(fileDescriptor[PIPE_WRITE]);
        eval(firstCommand);
        exit(0);
    } else {
        // parent
        dup2(fileDescriptor[PIPE_READ], STDIN_FILENO);
        close(fileDescriptor[PIPE_WRITE]);
        close(fileDescriptor[PIPE_READ]);
        int status = 0;
        waitpid(pid, &status, 0);
        string result = eval(secondCommand);
        return result;
    }
}

string Terminal::exec(string line) {
    if(hasRedirectOut(line)) {
        return setupRedirect(line);
    }
    Cmd* command = cmdFactory->getCmd(line);
    string result = command->exec();
    delete command;
    return result;
}

string Terminal::setupRedirect(string line) {
    int fileDescriptor[2] = {0, 0};
    pipe(fileDescriptor);
    pid_t pid = fork();
    if(pid == 0) {
        close(fileDescriptor[PIPE_READ]);
        vector<string> args;
        utils::split(line, ' ', args);
        int in = line.find('<');
        if (in != -1) {
            string file;
            for (int i = 0; i < args.size(); i++) {
                if (args[i] == "<") {
                    file = args[i + 1];
                }
            }
            FILE *readfile = fopen(file.c_str(), "r");
            dup2(fileDescriptor[PIPE_READ], fileno(readfile));
            close(fileDescriptor[PIPE_READ]);
            close(fileDescriptor[PIPE_WRITE]);
        } else {
            string file;
            for (int i = 0; i < args.size(); i++) {
                if (args[i] == ">") {
                    file = args[i + 1];
                }
            }
            freopen(file.c_str(), "a+", stdout);
//            int fd = open(file.c_str(), O_WRONLY | O_CREAT);
//            dup2(fd, PIPE_WRITE);   // make stdout go to file
//            close(fd);     // fd no longer needed - the dup'ed handles are sufficient
        }
        strstream newline;
        for (int i = 0; i < args.size(); i++) {
            if (args[i] == "<" || args[i] == ">") {
                i++;
            } else {
                newline << args[i] << " ";
            }
        }
        exec(newline.str());
        fflush(stdout);
        sleep(1);
        exit(0);
    } else {
        int status = 0;
        waitpid(pid, &status, 0);
        return "";
    }
}
