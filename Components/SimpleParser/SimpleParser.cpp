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

	SimpleParser::Error SimpleParser::nextToken(char* buf, size_t& reader, size_t buflen, int& line, Token& token, bool ws_relevant)
	{
		char* s_start;

		while (reader < buflen)
		{
			switch (buf[reader]) {
			case '=':
				token.type = TokenType::TOKEN_EQUEAL;
				reader++;
				return Error::PARSE_OK;
			case '>':
				token.type = TokenType::TOKEN_GREATER;
				reader++;
				return Error::PARSE_OK;
			case '<':
				token.type = TokenType::TOKEN_LESS;
				reader++;
				return Error::PARSE_OK;
			case '+':
				token.type = TokenType::TOKEN_PLUS;
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
			case '{':
				token.type = TokenType::TOKEN_LEFT_CURLYBRACKET;
				reader++;
				return Error::PARSE_OK;
			case '}':
				token.type = TokenType::TOKEN_RIGHT_CURLYBRACKET;
				reader++;
				return Error::PARSE_OK;
			case ',':
				token.type = TokenType::TOKEN_COMMA;
				reader++;
				return Error::PARSE_OK;
			case '.':
				token.type = TokenType::TOKEN_DOT;
				reader++;
				return Error::PARSE_OK;
			case ':':
				token.type = TokenType::TOKEN_COLON;
				reader++;
				return Error::PARSE_OK;
			case ';':
				token.type = TokenType::TOKEN_SEMICOLON;
				reader++;
				return Error::PARSE_OK;
			case '\n':				
				reader++;
				line++;
				if (ws_relevant) {
					while (reader < buflen && (buf[reader] == '\n' || buf[reader] == ' ' || buf[reader] == '\t'))
						reader++;
					token.type = TokenType::TOKEN_WHITESPACE;
					return Error::PARSE_OK;
				}
				if (!m_ignore_newline) {
					token.type = TokenType::TOKEN_NEWLINE;
					return Error::PARSE_OK;
				}
				break;
			case '#':
				if (!m_comment_type) {
					// a comment - skip until end of line
					while (reader < buflen && buf[reader] != '\n')
						reader++;
					line++;
					if (reader == buflen)
						return Error::PARSE_EOF;
					reader++;
					break;
				}
				else {
					token.type = TokenType::TOKEN_HASH;
					reader++;
					return Error::PARSE_OK;
				}
			case '~':
				token.type = TokenType::TOKEN_TILDE;
				reader++;
				return Error::PARSE_OK;
			case ' ':
			case '\t':
				// skip whitespace
				reader++;
				if (ws_relevant) {
					while (reader < buflen && (buf[reader] == '\n' || buf[reader] == ' ' || buf[reader] == '\t'))
						reader++;
					token.type = TokenType::TOKEN_WHITESPACE;
					return Error::PARSE_OK;
				}
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
			case '/': // NOTE: must be the last one before default
				// c-style comments
				if (m_comment_type == 1) {
					if (reader + 1 < buflen && buf[reader + 1] == '*') {
						// is a comment - search for end, if not found silently consumes the whole document
						reader += 3;
						while (reader < buflen && (buf[reader - 1] != '*' || buf[reader] != '/'))
							reader++;
						reader++;
						break;
					}
				}
			default:
				// number value
				int neg = 1;
				bool is_float = false;
				if ((buf[reader] >= '0' && buf[reader] <= '9') || buf[reader] == '-') {
					s_start = &buf[reader];
					if (buf[reader] == '-') {
						neg = -1;
						reader++;
						s_start++;
						if (!(reader < buflen))
							return Error::PARSE_ERR;
					}
					while (reader < buflen && ((buf[reader] >= '0' && buf[reader] <= '9') || buf[reader] == '.')) {
						if (buf[reader] == '.')
							if (!is_float)
								is_float = true;
							else
								return Error::PARSE_ERR; // no multiple dots
						reader++;
					}
					// dont allow empty numbers
					if (&buf[reader] == s_start)
						return Error::PARSE_ERR;
					if (is_float) {
						token.value = neg * std::stod(std::string(s_start, &buf[reader] - s_start));
						// ignore trailing float
						if (reader < buflen && buf[reader] == 'f')
							reader++;
					}
					else
						token.value = neg * std::stoi(std::string(s_start, &buf[reader] - s_start));
					//token.value = std::string(s_start, &buf[reader] - s_start);
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
							strchr(id_chars, buf[reader])
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