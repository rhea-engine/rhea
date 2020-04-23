#ifndef SIMPLEPARSER_H

#include <string>

namespace SimpleParser
{
	enum class TokenType {
		TOKEN_STRING,
		TOKEN_NUMBER,
		TOKEN_IDENTIFIER,
		TOKEN_EQUEAL,
		TOKEN_LEFT_PARENTHESIS,
		TOKEN_RIGHT_PARENTHESIS,
		TOKEN_LEFT_BRACKET,
		TOKEN_RIGHT_BRACKET,
		TOKEN_NEWLINE
	};

	struct Token
	{
		TokenType type = TokenType::TOKEN_STRING;
		std::string value;
	};

	class State
	{
	public:
		int error = 0;
		int line = 0;
		std::string error_message;
	};

	class SimpleParser
	{
	public:
		enum class Error {
			PARSE_OK,
			PARSE_EOF,
			PARSE_ERR,
			PARSE_UNTERMINATED_STRING,
			PARSE_INVALID_ID_CHAR
		};
		
		SimpleParser() :
			m_ignore_newline(true)
		{}
		SimpleParser(bool ignore_newline) :
			m_ignore_newline(ignore_newline)
		{}
		// get the next token
		Error nextToken(char* buf, size_t& reader, size_t buflen, int &line, Token& token);
		// get an error message
		char* getErrMsg(Error);
	protected:
		bool m_ignore_newline;
	};
}
#endif // !SIMPLEPARSER_H
