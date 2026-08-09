/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "CssParser.h"

namespace simpleparser
{
	enum class SelectorStates
	{
		kNone,
		kSequence,
		kClass,
		kId,
		kPseudo  // NEU: Pseudo-Klassen State
	};

	CssParser::Error CssParser::parseSelector(char* buf, size_t& reader, size_t buflen, int& line, Selector& selector)
	{
		Token token;
		SimpleParser::Error err;

		SelectorStates state = SelectorStates::kNone;
		SimpleSelectorType cur_simple_type = SimpleSelectorType::kType;
		Combinator cur_combinator = Combinator::kDescendant;
		SimpleSelectorSequence cur_simple_seq;
		selector.clear();

		bool selector_started = false;
		bool ws_relevant = false;

		id_chars = "-_";
		while (reader < buflen) {
			err = nextToken(buf, reader, buflen, line, token, ws_relevant);
			if (err >= SimpleParser::Error::PARSE_ERR)
				return Error::PARSE_ERR;
			else {
				switch (state) {
				case SelectorStates::kNone:
					if (err == SimpleParser::Error::PARSE_EOF)
						return selector_started
						? Error::PARSE_EXPECT_CONTENT_START
						: Error::PARSE_EOF;
					switch (token.type) {
					case TokenType::TOKEN_IDENTIFIER:
					case TokenType::TOKEN_STRING:
						// must be a type selector
						// start the simple sequence
						selector_started = true;
						cur_simple_seq.push_back({ SimpleSelectorType::kType, token.value.toString() });
						state = SelectorStates::kSequence;
						ws_relevant = true;
						break;
					case TokenType::TOKEN_DOT:
						// start of class selector
						state = SelectorStates::kClass;
						selector_started = true;
						ws_relevant = true;
						break;
					case TokenType::TOKEN_HASH:
						// start of id selector
						state = SelectorStates::kId;
						selector_started = true;
						ws_relevant = true;
						break;
					case TokenType::TOKEN_GREATER:
						if (cur_combinator != Combinator::kDescendant)
							return Error::PARSE_EXPECT_IDENTIFIER;
						cur_combinator = Combinator::kChild;
						break;
					case TokenType::TOKEN_PLUS:
						if (cur_combinator != Combinator::kDescendant)
							return Error::PARSE_EXPECT_IDENTIFIER;
						cur_combinator = Combinator::kSibling;
						break;
					case TokenType::TOKEN_TILDE:
						if (cur_combinator != Combinator::kDescendant)
							return Error::PARSE_EXPECT_IDENTIFIER;
						cur_combinator = Combinator::kSubsequent;
						break;
					case TokenType::TOKEN_LEFT_CURLYBRACKET:
						// start content
						if (!selector.size())
							return Error::PARSE_EXPECT_IDENTIFIER;
						return Error::PARSE_OK;
					default:
						return Error::PARSE_EXPECT_IDENTIFIER;
					}
					break;
				case SelectorStates::kSequence:
					// expect a new element of the sequence or whitespace or start of content
					if (err == SimpleParser::Error::PARSE_EOF)
						return Error::PARSE_EXPECT_CONTENT_START;
					switch (token.type) {
					case TokenType::TOKEN_DOT:
						// start of class selector
						state = SelectorStates::kClass;
						break;
					case TokenType::TOKEN_HASH:
						// start of id selector
						state = SelectorStates::kId;
						break;
					case TokenType::TOKEN_COLON:  // NEU: Pseudo-Klasse
						state = SelectorStates::kPseudo;
						break;
					case TokenType::TOKEN_WHITESPACE:
						// current sequence ended
						state = SelectorStates::kNone;
						ws_relevant = false;
						if (cur_simple_seq.size())
							selector.push_back({ cur_combinator , cur_simple_seq });
						cur_simple_seq.clear();
						cur_combinator = Combinator::kDescendant;
						break;
					case TokenType::TOKEN_LEFT_CURLYBRACKET:
						// start content
						if (cur_simple_seq.size())
							selector.push_back({ cur_combinator , cur_simple_seq });
						if (!selector.size())
							return Error::PARSE_EXPECT_IDENTIFIER;
						return Error::PARSE_OK;
						break;
					case TokenType::TOKEN_GREATER:
						// current sequence ended
						state = SelectorStates::kNone;
						ws_relevant = false;
						if (cur_simple_seq.size())
							selector.push_back({ cur_combinator , cur_simple_seq });
						cur_simple_seq.clear();
						cur_combinator = Combinator::kChild;
						break;
					case TokenType::TOKEN_PLUS:
						// current sequence ended
						state = SelectorStates::kNone;
						ws_relevant = false;
						if (cur_simple_seq.size())
							selector.push_back({ cur_combinator , cur_simple_seq });
						cur_simple_seq.clear();
						cur_combinator = Combinator::kSibling;
						break;
					case TokenType::TOKEN_TILDE:
						// current sequence ended
						state = SelectorStates::kNone;
						ws_relevant = false;
						if (cur_simple_seq.size())
							selector.push_back({ cur_combinator , cur_simple_seq });
						cur_simple_seq.clear();
						cur_combinator = Combinator::kSubsequent;
						break;
					default:
						return Error::PARSE_EXPECT_IDENTIFIER;
					}
					break;
				case SelectorStates::kClass:
					if (err == SimpleParser::Error::PARSE_EOF ||
						!(token.type == TokenType::TOKEN_IDENTIFIER ||
							token.type == TokenType::TOKEN_STRING))
						return Error::PARSE_EXPECT_IDENTIFIER;
					cur_simple_seq.push_back({ SimpleSelectorType::kClass, token.value.toString() });
					state = SelectorStates::kSequence;
					break;
				case SelectorStates::kId:
					if (err == SimpleParser::Error::PARSE_EOF ||
						!(token.type == TokenType::TOKEN_IDENTIFIER ||
							token.type == TokenType::TOKEN_STRING))
						return Error::PARSE_EXPECT_IDENTIFIER;
					cur_simple_seq.push_back({ SimpleSelectorType::kId, token.value.toString() });
					state = SelectorStates::kSequence;
					break;
				case SelectorStates::kPseudo:
					if (err == SimpleParser::Error::PARSE_EOF ||
						!(token.type == TokenType::TOKEN_IDENTIFIER ||
							token.type == TokenType::TOKEN_STRING))
						return Error::PARSE_EXPECT_IDENTIFIER;
					cur_simple_seq.push_back({ SimpleSelectorType::kPseudo, token.value.toString() });
					state = SelectorStates::kSequence;
					break;
				}
			}
		}
		return selector_started
			? Error::PARSE_EXPECT_CONTENT_START
			: Error::PARSE_EOF;
	}

