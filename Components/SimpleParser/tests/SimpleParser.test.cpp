/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <gtest/gtest.h>
#include "SimpleParser.h"
#include "Variant.h"
#include "Type.h"
#include "IniFileParser.h"
#include "CssParser.h"
#include <string>
#include <sstream>

namespace simpleparser
{
	// ========================================
	// Variant Tests
	// ========================================

	TEST(VariantTest, ConstructorBool)
	{
		Variant v(true);
		EXPECT_EQ(static_cast<bool>(v), true);
		EXPECT_FALSE(v.isNull());
	}

	TEST(VariantTest, ConstructorInt)
	{
		Variant v(42);
		EXPECT_EQ(static_cast<int>(v), 42);
		EXPECT_FALSE(v.isNull());
	}

	TEST(VariantTest, ConstructorFloat)
	{
		Variant v(3.14f);
		EXPECT_FLOAT_EQ(static_cast<float>(v), 3.14f);
	}

	TEST(VariantTest, ConstructorString)
	{
		std::string str = "Hello World";
		Variant v(str);
		EXPECT_EQ(static_cast<std::string>(v), "Hello World");
	}

	TEST(VariantTest, ConstructorStringConst)
	{
		const std::string str = "Const String";
		Variant v(str);
		EXPECT_EQ(static_cast<std::string>(v), "Const String");
	}

	TEST(VariantTest, ConstructorCString)
	{
		Variant v("C String");
		EXPECT_EQ(static_cast<std::string>(v), "C String");
	}

	TEST(VariantTest, CopyConstructor)
	{
		Variant v1(42);
		Variant v2(v1);
		EXPECT_EQ(static_cast<int>(v2), 42);
	}

	TEST(VariantTest, AssignmentOperator)
	{
		Variant v1(100);
		Variant v2;
		v2 = v1;
		EXPECT_EQ(static_cast<int>(v2), 100);
	}

	TEST(VariantTest, EqualityOperator)
	{
		Variant v1(42);
		Variant v2(42);
		Variant v3(24);
		
		EXPECT_TRUE(v1 == v2);
		EXPECT_FALSE(v1 == v3);
	}

	TEST(VariantTest, InequalityOperator)
	{
		Variant v1(42);
		Variant v2(24);
		
		EXPECT_TRUE(v1 != v2);
	}

	TEST(VariantTest, ArrayOperations)
	{
		std::vector<Variant> arr;
		arr.push_back(Variant(1));
		arr.push_back(Variant(2));
		arr.push_back(Variant(3));
		
		Variant v(arr);
		EXPECT_TRUE(v.isArray());
		EXPECT_EQ(v.length(), 3);
		EXPECT_EQ(static_cast<int>(v[0]), 1);
		EXPECT_EQ(static_cast<int>(v[1]), 2);
		EXPECT_EQ(static_cast<int>(v[2]), 3);
	}

	// ========================================
	// Type Tests
	// ========================================

	TEST(TypeTest, GetBaseTypes)
	{
		Type* intType = Type::get(Type::BaseType::INT);
		EXPECT_NE(intType, nullptr);
		
		Type* stringType = Type::get(Type::BaseType::STRING);
		EXPECT_NE(stringType, nullptr);
		
		Type* floatType = Type::get(Type::BaseType::FLOAT);
		EXPECT_NE(floatType, nullptr);
	}

	TEST(TypeTest, RegisterType)
	{
		auto factory = [](std::string name, std::string type) -> Variant* {
			return new Variant();
		};
		
		Type* customType = Type::Register("TestType", factory);
		EXPECT_NE(customType, nullptr);
		EXPECT_EQ(customType->getName(), "TestType");
		
		std::string typeName = "TestType";
		Type* retrievedType = Type::get(typeName);
		EXPECT_EQ(retrievedType, customType);
	}

	// ========================================
	// SimpleParser Tests
	// ========================================

	TEST(SimpleParserTest, TokenizeIdentifier)
	{
		std::string input = "identifier";
		SimpleParser parser;
		
		size_t reader = 0;
		int line = 1;
		simpleparser::Token token;
		
		auto err = parser.nextToken(const_cast<char*>(input.c_str()), reader, input.length(), line, token, false);
		
		EXPECT_EQ(err, SimpleParser::Error::PARSE_OK);
		EXPECT_EQ(token.type, simpleparser::TokenType::TOKEN_IDENTIFIER);
		EXPECT_EQ(static_cast<std::string>(token.value), "identifier");
	}

