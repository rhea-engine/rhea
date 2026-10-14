/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <time.h>       /* time_t, struct tm, time, localtime, strftime */
#include <stdio.h>
#include <stdarg.h>
#include <string>

#include <Psio/Log.h>

std::string LogSeverityString[] = {
    "NONE",
    "DEBUG",
    "INFO",
    "NOTICE",
    "WARNING",
    "ERROR",
    "FATAL"
};

Log::Log()
{
    m_token[LOG_HOST] = "-";
    m_token[LOG_USER] = "-";
    m_token[LOG_PID] = "-";

    m_level = 0;
}

Log::~Log()
{
    if (m_log_stream && m_log_stream.is_open()) {
        m_log_stream.close();     
    }
}

void Log::print(int level, char* msg)
{
    if (level < m_level) {
        return;
    }

    ::printf("%s\n", msg);
}

void Log::print(std::string s)
{
#if LOG_USE_STREAM
	if (m_log_stream && m_log_stream.is_open()) {
		m_log_stream << s;
		m_log_stream.flush();
	}
#else
    if (!m_logfile.empty()) {
        m_log_stream = std::ofstream(m_logfile, std::ios_base::app);
		if (!m_log_stream.is_open()) {
            ::printf("%s", s.c_str()); 
            return;
		}
		m_log_stream << s;
        m_log_stream.close();
    }
#endif
    else {
        ::printf("%s", s.c_str());
    }
}

void Log::printf(int level, const char* format, ...)
{
    time_t rawtime;
    struct tm* timeinfo;
    char t_buffer[80];
    va_list args;
    
    if (level < m_level) {
        return;
    }
    
    va_start(args, format);

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(t_buffer, 80, "%d/%b/%y:%H:%M:%S %z", timeinfo);

    ::printf("%s - %s [%s] ", m_token[LOG_HOST], m_token[LOG_USER], t_buffer);
    vprintf(format, args);
    if (level < m_level) {
        return;
    }
    ::printf("\n");
}

void Log::set(int key, char* value)
{
    m_token[key] = value;
}

void Log::setLogFile(std::string logfile)
{
	m_logfile = logfile;
    // open the logfile
    m_log_stream = std::ofstream(logfile, std::ios_base::app);
    if (!m_log_stream.is_open()) {
        ::printf("Could not open logfile %s\n", logfile.c_str());
		m_logfile.clear();
    }
#if !LOG_USE_STREAM
	m_log_stream.close();
#endif
}

Log& Log::logger()
{
    static Log m_logger;
    return m_logger;
}

LogMessage::LogMessage(int severity) :
    m_severity(severity)
{
    time_t rawtime;
    struct tm* timeinfo;
    char t_buffer[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    m_stream << "[" << LogSeverityString[severity] << "] ";

    strftime(t_buffer, 80, "%d/%b/%y:%H:%M:%S %z", timeinfo);
    m_stream << "[" << t_buffer << "] ";
}

LogMessage::~LogMessage() 
{
    m_stream << std::endl;
    Log::logger().print(m_stream.str());
}