	CssParser::Error CssParser::parseContent(char* buf, size_t& reader, size_t buflen, int& line, PropertyMap& property_map)
	{
		Token token;
		SimpleParser::Error err;
		std::string prop_name;
		std::vector<Property> properties;
		Variant value;

		int state = 0;
		id_chars = "-_";
		while (reader < buflen) {
			err = nextToken(buf, reader, buflen, line, token);
			if (err >= SimpleParser::Error::PARSE_ERR)
				return Error::PARSE_ERR;
			else {
				switch (state) {
				case 0: // outer context
					if (err == SimpleParser::Error::PARSE_EOF)
						return Error::PARSE_EXPECT_CONTENT_END;
					if (token.type == TokenType::TOKEN_RIGHT_CURLYBRACKET) {
						return Error::PARSE_OK;
					}
					else if (token.type != TokenType::TOKEN_IDENTIFIER &&
						token.type != TokenType::TOKEN_STRING)
						return Error::PARSE_EXPECT_IDENTIFIER;
					else {
						prop_name = token.value.toString();
						properties.clear();
						state = 1;
					}
					break;
				case 1: // parse property identifier
					if (err == SimpleParser::Error::PARSE_EOF ||
						(token.type != TokenType::TOKEN_COLON))
						return Error::PARSE_EXPECT_IDENTIFIER;
					else {
						// switch to value parsing
						Error css_err = parseProperty(buf, reader, buflen, line, properties);
						if (css_err != Error::PARSE_OK)
							return css_err;
						if (properties.size())
							property_map[prop_name] = properties;
						state = 0;
						id_chars = "-_";
					}
					break;
				}
			}
		}
		return Error::PARSE_EOF;
	}

