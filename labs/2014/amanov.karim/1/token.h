#ifndef TOKEN_H
#define TOKEN_H

#include <string>

class Token
{

public:
	enum TokenType {
		TPIPE,
		TINPUT_REDIRECT,
		TOUTPUT_REDIRECT,
		TLITERAL,
		TEOF
	};
	

	Token() {}
	TokenType getType() const { return type_; }
	const std::string& getValue() const { return value_; }
	void setType(TokenType type) { type_ = type; }
	void setValue(const std::string& value) { value_ = value; }

private:
	std::string value_;
	TokenType type_;
};


#endif //TOKEN_H
