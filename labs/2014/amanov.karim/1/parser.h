#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "token.h"
#include "cmdinfo.h"
#include <stack>

class Parser 
{
	std::vector<Token>& tokenBuffer_;
	std::vector<Token>::const_iterator currentToken_;
	std::stack<CmdInfoPtr> sPipe;
	bool isPipe;
	void nextToken();
    	void raiseSyntaxError() const;
    	bool checkType(Token::TokenType type) const;
    	void assertType(Token::TokenType type) const;	
	void parseCommand();
	void parseCmdType(CmdInfoPtr cmdInfo); 
	void parseArgument(CmdInfoPtr info);
public:
	Parser(std::vector<Token>& tokenBuffer) 
		: tokenBuffer_(tokenBuffer)
		, currentToken_(tokenBuffer_.begin())
		, isPipe(false)
	{}
	 
	CmdInfoPtr parse();

};



#endif //PARSER_H