	CssParser::Error CssParser::parseProperty(char* buf, size_t& reader, size_t buflen, int& line, std::vector<Property>& properties)
	{
		Token token;
		SimpleParser::Error err;
		int anchestor = -1;

		id_chars = ":/-'.";

		while (reader < buflen) {
			err = nextToken(buf, reader, buflen, line, token);
			if (err >= SimpleParser::Error::PARSE_ERR)
				return Error::PARSE_ERR;
			if (err == SimpleParser::Error::PARSE_EOF)
				if (anchestor < 0)
					return Error::PARSE_EOF;
				else
					return Error::PARSE_UNTERMINATED_ARRAY;
			switch (token.type) {
			case TokenType::TOKEN_COMMA:
				break;
			case TokenType::TOKEN_SEMICOLON:
				// end of property
				if (anchestor < 0)
					return Error::PARSE_OK;
				else
					return Error::PARSE_UNTERMINATED_ARRAY;
				break;
			case TokenType::TOKEN_RIGHT_CURLYBRACKET:
				// end of rule entry
				// push it back on stack
				reader--;
				if (anchestor < 0)
					return Error::PARSE_OK;
				else
					return Error::PARSE_UNTERMINATED_ARRAY;
				break;
			case TokenType::TOKEN_NUMBER:
				// lookahead if its a dimension
				if (reader < buflen && ((buf[reader] >= 'a' && buf[reader] <= 'z') ||
					(buf[reader] >= 'A' && buf[reader] <= 'Z') || buf[reader] == '%')) {
					// has a dimension
					Property prop;
					prop.id = PropertyId::kDimension;
					prop.value = token.value;
					if (buf[reader] == '%') {
						prop.extra = "%";
						reader++;
					}
					else {
						err = nextToken(buf, reader, buflen, line, token);
						if (err == SimpleParser::Error::PARSE_EOF || token.type != TokenType::TOKEN_IDENTIFIER)
							return Error::PARSE_EXPECT_IDENTIFIER;
						prop.extra = token.value;
					}
					properties.push_back(prop);
				}
				else {
					properties.push_back({ PropertyId::kNumeric, anchestor, token.value });
				}
				break;
			case TokenType::TOKEN_IDENTIFIER:
				properties.push_back({ PropertyId::kIdentifier, anchestor, token.value });
				break;
			case TokenType::TOKEN_LEFT_PARENTHESIS:
				// if last token was an identifier change it to function, else error
				anchestor = (int)properties.size() - 1;
				if (anchestor >= 0 && properties[anchestor].id == PropertyId::kIdentifier)
					properties[anchestor].id = PropertyId::kFunction;
				else
					return Error::PARSE_EXPECT_IDENTIFIER;
				break;
			case TokenType::TOKEN_RIGHT_PARENTHESIS:
				if (anchestor >= 0)
					anchestor = -1;
				else
					return Error::PARSE_UNTERMINATED_ARRAY;
				break;
			case TokenType::TOKEN_STRING:
				properties.push_back({ PropertyId::kString, anchestor, token.value });
				break;
			}
		}
		if (anchestor < 0)
			return Error::PARSE_EOF;
		else
			return Error::PARSE_UNTERMINATED_ARRAY;
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

		if (err == Error::PARSE_EOF)
			return Error::PARSE_EXPECT_CONTENT_END;

		if (err != Error::PARSE_OK)
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
		m_comment_type = 1;
		while (reader < buflen)
		{
			err = parseEntry(buf, reader, buflen, line, entry);
			if (err == Error::PARSE_OK) {
				css.push_back(std::move(entry));
				entries++;
				continue;
			}
			else if (err == Error::PARSE_EOF) {
				break;
			}
			else {
				printf("Css Parser: %s in line %d\n", getErrMsg(err), line);
				recoverToNextRule(buf, reader, buflen, line);
			}
		}
		return entries;
	}

	bool CssParser::ParsePropertyValue(const std::string& value, std::vector<Property>& properties)
	{
		Error err;
		int line = 0;
		size_t reader = 0;

		err = parseProperty((char*)value.c_str(), reader, value.size(), line, properties);
		if (err == Error::PARSE_OK || (err == Error::PARSE_EOF && reader >= value.size()))
			return true;
		else
			return false;
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
			return "Expected content end '}'";

		default:
			return "Syntax error";
		}
	}

	void CssParser::recoverToNextRule(char* buf, size_t& reader, size_t buflen, int& line)
	{
		Token token;

		while (reader < buflen) {
			const auto err = nextToken(buf, reader, buflen, line, token);

			if (err == SimpleParser::Error::PARSE_EOF)
				return;

			if (token.type == TokenType::TOKEN_RIGHT_CURLYBRACKET)
				return;
		}
	}
}