	TEST(SimpleParserTest, TokenizeNumber)
	{
		std::string input = "42";
		SimpleParser parser;
		
		size_t reader = 0;
		int line = 1;
		simpleparser::Token token;
		
		auto err = parser.nextToken(const_cast<char*>(input.c_str()), reader, input.length(), line, token, false);
		
		EXPECT_EQ(err, SimpleParser::Error::PARSE_OK);
		EXPECT_EQ(token.type, simpleparser::TokenType::TOKEN_NUMBER);
		EXPECT_EQ(static_cast<int>(token.value), 42);
	}

	TEST(SimpleParserTest, TokenizeFloat)
	{
		std::string input = "3.14";
		SimpleParser parser;
		
		size_t reader = 0;
		int line = 1;
		simpleparser::Token token;
		
		auto err = parser.nextToken(const_cast<char*>(input.c_str()), reader, input.length(), line, token, false);
		
		EXPECT_EQ(err, SimpleParser::Error::PARSE_OK);
		EXPECT_EQ(token.type, simpleparser::TokenType::TOKEN_NUMBER);
		EXPECT_FLOAT_EQ(static_cast<float>(token.value), 3.14f);
	}

	TEST(SimpleParserTest, TokenizeString)
	{
		std::string input = "\"Hello World\"";
		SimpleParser parser;
		
		size_t reader = 0;
		int line = 1;
		simpleparser::Token token;
		
		auto err = parser.nextToken(const_cast<char*>(input.c_str()), reader, input.length(), line, token, false);
		
		EXPECT_EQ(err, SimpleParser::Error::PARSE_OK);
		EXPECT_EQ(token.type, simpleparser::TokenType::TOKEN_STRING);
		EXPECT_EQ(static_cast<std::string>(token.value), "Hello World");
	}

