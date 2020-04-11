#ifndef LOG_H
#define LOG_H

class Log
{
public:
    enum {
        LOG_HOST,
        LOG_USER,
        LOG_PID
    };

    Log();

    // log a string
    void info(const char* format, ...);
    // set the token key
    void set(int key, char* value);
protected:
    char* m_token[3];
};

#endif // !LOG_H