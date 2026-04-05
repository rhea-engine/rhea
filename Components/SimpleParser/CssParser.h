#ifndef CSS_PARSER_H
#define CSS_PARSER_H

#include "SimpleParser.h"
#include <unordered_map>

namespace simpleparser
{
	class CssParser : public SimpleParser
	{
	public:

		enum class SimpleSelectorType
		{
			kType,
			kUniversal,
			kId,
			kClass,
			kAttribute,
			kPseudo
		};

		struct SimpleSelector
		{
			SimpleSelectorType type;
			std::string name;
		};

		typedef std::vector<SimpleSelector> SimpleSelectorSequence;

		enum class Combinator {
			kDescendant,
			kChild,
			kSibling,
			kSubsequent
		};

		struct SelectorItem
		{
			Combinator combinator;
			SimpleSelectorSequence sequence;
		};

		typedef std::vector<SelectorItem> Selector;

		enum class PropertyId {
			kString,
			kIdentifier,
			kFunction,
			kNumeric,
			kDimension,
		};

		struct Property {
			PropertyId id = PropertyId::kString;
			int anchestor = -1;
			Variant value;
			Variant extra;
		};

		typedef std::unordered_map<std::string, std::vector<Property>> PropertyMap;

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
			PARSE_UNTERMINATED_ARRAY,
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
