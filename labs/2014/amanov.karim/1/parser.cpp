#include "parser.h"
#include <stdexcept>
#include <memory>

CmdInfoPtr Parser::parse() 
{
	
	parseCommand();
	return sPipe.top();
}

void Parser::parseCommand()
{
	assertType(Token::TLITERAL);
	CmdInfoPtr cmdInfo = CmdInfoPtr(new CmdInfo());
	if (isPipe) {
		cmdInfo->isPipe = true;
		cmdInfo->pipeCmd = sPipe.top();
		isPipe = false;
	}
	parseCmdType(cmdInfo);
	nextToken();
	sPipe.push(cmdInfo);	
	while(!checkType(Token::TEOF)) {	
		parseArgument(cmdInfo);
	}

}

void Parser::parseCmdType(CmdInfoPtr cmdInfo) 
{
	const std::string& cmdName = currentToken_->getValue();
	if (cmdName == "ls") {
		cmdInfo->cmdType = CmdInfo::C_LS;
	} else if (cmdName == "pwd") {
		cmdInfo->cmdType = CmdInfo::C_PWD;
	} else if (cmdName == "ps") {
		cmdInfo->cmdType = CmdInfo::C_PS;
	} else if (cmdName == "kill") {
		cmdInfo->cmdType = CmdInfo::C_KILL;
	} else if (cmdName == "cd") {
		cmdInfo->cmdType = CmdInfo::C_CD;
	} else if (cmdName == "exit") {
		cmdInfo->cmdType = CmdInfo::C_EXIT;
	} else {
		cmdInfo->cmdType = CmdInfo::C_PROG;
	}
	cmdInfo->setCommand(cmdName);
}


void Parser::parseArgument(CmdInfoPtr info)
{
	if (checkType(Token::TINPUT_REDIRECT)) {
		nextToken();
		assertType(Token::TLITERAL);
		info->input = currentToken_->getValue();
		nextToken();
	} else if (checkType(Token::TOUTPUT_REDIRECT)) {
		nextToken();
		assertType(Token::TLITERAL);
		info->output = currentToken_->getValue();
		nextToken();
 	} else if (checkType(Token::TLITERAL)) {
		info->addArg(currentToken_->getValue());
		nextToken();
	} else if (checkType(Token::TPIPE)) {
		nextToken();
		isPipe = true;
		parseCommand();
	}
		

	
}



void Parser::nextToken()
{
	if(currentToken_ != tokenBuffer_.end() - 1) {
		++currentToken_;
	}

}

bool Parser::checkType(Token::TokenType type) const
{
    	return currentToken_->getType() == type;
}



void Parser::assertType(Token::TokenType type) const
{
    	if(!checkType(type))
        	raiseSyntaxError();
}

void Parser::raiseSyntaxError() const
{
    	throw std::logic_error("Syntax error");
}