	TEST(SimpleParserTest, QuotedStringDelimitersAndEscapes)
	{
		struct TestCase {
			std::string input;
			std::string expected;
		};
		TestCase cases[] = {
			{"'Hello World'", "Hello World"},
			{"''", ""},
			{"\"\"", ""},
			{"'say \"hello\"'", "say \"hello\""},
			{"\"it's valid\"", "it's valid"},
			{R"('it\'s valid')", "it's valid"},
			{R"("say \"hello\"")", "say \"hello\""},
			{R"('path\\file\n\r\t')", "path\\file\n\r\t"},
			{R"("path\\file\n\r\t")", "path\\file\n\r\t"},
		};
		for (auto& test_case : cases) {
			SCOPED_TRACE(test_case.input);
			SimpleParser parser;
			size_t reader = 0;
			int line = 1;
			Token token;
			ASSERT_EQ(parser.nextToken(test_case.input.data(), reader, test_case.input.size(), line, token, false),
				SimpleParser::Error::PARSE_OK);
			EXPECT_EQ(token.type, TokenType::TOKEN_STRING);
			EXPECT_EQ(static_cast<std::string>(token.value), test_case.expected);
			EXPECT_EQ(reader, test_case.input.size());
		}
	}

	TEST(SimpleParserTest, UnterminatedQuotedStrings)
	{
		std::string cases[] = {"'", "\"", "'unterminated", "\"unterminated", "'mismatched\"", "\"mismatched'", "'trailing\\", "\"trailing\\"};
		for (auto& input : cases) {
			SCOPED_TRACE(input);
			SimpleParser parser;
			size_t reader = 0;
			int line = 1;
			Token token;
			EXPECT_EQ(parser.nextToken(input.data(), reader, input.size(), line, token, false),
				SimpleParser::Error::PARSE_UNTERMINATED_STRING);
		}
	}

	TEST(SimpleParserTest, AdjacentQuotedStrings)
	{
		std::string input = "'first'\"second\"";
		SimpleParser parser;
		size_t reader = 0;
		int line = 1;
		Token token;
		ASSERT_EQ(parser.nextToken(input.data(), reader, input.size(), line, token, false), SimpleParser::Error::PARSE_OK);
		EXPECT_EQ(static_cast<std::string>(token.value), "first");
		EXPECT_EQ(reader, 7u);
		ASSERT_EQ(parser.nextToken(input.data(), reader, input.size(), line, token, false), SimpleParser::Error::PARSE_OK);
		EXPECT_EQ(token.type, TokenType::TOKEN_STRING);
		EXPECT_EQ(static_cast<std::string>(token.value), "second");
		EXPECT_EQ(reader, input.size());
	}

	TEST(SimpleParserTest, TokenizeOperators)
	{
		struct TestCase {
			std::string input;
			simpleparser::TokenType expectedType;
		};
		
		TestCase cases[] = {
			{"=", simpleparser::TokenType::TOKEN_EQUEAL},
			{"{", simpleparser::TokenType::TOKEN_LEFT_CURLYBRACKET},
			{"}", simpleparser::TokenType::TOKEN_RIGHT_CURLYBRACKET},
			{"[", simpleparser::TokenType::TOKEN_LEFT_BRACKET},
			{"]", simpleparser::TokenType::TOKEN_RIGHT_BRACKET},
			{"(", simpleparser::TokenType::TOKEN_LEFT_PARENTHESIS},
			{")", simpleparser::TokenType::TOKEN_RIGHT_PARENTHESIS},
			{",", simpleparser::TokenType::TOKEN_COMMA},
			{":", simpleparser::TokenType::TOKEN_COLON},
			{";", simpleparser::TokenType::TOKEN_SEMICOLON},
			{".", simpleparser::TokenType::TOKEN_DOT},
		};
		
		SimpleParser parser;
		for (const auto& testCase : cases) {
			size_t reader = 0;
			int line = 1;
			simpleparser::Token token;
			
			auto err = parser.nextToken(const_cast<char*>(testCase.input.c_str()), 
			                           reader, testCase.input.length(), line, token, false);
			
			EXPECT_EQ(err, SimpleParser::Error::PARSE_OK);
			EXPECT_EQ(token.type, testCase.expectedType) 
				<< "Failed for input: " << testCase.input;
		}
	}

	TEST(SimpleParserTest, SkipWhitespace)
	{
		std::string input = "   \t\n  identifier";
		SimpleParser parser;
		
		size_t reader = 0;
		int line = 1;
		simpleparser::Token token;
		
		auto err = parser.nextToken(const_cast<char*>(input.c_str()), reader, input.length(), line, token, false);
		
		EXPECT_EQ(err, SimpleParser::Error::PARSE_OK);
		EXPECT_EQ(token.type, simpleparser::TokenType::TOKEN_IDENTIFIER);
	}

	TEST(SimpleParserTest, SkipComments)
	{
		std::string input = "# This is a comment\nidentifier";
		SimpleParser parser;
		
		size_t reader = 0;
		int line = 1;
		simpleparser::Token token;
		
		auto err = parser.nextToken(const_cast<char*>(input.c_str()), reader, input.length(), line, token, false);
		
		EXPECT_EQ(err, SimpleParser::Error::PARSE_OK);
		EXPECT_EQ(token.type, simpleparser::TokenType::TOKEN_IDENTIFIER);
		EXPECT_EQ(static_cast<std::string>(token.value), "identifier");
	}

	TEST(SimpleParserTest, NegativeNumber)
	{
		std::string input = "-42";
		SimpleParser parser;
		
		size_t reader = 0;
		int line = 1;
		simpleparser::Token token;
		
		auto err = parser.nextToken(const_cast<char*>(input.c_str()), reader, input.length(), line, token, false);
		
		EXPECT_EQ(err, SimpleParser::Error::PARSE_OK);
		EXPECT_EQ(token.type, simpleparser::TokenType::TOKEN_NUMBER);
		EXPECT_EQ(static_cast<int>(token.value), -42);
	}

	TEST(SimpleParserTest, UnterminatedString)
	{
		std::string input = "\"unterminated";
		SimpleParser parser;
		
		size_t reader = 0;
		int line = 1;
		simpleparser::Token token;
		
		auto err = parser.nextToken(const_cast<char*>(input.c_str()), reader, input.length(), line, token, false);
		
		EXPECT_EQ(err, SimpleParser::Error::PARSE_UNTERMINATED_STRING);
	}

	// ========================================
	// IniFileParser Tests
	// ========================================

	TEST(IniFileParserTest, ParseSimpleIni)
	{
		std::string ini = 
			"[Section1]\n"
			"key1=value1\n"
			"key2=42\n"
			"\n"
			"[Section2]\n"
			"key3=3.14\n";
		
		IniFileParser parser;
		std::vector<IniFileParser::Section> sections;
		EXPECT_EQ(parser.parse(ini.data(), ini.size(), sections), IniFileParser::Error::PARSE_OK);
		
		ASSERT_EQ(sections.size(), 2u);
		EXPECT_EQ(sections[0].section_name, "Section1");
		EXPECT_EQ(sections[1].section_name, "Section2");
		auto& section1 = sections[0].values;
		EXPECT_EQ(static_cast<std::string>(section1["key1"]), "value1");
		EXPECT_EQ(static_cast<int>(section1["key2"]), 42);
		
		auto& section2 = sections[1].values;
		EXPECT_FLOAT_EQ(static_cast<float>(section2["key3"]), 3.14f);
	}

	TEST(IniFileParserTest, ParseWithComments)
	{
		std::string ini = 
			"# Comment line\n"
			"[Section]\n"
			"# Another comment\n"
			"key=value\n";
		
		IniFileParser parser;
		std::vector<IniFileParser::Section> sections;
		EXPECT_EQ(parser.parse(ini.data(), ini.size(), sections), IniFileParser::Error::PARSE_OK);
		
		ASSERT_EQ(sections.size(), 1u);
		EXPECT_EQ(sections[0].section_name, "Section");
		EXPECT_EQ(static_cast<std::string>(sections[0].values["key"]), "value");
	}

	// ========================================
	// CssParser Tests (Basic)
	// ========================================

	TEST(CssParserTest, ParseQuotedUrl)
	{
		for (const std::string& input : {std::string("url('//gui_sandbox/plus.png')"), std::string("url(\"//gui_sandbox/plus.png\")")}) {
			SCOPED_TRACE(input);
			CssParser parser;
			std::vector<CssParser::Property> properties;
			ASSERT_TRUE(parser.ParsePropertyValue(input, properties));
			ASSERT_EQ(properties.size(), 2u);
			EXPECT_EQ(properties[0].id, CssParser::PropertyId::kFunction);
			EXPECT_EQ(properties[0].value.toString(), "url");
			EXPECT_EQ(properties[1].id, CssParser::PropertyId::kString);
			EXPECT_EQ(properties[1].anchestor, 0);
			EXPECT_EQ(properties[1].value.toString(), "//gui_sandbox/plus.png");
		}
	}

	TEST(CssParserTest, ParseSimpleCss)
	{
		std::string css = 
			".class1 {\n"
			"  property1: value1;\n"
			"  property2: 42;\n"
			"}\n";
		
		CssParser parser;
		std::vector<CssParser::Entry> entries;
		EXPECT_EQ(parser.parse(css.data(), css.size(), entries), CssParser::Error::PARSE_OK);

		ASSERT_EQ(entries.size(), 1u);
		ASSERT_EQ(entries[0].selector.size(), 1u);
		ASSERT_EQ(entries[0].selector[0].sequence.size(), 1u);
		EXPECT_EQ(entries[0].selector[0].sequence[0].type, CssParser::SimpleSelectorType::kClass);
		EXPECT_EQ(entries[0].selector[0].sequence[0].name, "class1");
		auto& properties = entries[0].properties;
		ASSERT_EQ(properties.count("property1"), 1u);
		ASSERT_EQ(properties.at("property1").size(), 1u);
		EXPECT_EQ(properties.at("property1")[0].value.toString(), "value1");
	}

	// ========================================
	// Error Message Tests
	// ========================================

	TEST(SimpleParserTest, ErrorMessages)
	{
		SimpleParser parser;
		
		EXPECT_STREQ(parser.getErrMsg(SimpleParser::Error::PARSE_OK), "OK");
		EXPECT_STREQ(parser.getErrMsg(SimpleParser::Error::PARSE_EOF), "End of file");
		EXPECT_STREQ(parser.getErrMsg(SimpleParser::Error::PARSE_ERR), "Parse error");
		EXPECT_STREQ(parser.getErrMsg(SimpleParser::Error::PARSE_UNTERMINATED_STRING), 
		            "Unterminated string");
		EXPECT_STREQ(parser.getErrMsg(SimpleParser::Error::PARSE_INVALID_ID_CHAR), 
		            "Invalid character in identifier");
	}

} // namespace simpleparser
