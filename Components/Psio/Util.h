#ifndef PSIO_UTIL_H
#define PSIO_UTIL_H

#include <string>
#include <algorithm>

bool stricmp(const std::string& a, const std::string& b);

int strncmp_i(char const* a, char const* b, int len);

// lower all ascii chars. standard library tolower is locale sensitive
char tolowerASCII(char c);

// lower string
std::string str_tolower(std::string data);

#endif // !PSIO_UTIL_H
