/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Psio/Util.h>

bool stricmp(const std::string& a, const std::string& b)
{
    return std::equal(a.begin(), a.end(),
        b.begin(), b.end(),
        [](char a, char b) {
            return tolower(a) == tolower(b);
        });
}

int strncmp_i(char const* a, char const* b, int len)
{
    int i = 0;

    // find first differing character between the C-strings a and b  
    while (i < len && *a && std::toupper(*a) == std::toupper(*b))
        ++a, ++b, ++i;
    if (i==len)
        return 0;
    return *a - *b;
}

char tolowerASCII(char c) {
    return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
}

// lower string
std::string str_tolower(std::string data) 
{
    std::transform(data.begin(), data.end(), data.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return data;
}
