#include "interpreter.h"
#include <iostream>
#include <cstdlib>
#include <sstream>
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "executor.h"
#include <signal.h>
#include <unistd.h>

void Interpreter::run()
{
	mysignal(SIGINT, sigHndlr);
	std::string user_name = std::getenv("USER");
	
	while(true) {
		std::cout << user_name << "@:" << get_current_dir_name() << ":> ";
		startInterpret();
	}		
}

void Interpreter::startInterpret() 
{
	try {
	std::string input;
	std::getline(std::cin, input);
	Lexer lexer;
	if(!lexer.startLexAnalyze(input))
		return;	
	Parser parser(lexer.getTokenBuffer());
	CmdInfoPtr cmdInfo = parser.parse();
	Executor executor;
	executor.exec(cmdInfo);
	} catch(std::exception& exception) {
		std::cerr << exception.what() << std::endl;
	}
}




void (*Interpreter::mysignal(int signo, void (*hndlr) (int))) (int) 
{
	struct sigaction act, oact;
	act.sa_handler = hndlr;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signo != SIGALRM)
		act.sa_flags |= SA_RESTART; 
	if (sigaction(signo, &act, &oact) < 0)
		return (SIG_ERR);
	return (oact.sa_handler);
}

void Interpreter::sigHndlr(int signo)
{

	std::cout << std::endl << "Ctrl + C pressed" << std::endl;
}
