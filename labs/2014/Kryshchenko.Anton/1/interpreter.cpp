#include "interpreter.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <sys/param.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>

using std::string;
using std::cout;
using std::endl;
using std::cin;

Command read_command() {

    Command c;
    string input_buf;
    cout << "# ";
    std::getline( cin, input_buf );

    std::stringstream str( input_buf );

    str >> c.command;
    while( ! str.eof() ) {
        string arg;
        str >> arg;
        c.args.push_back( arg );
    }

    return c;


}

bool exec_command( Command const & c ) {

    if ( c.command == "ls" )
        return exec_ls( c );

    if ( c.command == "ps" )
        return exec_ps( c );
    if ( c.command == "pwd" )
        return exec_pwd( c );
    if ( c.command == "kill" )
        return exec_kill( c );
    if ( c.command == "e" )
        return false;
    return run_proc( c );

}

void info_command() {

    cout<< "Commands:" << endl;
    cout<< "ls" << endl << "pwd" << endl << "ps"<< endl;
    cout<< "kill [SIG] [PID]"<< endl;
    cout<< "e" << endl;
}

string GetCurrentDirectory() {

    char path[ MAXPATHLEN ];
    getcwd( path, MAXPATHLEN );
    return string( path );
}

bool exec_ls(const Command & c) {

    if( !c.args.empty() ) {
        cout<< "Parameters error" << endl;
        return true;
    }

    DIR * directory;
    string current_dir = GetCurrentDirectory();

    if( ( directory = opendir( current_dir.c_str() ) ) == 0 ) {

        cout << "Don't read current directory: "<< current_dir << endl;
        return true;
    }
    else {
        dirent * current_file;
        while( ( current_file = readdir( directory ) ) != 0  ) {
            if( strcmp( current_file->d_name, "." ) &&
                    strcmp( current_file->d_name, ".." ) )
                cout<< current_file->d_name << endl;

        }

        closedir( directory );
    }

    return true;
}

bool run_proc(const Command & c) {

    std::stringstream str;
    str << c.command;

    for ( size_t i = 0 ; i < c.args.size(); ++i )
        str << " " << c.args[i];
    system( str.str().c_str() );
    return true;
}

bool exec_kill(const Command & c) {

    if( c.args.size() != 2 ) {

        cout<< "Parameters Error" << endl;
        return true;

    }
    else {

        int signal = atoi( c.args[0].c_str() );
        int pid = atoi( c.args[1].c_str()  );
        kill(pid, signal);
    }
    return true;

}


bool exec_pwd(const Command & c) {
    if( !c.args.empty() ) {
        cout << "Parameters Error" << endl;
        return true;
    }
    cout << GetCurrentDirectory() << endl;
    return true;
}


bool exec_ps(const Command & c) {

    if( !c.args.empty() ) {
        cout << "Parameters Error" << endl;
        return true;
    }

    DIR* proc_dir;

    string path_proc_dir = "/proc/";

    if( ( proc_dir = opendir( path_proc_dir.c_str() ) ) == 0 ) {
        cout << "Error read proc directory" << endl;
        return true;
    }
    else {

        dirent * pid;
        while( ( pid = readdir( proc_dir ) ) != 0 ) {

            if( pid->d_type == DT_DIR  ) {

                std::ifstream cmd_file(
                            (path_proc_dir + string( pid->d_name )
                             + "/comm").c_str() );
                if( cmd_file.is_open() ) {
                    string command_line;
                    getline( cmd_file, command_line );
                    cout << pid->d_name << ":" <<
                            command_line << endl;
                    cmd_file.close();
                }


            }
        }

        closedir( proc_dir );

    }

    return true;

}

