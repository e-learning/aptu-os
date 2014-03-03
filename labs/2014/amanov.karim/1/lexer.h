#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <string>
#include "token.h"


class Lexer
{
	size_t offset_;
	std::string delims_;
	std::vector<Token> tokenBuffer_;
	bool skipSpaces(const std::string& text);
	bool getToken(const std::string& input, Token& token);
	void defTokenType(Token& token, const  std::string& value);
	bool defTokenValue(const std::string& text, std::string& value);

public:
	Lexer() : offset_(0), delims_("<>| ") {}
	std::vector<Token>& getTokenBuffer() { return tokenBuffer_; }
	bool startLexAnalyze(const std::string& text);

};




#endif //LEXER_H
