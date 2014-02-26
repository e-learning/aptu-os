#include <vector>
#include <string>

struct Command{
	std::string cmd;
	std::vector<std::string> args;
	bool hasNoArguments();
};

class Executor{
	public:
	 // Executor();
	 bool execute(Command const & c);
	private:
	 void executeKillCommand(Command const & c);
	 void executeLsCommand(Command const & c);
	 void executePwdCommand(Command const & c);
	 void executePsCommand(Command const & c);
	 void executeOtherCommand(Command const & c);	

	 void killProcess(int pid, int sig);
	 void listFiles();
	 void showDirPath();
	 void showProcessList();
};
