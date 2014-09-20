#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include "Terminal.h"
#include "utils/StringUtils.h"

const int PIPE_READ = 0;
const int PIPE_WRITE = 1;

Terminal::Terminal() {
    cmdFactory = new CmdFactory();
}

string Terminal::read() {
    cout << "$ " << flush;
    string command;
    while (command.empty()) {
        getline(cin, command);
        utils::trim(command);
    }
    return command;
}

void Terminal::eval(string line) {
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
    // Simple REL
    while (true) {
        string command = read();
        eval(command);
    }
}

void Terminal::execWithPipe(string line) {
    int fileDescriptor[2] = {0, 0};
    int pipe_pos = line.rfind('|');
    string firstCommand = line.substr(0, pipe_pos);
    string secondCommand = line.substr(pipe_pos + 1);

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
        eval(secondCommand);
    }
}

void Terminal::exec(string line) {
    if (hasRedirectOut(line)) {
        setupRedirect(line);
        return;
    }
    Cmd *command = cmdFactory->getCmd(line);
    command->exec();
    delete command;
}

string Terminal::setupRedirect(string line) {
    int fileDescriptor[2] = {0, 0};
    pipe(fileDescriptor);
    vector<string> args;
    utils::split(line, ' ', args);
    int in = line.find('<');
    stringstream newline;
    for (int i = 0; i < args.size(); i++) {
        if (args[i] == "<" || args[i] == ">") {
            i++;
        } else {
            newline << args[i] << " ";
        }
    }
    if (in != -1) {
        string file;
        for (int i = 0; i < args.size(); i++) {
            if (args[i] == "<") {
                file = args[i + 1];
            }
        }
        FILE *readfile = fopen(file.c_str(), "r+");
        dup2(fileDescriptor[PIPE_READ], fileno(readfile));
        close(fileDescriptor[PIPE_READ]);
        close(fileDescriptor[PIPE_WRITE]);
        exec(newline.str());
        fclose(readfile);
    } else {
        string file;
        for (int i = 0; i < args.size(); i++) {
            if (args[i] == ">") {
                file = args[i + 1];
            }
        }
        int fl = open(file.c_str(), O_CREAT | O_RDWR | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );
        dup2(fl,1);
        close(fl);
        exec(newline.str());
    }
}

Terminal::~Terminal() {
    delete cmdFactory;
}
