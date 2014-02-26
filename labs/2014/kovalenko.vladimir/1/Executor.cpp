#include "Executor.h"
#include <iostream>
#include <signal.h>



bool isNumber(const std::string& str)
{
    std::string::const_iterator it = str.begin();
    if(*it == '-'){
        it++;
    }
    while (it != str.end() && std::isdigit(*it)) ++it;
    return (!str.empty() && (it == str.end()));
}

bool Command::hasNoArguments(){
	return args.empty();
}

bool Executor::execute(Command const & c){
	 
	if (c.cmd == "exit")
        return false;

	if (c.cmd.empty())
        return true;

    if (c.cmd == "pwd")
        executePwdCommand(c);
    else if (c.cmd == "ls")
        executeLsCommand(c);
    else if (c.cmd == "ps")
        executePsCommand(c);
    else if (c.cmd == "kill")
        executeKillCommand(c);
    else
        system(c.cmd.c_str());

    return true;
}

void Executor::executeKillCommand(Command const & cmd){
	if (cmd.args.size() != 2 || !isNumber(cmd.args[0]) || !isNumber(cmd.args[1])) {
    	std::cerr << "'kill' command requires two numeric arguments" << std::endl;
    	return;
    }
    kill(atoi(cmd.args[0].c_str()), atoi(cmd.args[1].c_str()));
}

void Executor::executePsCommand(Command const & cmd){

}

void Executor::executeLsCommand(Command const & cmd){

}

void Executor::executePwdCommand(Command const & cmd){

}

void Executor::executeOtherCommand(Command const & cmd){

}
