#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <vector>

struct Command {
    Command() :
        command("") {}

    Command( std::string cmd ) :
        command( cmd ) {}

    std::string command;
    std::vector< std::string > args;
};

Command read_command();
bool exec_command( Command const & c );
void info_command();
bool exec_ls( Command const & c );
bool exec_ps( Command const & c );
bool exec_kill( Command const & c );
bool exec_pwd( Command const & c );
bool run_proc( Command const & c );

#endif // INTERPRETER_H
