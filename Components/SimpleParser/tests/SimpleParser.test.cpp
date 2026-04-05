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
		
		EXPECT_EQ(err, SimpleParser::Error::PARSE_ERR);
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
		auto result = parser.parse(const_cast<char*>(ini.c_str()), ini.length());
		
		EXPECT_EQ(result.error, IniFileParser::Error::OK);
		EXPECT_EQ(result.sections.size(), 2);
		
		EXPECT_TRUE(result.sections.find("Section1") != result.sections.end());
		EXPECT_TRUE(result.sections.find("Section2") != result.sections.end());
		
		auto& section1 = result.sections["Section1"];
		EXPECT_EQ(static_cast<std::string>(section1["key1"]), "value1");
		EXPECT_EQ(static_cast<int>(section1["key2"]), 42);
		
		auto& section2 = result.sections["Section2"];
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
		auto result = parser.parse(const_cast<char*>(ini.c_str()), ini.length());
		
		EXPECT_EQ(result.error, IniFileParser::Error::OK);
		EXPECT_EQ(result.sections.size(), 1);
		EXPECT_EQ(static_cast<std::string>(result.sections["Section"]["key"]), "value");
	}

	// ========================================
	// CssParser Tests (Basic)
	// ========================================

	TEST(CssParserTest, ParseSimpleCss)
	{
		std::string css = 
			".class1 {\n"
			"  property1: value1;\n"
			"  property2: 42;\n"
			"}\n";
		
		CssParser parser;
		auto result = parser.parse(const_cast<char*>(css.c_str()), css.length());
		
		EXPECT_EQ(result.error, CssParser::Error::OK);
		EXPECT_TRUE(result.rules.find(".class1") != result.rules.end());
		
		auto& rule = result.rules[".class1"];
		EXPECT_EQ(static_cast<std::string>(rule["property1"]), "value1");
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