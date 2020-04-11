#include "SimpleParser.h"

namespace SimpleParser
{
	char* SimpleParser::getErrMsg(Error err)
	{
		switch (err) {
		case Error::PARSE_OK:
			return "OK";
		case Error::PARSE_EOF:
			return "End of file";
		case Error::PARSE_ERR:
			return "Parse error";
		case Error::PARSE_UNTERMINATED_STRING:
			return "Unterminated string";
		case Error::PARSE_INVALID_ID_CHAR:
			return "Invalid character in identifier";
		}
		return "";
	}

	Error SimpleParser::nextToken(char* buf, size_t& reader, size_t buflen, int& line, Token& token)
	{
		char* s_start;

		while (reader < buflen)
		{
			switch (buf[reader]) {
			case '=':
				token.type = TokenType::TOKEN_EQUEAL;
				reader++;
				return Error::PARSE_OK;
			case '(':
				token.type = TokenType::TOKEN_LEFT_PARENTHESIS;
				reader++; 
				return Error::PARSE_OK;
			case ')':
				token.type = TokenType::TOKEN_RIGHT_PARENTHESIS;
				reader++; 
				return Error::PARSE_OK;
			case '[':
				token.type = TokenType::TOKEN_LEFT_BRACKET;
				reader++; 
				return Error::PARSE_OK;
			case ']':
				token.type = TokenType::TOKEN_RIGHT_BRACKET;
				reader++; 
				return Error::PARSE_OK;
			case '\n':
				reader++;
				line++;
				if (!m_ignore_newline) {
					token.type = TokenType::TOKEN_NEWLINE;
					return Error::PARSE_OK;
				}
				break;
			case '#':
				// a comment - skip until end of line
				while (reader < buflen && buf[reader] != '\n')
					reader++;
				line++;
				if (reader == buflen)
					return Error::PARSE_EOF;
				reader++;
				break;
			case ' ':
				// skip whitespace
				reader++;
				break;
			case '"':
				// string value
				reader++;
				if (reader == buflen) {
					return Error::PARSE_UNTERMINATED_STRING;
				}
				s_start = &buf[reader];
				while (reader < buflen && buf[reader] != '"') {
					if (buf[reader] == '\n')
						line++;
					reader++;
				}
				if (reader == buflen)
					return Error::PARSE_ERR;
				token.value = std::string(s_start, &buf[reader] - s_start);
				token.type = TokenType::TOKEN_STRING;
				reader++;
				return Error::PARSE_OK;
			default:
				// number value
				if (buf[reader] >= 0 && buf[reader] <= 9) {
					s_start = &buf[reader];
					while (reader < buflen && (buf[reader] >= 0 && buf[reader] <= 9))
						reader++;
					token.value = std::string(s_start, &buf[reader] - s_start);
					token.type = TokenType::TOKEN_NUMBER;
					return Error::PARSE_OK;
				}
				else {
					// identifier
					s_start = &buf[reader];
					while (reader < buflen &&
						((buf[reader] >= 'a' && buf[reader] <= 'z') ||
							(buf[reader] >= 'A' && buf[reader] <= 'Z') ||
							(buf[reader] >= '0' && buf[reader] <= '9') ||
							buf[reader] == '_' || buf[reader] == ':' ||
							buf[reader] == '/'
							))
						reader++;
					if (&buf[reader] - s_start == 0)
						return Error::PARSE_EOF;
					token.value = std::string(s_start, &buf[reader] - s_start);
					token.type = TokenType::TOKEN_IDENTIFIER;
					return Error::PARSE_OK;
				}
			}
		}
		return Error::PARSE_EOF;
	}
}