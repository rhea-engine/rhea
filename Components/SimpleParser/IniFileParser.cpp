#include <string>
#include <unordered_map>
#include <memory>

#include "IniFileParser.h"

namespace SimpleParser
{
	char* IniFileParser::getErrMsg(Error err)
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
		case Error::PARSE_EXPECT_HEADER_START:
			return "Expected section start '['";
		case Error::PARSE_EXPECT_HEADER_END:
			return "Expected header end ']'";
		case Error::PARSE_EXPECT_HEADER_IDENTIFIER:
			return "Expected identifier in section header";

		default:
			return "Syntax error";
		}
	}

	IniFileParser::Error IniFileParser::parseHeader(char* buf, size_t& reader, size_t buflen, int& line, std::string& path)
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
					if (err == SimpleParser::Error::PARSE_EOF)
						return Error::PARSE_EOF;
					else {
						if (token.type == TokenType::TOKEN_LEFT_BRACKET) {
							// section start
							state = 1;
							break;
						}
						else {
							return Error::PARSE_EXPECT_HEADER_START;
						}
					}
					break;
				case 1: // path
					if (err == SimpleParser::Error::PARSE_EOF ||
						(token.type != TokenType::TOKEN_IDENTIFIER) &&
						(token.type != TokenType::TOKEN_STRING))
						return Error::PARSE_EXPECT_HEADER_IDENTIFIER;
					else {
						path = token.value.toString();
						state = 2;
					}
					break;
				case 2: // end of section
					if (err == SimpleParser::Error::PARSE_EOF ||
						token.type != TokenType::TOKEN_RIGHT_BRACKET) {
						return Error::PARSE_EXPECT_HEADER_END;
					}
					else {
						return Error::PARSE_OK;
					}
				}
			}
		}
		return Error::PARSE_OK;
	}

	IniFileParser::Error IniFileParser::parseContent(char* buf, size_t& reader, size_t buflen, int& line, values_map& values)
	{
		Error err;
		std::string key;
		Variant value;

		while (reader < buflen) {
			err = parseContentEntry(buf, reader, buflen, line, key, value);
			if (err == Error::PARSE_EOF)
				return Error::PARSE_EOF;
			else if (err != Error::PARSE_OK)
				return err;
			else
				values[key] = value;
		}
		return Error::PARSE_OK;
	}

	IniFileParser::Error IniFileParser::parseContentEntry(char* buf, size_t& reader, size_t buflen, int& line, std::string& key, Variant& value)
	{
		Token token;
		SimpleParser::Error err;
		std::vector<Variant> arr;

		int state = 0;
		while (reader < buflen) {
			err = nextToken(buf, reader, buflen, line, token);
			if (err >= SimpleParser::Error::PARSE_ERR)
				return Error::PARSE_ERR;
			else {
				switch (state) {
				case 0:
					if (err == SimpleParser::Error::PARSE_EOF)
						return Error::PARSE_EOF;
					else {
						if (token.type == TokenType::TOKEN_LEFT_BRACKET) {
							// next section - push token back
							reader--;
							return Error::PARSE_EOF;
						}
						else if (token.type == TokenType::TOKEN_IDENTIFIER ||
							token.type == TokenType::TOKEN_STRING) {
							key = token.value.toString();
							state = 1;
						}						
						else {
							return Error::PARSE_EXPECT_IDENTIFIER;
						}
					}
					break;
				case 1: // assign
					if (err == SimpleParser::Error::PARSE_EOF ||
						token.type != TokenType::TOKEN_EQUEAL)
						return Error::PARSE_EXPECT_ASSIGN;
					else {
						state = 2;
					}
					break;
				case 2:
					if (err == SimpleParser::Error::PARSE_EOF) {
						// an empty value is allowed
						value = "";
						return Error::PARSE_EOF;
					}
					else if (token.type == TokenType::TOKEN_LEFT_PARENTHESIS) {
						// start of array
						state = 3;
					}
					else {
						value = token.value;
						return Error::PARSE_OK;
					}
					break;
				case 3: // in array
					if (token.type == TokenType::TOKEN_NUMBER ||
						token.type == TokenType::TOKEN_STRING ||
						token.type == TokenType::TOKEN_IDENTIFIER)
					{
						arr.push_back(token.value);
					}
					else if (token.type == TokenType::TOKEN_COMMA) {
						// simply ignore
						continue;
					}
					else if (err == SimpleParser::Error::PARSE_EOF ||
						token.type != TokenType::TOKEN_RIGHT_PARENTHESIS) {
						return Error::PARSE_UNERMINATED_ARRAY;
					}
					else {
						value = arr;
						return Error::PARSE_OK;
					}
				}
			}
		}
		return Error::PARSE_OK;
	}

	IniFileParser::Error IniFileParser::parseSection(char* buf, size_t& reader, size_t buflen, int& line, IniFileParser::Section& section)
	{
		Error err;

		// parse a section header
		err = parseHeader(buf, reader, buflen, line, section.section_name);
		if (err != Error::PARSE_OK) {
			return err;
		}

		// parse section content
		section.attributes.clear();
		section.values.clear();
		err = parseContent(buf, reader, buflen, line, section.values);

		// continue on eof: empty sections are allowd
		if (err != Error::PARSE_OK && err != Error::PARSE_EOF)
			return err;

		return Error::PARSE_OK;
	}

	enum class REGFILE_STATE {
		REGFILE_NONE,
		REGFILE_END,
		REGFILE_HEADER,
		REGFILE_KEY,
		REGFILE_ASSIGN,
		REGFILE_VALUE,
		REGFILE_COMMENT
	};

	int IniFileParser::parse(char* buf, size_t buflen, std::vector<IniFileParser::Section>& sections)
	{
		Token next_token;
		Error err;
		std::string path;
		int line = 1;
		int entries = 0;
		IniFileParser::Section section;

		size_t reader = 0;
		while (reader < buflen)
		{
			err = parseSection(buf, reader, buflen, line, section);
			if (err == Error::PARSE_OK) {
				sections.push_back(section);
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
}