#include <time.h>       /* time_t, struct tm, time, localtime, strftime */
#include <stdio.h>
#include <stdarg.h>

#include "Components/Log/Log.h"

Log::Log()
{
    m_token[LOG_HOST] = "-";
    m_token[LOG_USER] = "-";
    m_token[LOG_PID] = "-";
}

void Log::info(const char* format, ...)
{
    time_t rawtime;
    struct tm* timeinfo;
    char t_buffer[80];
    va_list args;
    va_start(args, format);

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(t_buffer, 80, "%d/%b/%y:%H:%M:%S %z", timeinfo);

    printf("%s - %s [%s] ", m_token[LOG_HOST], m_token[LOG_USER], t_buffer);
    vprintf(format, args);
    printf("\n");
}

void Log::set(int key, char* value)
{
    m_token[key] = value;
}
