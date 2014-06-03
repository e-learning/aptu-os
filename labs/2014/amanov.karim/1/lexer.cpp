#include "lexer.h"
#include <sstream>
#include <cstring>
#include <string>

bool Lexer::startLexAnalyze(const std::string& text) 
{
	unsigned int textLength = text.size();
	while(offset_ != textLength) {
		Token token;
		if(getToken(text, token)) {
			tokenBuffer_.push_back(token);
		}

	}
	if(tokenBuffer_.empty())
		return false;
	Token token;
	token.setType(Token::TEOF);
	tokenBuffer_.push_back(token);
	return true;
}




bool Lexer::getToken(const std::string& text, Token& token)
{
	if(!skipSpaces(text))
		return false;
	
	std::string value;
	if(!defTokenValue(text, value))
		return false;

	defTokenType(token, value);
	return true;
}

bool Lexer::defTokenValue(const std::string& text, std::string& value)
{
	if(offset_ == text.size())
		return false;
	size_t nextPos = text.find_first_of(delims_, offset_);

	if(nextPos == std::string::npos) {
		value = text.substr(offset_);
		offset_ = text.length();
	}
	else {
		if(nextPos == offset_) {
			nextPos++;
		}

		value = text.substr(offset_, nextPos - offset_);
		offset_ = nextPos;
	}
	return true;
}

void Lexer::defTokenType(Token& token, const std::string& value)
{

	if (value == "<") {
        token.setType(Token::TINPUT_REDIRECT);
	} else if (value == ">") {
		token.setType(Token::TOUTPUT_REDIRECT);
	} else if (value == "|") {
		token.setType(Token::TPIPE);
	}
	else {
		token.setType(Token::TLITERAL);
		token.setValue(value);
	}

}


bool Lexer::skipSpaces(const std::string& text)
{
	if(strchr(" ",text[offset_])) {
		offset_ = text.find_first_not_of(" ", offset_);
		if(offset_ == std::string::npos) {
			offset_ = text.length();
			return false;
		}
	}
	return true;
}

