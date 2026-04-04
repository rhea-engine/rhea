#ifndef REGISTRYFILEPARSER_H
#define REGISTRYFILEPARSER_H

#include <cstdio>
#include "SimpleParser/SimpleParser.h"

class Registry;

class RegistryFileParser
{
public:
	enum class Error
	{
		PARSE_OK,
		PARSE_EOF,
		PARSE_ERROR,
		PARSE_EXPECT_IDENTIFIER,
		PARSE_EXPECT_ASSIGN,
		PARSE_EXPECT_HEADER_START,
		PARSE_EXPECT_HEADER_END,
		PARSE_EXPECT_HEADER_IDENTIFIER,

	};

	RegistryFileParser(Registry* registry) :
		m_registry(registry)
	{}
	// parse a buffer
	int parse(char* buf, size_t buflen);

	// get english error message
	char* getErrMsg(Error err);
protected:
	Error parseSection(char* buf, size_t& reader, size_t buflen, int &line, std::string& path, Entry** entry);
	Error parseHeader(char* buf, size_t& reader, size_t buflen, int &line, std::string& path);
	Error parseContent(char* buf, size_t& reader, size_t buflen, int &line, std::unordered_map<std::string, std::string>& values);
	Error parseContentEntry(char* buf, size_t& reader, size_t buflen, int &line, std::string& key, std::string &value);
	Registry* m_registry;
	std::unique_ptr<simpleparser::SimpleParser> parser;
};

#endif // !REGISTRYFILEPARSER_H
