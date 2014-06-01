#ifndef CMDINFO_H
#define CMDINFO_H

#include <string>
#include <memory>
#include <vector>

struct CmdInfo;
typedef std::shared_ptr<CmdInfo> CmdInfoPtr;

struct CmdInfo
{
	enum CMDTYPE {
		C_LS,
		C_PWD,
		C_PS,
		C_EXIT,
		C_KILL,
		C_CD,
		C_PROG
	};

	CMDTYPE cmdType;
	unsigned int argc;
	std::string command;
	std::string input;
	std::string output;
	CmdInfoPtr pipeCmd;
	bool isPipe;
	std::vector<std::string> args;
	CmdInfo() : cmdType(C_PROG),  argc(0), isPipe(false) {}
	void setCommand(const std::string& cmd) { 
		args.push_back(cmd); 
		if(cmdType == C_PROG) {
			command = cmd;
		 }
	}
	void addArg(const std::string& arg) { args.push_back(arg);}
	void  getArgs(std::vector<char*>& argv) { 
		argc = args.size();
		for(unsigned int i = 0; i < argc; i++) {
			argv.push_back(&args[i][0]);
		}
		argv.push_back(NULL);
	}	
};



#endif //CMDINFO_H
