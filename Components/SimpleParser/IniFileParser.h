#ifndef SIMPLEPARSER_INI_H
#define SIMPLEPARSER_INI_H

#include <cstdio>
#include <vector>
#include <unordered_map>
#include <string>
#include "SimpleParser.h"

namespace SimpleParser
{
	class IniFileParser : public SimpleParser
	{
	public:
		enum class Error
		{
			// base class errors
			PARSE_OK,
			PARSE_EOF,
			PARSE_ERR,
			PARSE_UNTERMINATED_STRING,
			PARSE_INVALID_ID_CHAR,
			// IniFileParser
			PARSE_EXPECT_IDENTIFIER,
			PARSE_EXPECT_ASSIGN,
			PARSE_EXPECT_HEADER_START,
			PARSE_EXPECT_HEADER_END,
			PARSE_EXPECT_HEADER_IDENTIFIER,

		};

		struct Section
		{
			std::string section_name;
			std::unordered_map<std::string, std::string> attributes;
			std::unordered_map<std::string, std::string> values;
		};

		// parse a buffer into sections
		int parse(char* buf, size_t buflen, std::vector<IniFileParser::Section>& sections);

		// get english error message
		char* getErrMsg(Error err);
	protected:
		Error parseSection(char* buf, size_t& reader, size_t buflen, int& line, Section &section);
		Error parseHeader(char* buf, size_t& reader, size_t buflen, int& line, std::string& path);
		Error parseContent(char* buf, size_t& reader, size_t buflen, int& line, std::unordered_map<std::string, std::string>& values);
		Error parseContentEntry(char* buf, size_t& reader, size_t buflen, int& line, std::string& key, std::string& value);
	};
}
#endif // !SIMPLEPARSER_INI_H
