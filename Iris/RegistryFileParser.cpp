#include <string>
#include <unordered_map>
#include <memory>

#include "Registry.h"
#include "RegistryFileParser.h"
#include "Entry.h"

using simpleparser::TokenType;

char* RegistryFileParser::getErrMsg(Error err)
{
	switch (err) {
	case Error::PARSE_OK:
		return "OK";
	case Error::PARSE_EOF:
		return "End of file";
	case Error::PARSE_ERROR:
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

RegistryFileParser::Error RegistryFileParser::parseHeader(char* buf, size_t& reader, size_t buflen, int &line, std::string& path)
{
	simpleparser::Token token;
	simpleparser::SimpleParser::Error err;

	int state = 0;

	while (reader < buflen) {
		err = parser->nextToken(buf, reader, buflen, line, token);
		if (err >= simpleparser::SimpleParser::Error::PARSE_ERR)
			return Error::PARSE_ERROR;
		else {
			switch (state) {
			case 0:
				if (err == simpleparser::SimpleParser::Error::PARSE_EOF)
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
				if (err == simpleparser::SimpleParser::Error::PARSE_EOF ||
					(token.type != TokenType::TOKEN_IDENTIFIER) && 
					(token.type != TokenType::TOKEN_STRING))
					return Error::PARSE_EXPECT_HEADER_IDENTIFIER;
				else {
					path = token.value;
					state = 2;
				}
				break;
			case 2:
				if (err == simpleparser::SimpleParser::Error::PARSE_EOF ||
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

RegistryFileParser::Error RegistryFileParser::parseContent(char* buf, size_t& reader, size_t buflen, int &line, std::unordered_map<std::string, std::string>& values)
{
	Error err;
	std::string key;
	std::string value;

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

RegistryFileParser::Error RegistryFileParser::parseContentEntry(char* buf, size_t& reader, size_t buflen, int &line, std::string& key, std::string &value)
{
	simpleparser::Token token;
	simpleparser::SimpleParser::Error err;

	int state = 0;
	while (reader < buflen) {
		err = parser->nextToken(buf, reader, buflen, line, token);
		if (err >= simpleparser::SimpleParser::Error::PARSE_ERR)
			return Error::PARSE_ERROR;
		else {
			switch (state) {
			case 0:
				if (err == simpleparser::SimpleParser::Error::PARSE_EOF)
					return Error::PARSE_EOF;
				else {
					if (token.type == TokenType::TOKEN_LEFT_BRACKET) {
						// next section - push token back
						reader--;
						return Error::PARSE_EOF;
					}
					else if (token.type == TokenType::TOKEN_IDENTIFIER ||
						token.type == TokenType::TOKEN_STRING) {
						key = token.value;
						state = 1;
					}
					else {
						return Error::PARSE_EXPECT_IDENTIFIER;
					}
				}
				break;
			case 1: // assign
				if (err == simpleparser::SimpleParser::Error::PARSE_EOF ||
					token.type != TokenType::TOKEN_EQUEAL)
					return Error::PARSE_EXPECT_ASSIGN;
				else {
					state = 2;
				}
				break;
			case 2:
				if (err == simpleparser::SimpleParser::Error::PARSE_EOF) {
					// an empty value is allowed
					value = "";
					return Error::PARSE_EOF;
				}
				else {
					value = token.value;
					return Error::PARSE_OK;
				}
			}
		}
	}
	return Error::PARSE_OK;
}

RegistryFileParser::Error RegistryFileParser::parseSection(char* buf, size_t& reader, size_t buflen, int &line, std::string &path, Entry** entry)
{
	Error err;	
	std::unordered_map<std::string, std::string> values;
	
	// defaults
	values["name"] = "(unknown)";
	values["type"] = "(unknown)";

	// parse a section header
	err = parseHeader(buf, reader, buflen, line, path);
	if (err != Error::PARSE_OK) {
		return err;
	}
	// parse section content
	err = parseContent(buf, reader, buflen, line, values);

	// continue on eof: empty sections are allowd
	if (err != Error::PARSE_OK && err != Error::PARSE_EOF)
		return err;

	*entry = new Entry(values["name"], values["type"]);
	for (const auto& val : values)
	{
		if (!val.first.compare("name") ||
			!val.first.compare("type"))
			continue;
		(*entry)->setValue(val.first, val.second);
	}

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

int RegistryFileParser::parse(char* buf, size_t buflen)
{
	Entry* entry;
	parser = std::make_unique<simpleparser::SimpleParser>();
	simpleparser::Token next_token;
	Error err;
	std::string path;
	int line = 1;
	int entries = 0;

	size_t reader = 0;
	while (reader < buflen)
	{
		err = parseSection(buf, reader, buflen, line, path, &entry);
		if (err == Error::PARSE_OK) {
			m_registry->insert(path, entry);
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