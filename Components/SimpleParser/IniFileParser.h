/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef SIMPLEPARSER_INI_H
#define SIMPLEPARSER_INI_H

#include <cstdio>
#include <vector>
#include <unordered_map>
#include <string>
#include "SimpleParser.h"

namespace simpleparser
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
			PARSE_UNERMINATED_ARRAY,
			PARSE_INVALID_ID_CHAR,
			// IniFileParser
			PARSE_EXPECT_IDENTIFIER,
			PARSE_EXPECT_ASSIGN,
			PARSE_EXPECT_HEADER_START,
			PARSE_EXPECT_HEADER_END,
			PARSE_EXPECT_HEADER_IDENTIFIER,
		};

		typedef std::unordered_map<std::string, Variant> values_map;

		struct Section
		{
			std::string section_name;
			std::unordered_map<std::string, std::string> attributes;
			values_map values;
		};

		// parse a buffer into sections
		int parse(char* buf, size_t buflen, std::vector<IniFileParser::Section>& sections);

		// get english error message
		char* getErrMsg(Error err);

		// helper definitions
		using AttributeMap = std::unordered_map<std::string, std::string>;
		using ValueMap = std::unordered_map<std::string, Variant>;

	protected:
		Error parseSection(char* buf, size_t& reader, size_t buflen, int& line, Section &section);
		Error parseHeader(char* buf, size_t& reader, size_t buflen, int& line, std::string& path);
		Error parseContent(char* buf, size_t& reader, size_t buflen, int& line, values_map& values);
		Error parseContentEntry(char* buf, size_t& reader, size_t buflen, int& line, std::string& key, Variant& value);
	};
}
#endif // !SIMPLEPARSER_INI_H
