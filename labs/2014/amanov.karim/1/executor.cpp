#include "executor.h"
#include <iostream>
#include "cmdinfo.h"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <dirent.h>
#include <string.h>
#include <fstream>
#include <signal.h>
#include "pscmd.h"

void Executor::exec(CmdInfoPtr cmdInfo)
{
	switch(cmdInfo->cmdType) {
		case CmdInfo::C_CD : 	execCd(cmdInfo);	
					break;
		case CmdInfo::C_KILL : 	execKill(cmdInfo);
					break;
		case CmdInfo::C_EXIT :  execExit(cmdInfo);
					break;
		default: 		execProg(cmdInfo);
	}
}

void Executor::execCd(CmdInfoPtr cmdInfo)
{
	int res = 0;
	std::vector<char*> argv;
	cmdInfo->getArgs(argv);
	if (cmdInfo->argc > 1) {
		res = chdir(argv[1]);
		procError(res, "cli : cd");
	}

}

int Executor::select(const struct dirent * d) 
{
	return 1;

}


void Executor::execLs(CmdInfoPtr cmdInfo)
{
		int res = 0;
		std::vector<char*> argv;
		cmdInfo->getArgs(argv);
		char* dir;
		if (cmdInfo->argc == 1) {
			dir = get_current_dir_name();	
		} else {
			dir = argv[1];
		}
		struct dirent ** entry;
		res = scandir(dir, &entry, select, alphasort);
		if (res != -1) {
		for(int i = 2; i < res; i++) {
			std::cout << entry[i]->d_name << "   ";
		}
		std::cout << std::endl;
		} else {
			procError(res, "cli : ls");
		}
		exit(EXIT_SUCCESS);
}

void Executor::execPwd(CmdInfoPtr cmdInfo)
{
	std::cout << get_current_dir_name() << std::endl;
	exit(EXIT_SUCCESS);

}

void Executor::execExit(CmdInfoPtr cmdInfo)
{
	exit(EXIT_SUCCESS);
}

void Executor::execKill(CmdInfoPtr cmdInfo)
{
	int res = 0;
	
	std::vector<char*> argv;
	cmdInfo->getArgs(argv);

	if (cmdInfo->argc < 3) {
		std::cerr << "Invalid arguments: use :> kill signal pid" << std::endl;
	} else {
		int sig;
		pid_t proc;
		if (checkIsNumber(argv[1])) {
			sig = std::stoi(argv[1]);
		} else {
			std::cerr << "Invalid arguments: use :> kill signal pid" << std::endl;
			return;
		}

		if (checkIsNumber(argv[2])) {
			proc = std::stoi(argv[2]);
		} else {
			std::cerr << "Invalid arguments: use :> kill signal pid" << std::endl;
			return;
		}
		res = kill(proc, sig);
		procError(res, "cli : kill");
	}

}

void Executor::execute(CmdInfoPtr cmdInfo)
{
	int res = 0;
	int pipefd[2];
	if (cmdInfo->isPipe) {
        	res = pipe(pipefd);
		procError(res, "pipe", true);	
		pid_t pipedProc = fork();
		procError(pipedProc, "fork", true);
		if(pipedProc == 0) {
			res = close(pipefd[0]);
			procError(res, "close", true);
			res = dup2(pipefd[1], 1);
			res = close(pipefd[1]);
			procError(res, "dup", true);
		        execute(cmdInfo->pipeCmd);
			exit(EXIT_SUCCESS);
		} else {
			res = close(pipefd[1]);
			procError(res, "close", true);
			res = dup2(pipefd[0], 0);
			res = close(pipefd[0]);
			procError(res, "dup", true);
		}
		
	}
	switch(cmdInfo->cmdType) {
	case CmdInfo::C_LS : 	execLs(cmdInfo);
				break;
	case CmdInfo::C_PWD : 	execPwd(cmdInfo);
				break;
	case CmdInfo::C_PS :	execPs();
				break;
	case CmdInfo::C_PROG :	{
					std::vector<char*> argv;
					cmdInfo->getArgs(argv);
					res = execv(cmdInfo->command.c_str(), &argv[0]);	
					if (res == -1) {
						std::string path = "/bin/";
						path += cmdInfo->command;
						res = execv(path.c_str(), &argv[0]);	
					}	
					procError(res, "cli", true);	
					break; 
				}
	default : return;
	}
}

void Executor::execProg(CmdInfoPtr cmdInfo)
{
	int res = 0;
	pid_t processId = fork();
	procError(processId, "fork");	
	if (processId == 0) {
		redirectIO(cmdInfo);
		execute(cmdInfo);
	} else {
		int statLoc;
		res = waitpid(processId, &statLoc, __WALL);
		procError(res, "wait");
	}

}		

void Executor::redirectIO(CmdInfoPtr cmdInfo)
{
	int res = 0;	
	if (!cmdInfo->input.empty()) {
		close(0);
		res = open(cmdInfo->input.c_str(), O_RDONLY);
		procError(res, "open");
	}
	if (!cmdInfo->output.empty()) {
		close(1);
		res = open(cmdInfo->output.c_str(), O_WRONLY | O_CREAT | O_TRUNC);
		procError(res, "open");
	}



}
	
void Executor::procError(int& res, const char* reason, bool isFatal) 
{
	if (res == -1) {
		perror(reason);
		res = 0;
		if (isFatal) {
			exit(EXIT_FAILURE);	
		}
	}
}
