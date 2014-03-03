#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "cmdinfo.h"


class Executor
{
	int stdIn;
	int stdOut;
	bool isInRedirect;
	bool isOutRedirect;
	int fileIn;
	int fileOut;
	void execLs(CmdInfoPtr cmdInfo);
	void execPwd(CmdInfoPtr cmdInfo);
	void execCd(CmdInfoPtr cmdInfo);
	static void (*mysignal(int signo, void (*hndlr) (int))) (int); 
	void returnStdIO();
	void execExit(CmdInfoPtr cmdInfo);
	void execKill(CmdInfoPtr cmdInfo);
	void execute(CmdInfoPtr cmdInfo);
	static int select(const struct dirent * d); 
	void redirectIO(CmdInfoPtr cmdInfo);
	void execProg(CmdInfoPtr cmdInfo);
	void procError(int& res, const char* reason, bool isFatal = false); 
public:
	void exec(CmdInfoPtr cmdInfo);
	Executor() : isInRedirect(false), isOutRedirect(false) {}


};



#endif //EXECUTOR_H
