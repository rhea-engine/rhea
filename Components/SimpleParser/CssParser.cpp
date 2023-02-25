#include "CssParser.h"

namespace SimpleParser
{
	CssParser::Error CssParser::parseSelector(char* buf, size_t& reader, size_t buflen, int& line, Selector& name)
	{
		Token token;
		SimpleParser::Error err;

		int state = 0;

		while (reader < buflen) {
			err = nextToken(buf, reader, buflen, line, token);
			if (err >= SimpleParser::Error::PARSE_ERR)
				return Error::PARSE_ERR;
			else {
				switch (state) {
				case 0:
					if (err == SimpleParser::Error::PARSE_EOF ||
						(token.type != TokenType::TOKEN_IDENTIFIER) &&
						(token.type != TokenType::TOKEN_STRING))
						return Error::PARSE_EXPECT_IDENTIFIER;
					else {
						name.name = token.value;
						state = 1;
					}
					break;
				case 1:
					if (err == SimpleParser::Error::PARSE_EOF ||
						token.type != TokenType::TOKEN_LEFT_CURLYBRACKET) {
						return Error::PARSE_EXPECT_CONTENT_START;
					}
					else {
						return Error::PARSE_OK;
					}
					break;
				}
			}
		}
	}

	CssParser::Error CssParser::parseContent(char* buf, size_t& reader, size_t buflen, int& line, PropertyMap& property_map)
	{
		Token token;
		SimpleParser::Error err;
		std::string prop_name;
		std::vector<Variant> properties;
		std::string value;

		int state = 0;
		id_chars = "-";
		while (reader < buflen) {
			err = nextToken(buf, reader, buflen, line, token);
			if (err >= SimpleParser::Error::PARSE_ERR)
				return Error::PARSE_ERR;
			else {
				switch (state) {
				case 0:
					if (err == SimpleParser::Error::PARSE_EOF)
						return Error::PARSE_EXPECT_IDENTIFIER; 
					if (token.type == TokenType::TOKEN_RIGHT_CURLYBRACKET) {
						return Error::PARSE_OK;
					}
					else if (token.type != TokenType::TOKEN_IDENTIFIER &&
						token.type != TokenType::TOKEN_STRING)
						return Error::PARSE_EXPECT_IDENTIFIER;
					else {
						prop_name = token.value;
						properties.clear();
						state = 1;				
					}
					break;
				case 1:
					if (err == SimpleParser::Error::PARSE_EOF ||
						(token.type != TokenType::TOKEN_COLON))
						return Error::PARSE_EXPECT_IDENTIFIER;
					else {
						state = 2;
						id_chars = ":/-()'.";
						value = "";
					}
					break;
				case 2:
					if (err == SimpleParser::Error::PARSE_EOF)
						return Error::PARSE_EXPECT_IDENTIFIER;
					else {
						if (token.type == TokenType::TOKEN_COMMA) {
							if (value.compare(""))
								properties.push_back(value);
							value = "";
						}
						else if (token.type == TokenType::TOKEN_SEMICOLON) {
							if (value.compare(""))
								properties.push_back(value);
							if (properties.size())
								property_map[prop_name] = properties;
							state = 0;
							id_chars = "-";
						}
						else if (token.type == TokenType::TOKEN_RIGHT_CURLYBRACKET) {
							if (value.compare(""))
								properties.push_back(value);
							if (properties.size())
								property_map[prop_name] = properties;
							return Error::PARSE_OK;
						}
						else {
							if (value.compare(""))
								value += std::string(" ") + (std::string)token.value;
							else
								value = token.value;
						}
					}
				}
			}
		}
		return Error::PARSE_EOF;
	}

	CssParser::Error CssParser::parseEntry(char* buf, size_t& reader, size_t buflen, int& line, Entry& entry)
	{
		Error err;

		// parse a selector
		err = parseSelector(buf, reader, buflen, line, entry.selector);
		if (err != Error::PARSE_OK) {
			return err;
		}

		// parse entry content
		entry.properties.clear();
		err = parseContent(buf, reader, buflen, line, entry.properties);

		// continue on eof: empty sections are allowd
		if (err != Error::PARSE_OK && err != Error::PARSE_EOF)
			return err;

		return Error::PARSE_OK;
	}

	int CssParser::parse(char* buf, size_t buflen, std::vector<CssParser::Entry>& css)
	{
		Token next_token;
		Error err;
		std::string path;
		int line = 1;
		int entries = 0;
		CssParser::Entry entry;

		size_t reader = 0;
		m_ignore_newline = true;
		while (reader < buflen)
		{
			err = parseEntry(buf, reader, buflen, line, entry);
			if (err == Error::PARSE_OK) {
				css.push_back(entry);
				entries++;
			}
			else if (err == Error::PARSE_EOF)
				break;
			else {
				printf("%s in line %d\n", getErrMsg(err), line);
				return entries;
			}
		}
		return entries;
	}

	char* CssParser::getErrMsg(Error err)
	{
		switch (err) {
		case Error::PARSE_OK:
			return "OK";
		case Error::PARSE_EOF:
			return "End of file";
		case Error::PARSE_ERR:
			return "Syntax error";
		case Error::PARSE_EXPECT_IDENTIFIER:
			return "Identifier expected";
		case Error::PARSE_EXPECT_ASSIGN:
			return "Assign expression expected";
		case Error::PARSE_EXPECT_CONTENT_START:
			return "Expected content start '{'";
		case Error::PARSE_EXPECT_CONTENT_END:
			return "Expected header end '}'";

		default:
			return "Syntax error";
		}
	}
}