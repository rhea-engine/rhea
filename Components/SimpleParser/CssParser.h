#ifndef CSS_PARSER_H
#define CSS_PARSER_H

#include "SimpleParser.h"
#include <unordered_map>

namespace SimpleParser
{
	class CssParser : public SimpleParser
	{
	public:

		struct Selector
		{
			std::string name;
		};

		typedef std::unordered_map<std::string, std::vector<Variant>> PropertyMap;

		struct Entry
		{
			Selector selector;
			PropertyMap properties;
		};

		// parse a buffer into list of tokens
		int parse(char* buf, size_t buflen, std::vector<CssParser::Entry>& css);

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
			PARSE_EXPECT_CONTENT_START,
			PARSE_EXPECT_CONTENT_END
		};

		// get an error message
		char* getErrMsg(Error);

	protected:
		Error parseEntry(char* buf, size_t& reader, size_t buflen, int& line, Entry& entry);
		Error parseSelector(char* buf, size_t& reader, size_t buflen, int& line, Selector& name);
		Error parseContent(char* buf, size_t& reader, size_t buflen, int& line, PropertyMap& property_map);

		enum CSSPARSE_STATE {
			PARSE_NONE,
			PARSE_NAME,
			PARSE_WS
		};
	};
}

#endif // !CSS_PARSER_H
