#include "shell_commands.h"
#include <iostream>

using std::endl;
using std::cout;
using std::string;

const string PWD_COM = "pwd";
const string PS_COM = "ps";
const string LS_COM = "ls";
const string KILL_COM = "kill";
const string EXIT_COM = "exit";

bool Execute(string const& command);
void ExecuteKill(string const& command);
string GetWord(string const& str, size_t word_num);
bool IsNumber(string const& str);

int main() {
    while(true) {
        cout << "$wag-$hell:~$ " ;
        string command;
        std::getline(std::cin, command);
        if(!Execute(command)) {
            break;
        }
    }
    return 0;
}

bool Execute(string const& command) {
    if(command.empty()) {
        return true;
    }
    string first_word = GetWord(command, 1);
    if(first_word == PWD_COM) {
        Pwd();
    }
    else if (first_word == PS_COM) {
        Ps();
    }
    else if (first_word == LS_COM) {
        Ls();
    }
    else if (first_word == KILL_COM) {
        ExecuteKill(command);
    }
    else if(first_word == EXIT_COM) {
        return false;
    }
    else {
        Run(command);
    }
    return true;
}

void ExecuteKill(string const& command) {
    string pid = GetWord(command, 2);
    string sig = GetWord(command, 3);
    if(pid.empty() || sig.empty()) {
        cout << "Not enough arguments" << endl;
        return;
    }
    if(!IsNumber(pid)) {
        cout << "Process id should be numeric" << endl;
        return;
    }
    Kill(GetWord(command, 2), GetWord(command, 3));
}

string GetWord(string const& str, size_t word_num) {
    size_t first_char = str.find_first_not_of(" ", 0);
    if(str.find_first_of(" ", first_char) == string::npos) {
        return word_num == 1 ? str.substr(first_char, str.size()) : string();
    }
    for(size_t i = 0; i != word_num - 1; ++i) {
        first_char = str.find_first_of(" ", first_char);
        first_char = str.find_first_not_of(" ", first_char);
        if(first_char == string::npos) {
            return string();
        }
    }
    size_t word_end = str.find_first_of(" ", first_char);
    if(word_end == string::npos) {
        return str.substr(first_char, str.size());
    }
    return str.substr(first_char, word_end - first_char);
}

bool IsNumber(string const& str) {
    string::const_iterator it = str.begin();
    while (it != str.end() && std::isdigit(*it)) {
        ++it;
    }
    return !str.empty() && it == str.end();
}
