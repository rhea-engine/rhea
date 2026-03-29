#ifndef LOG_H
#define LOG_H

#include <sstream>
#include <fstream>

// LOG Macros

// Helper macro: only log if condition is true, so arguments to stream are not evalated otherwise
#define LOG_COND(condition, severity) (condition) ? (void) 0 : LogMessageNull() 

#define LOG(severity) LOG_COND(Log::logger().level()>severity, severity) & LogMessage(severity).stream()
//#define LOG(severity) LogMessage(severity).stream()
#ifdef _DEBUG
    #define DLOG(severity) LOG_COND(Log::logger().level()>severity, severity) & LogMessage(severity).stream()
#else
    #define DLOG(severity) LogMessageNull() & LogMessage(severity).stream()
#endif


// helper class for one LogMessage
// use the Log Macros
class LogMessage
{
public:
    LogMessage(int severity);
    virtual ~LogMessage();

    std::ostream& stream() { return m_stream; }
private:
    int m_severity;
    std::ostringstream m_stream;
};

// helper function for not used logging
class LogMessageNull
{
public:
    LogMessageNull()
    {}
    void operator&(std::ostream&)
    {}    
};

enum LogSeverity
{
    LOG_NONE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_NOTICE,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
};

class Log
{
public:
    enum Key {
        LOG_HOST,
        LOG_USER,
        LOG_PID
    };

     // log a string
    void print(int level, char* msg);
    void printf(int level, const char* p_format, ...);
    // print string always
    void print(std::string s);

    void setLevel(int level) {
        m_level = level;
    }

    bool isLogging(int severity) { return severity >= m_level && severity <= LOG_FATAL; }
    int level() { return m_level; }

    // set the token key to the value
    void set(int key, char* value);

    // set log file 
    void setLogFile(std::string logfile);

    // Singleton
    static Log& logger();

protected:
    Log();
    ~Log();

    char* m_token[3];
    int m_level;
    std::string m_logfile;
    std::ofstream m_log_stream;
};

#endif // !LOG_H