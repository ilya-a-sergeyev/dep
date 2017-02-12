#ifndef _LOGGING_
#define _LOGGING_

#include "log4cpp/Category.hh"
#include "log4cpp/Appender.hh"
#include "log4cpp/OstreamAppender.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/Priority.hh"
#include "log4cpp/NDC.hh"
#include "log4cpp/PropertyConfigurator.hh"

#define CODE_LOCATION __FILE__

// DEBUG < INFO < NOTICE < WARN < ERROR < CRIT < ALERT < FATAL = EMERG

#define LOG_EMERG(__logstream)  __logstream << log4cpp::Priority::EMERG << CODE_LOCATION
#define LOG_ALERT(__logstream)  __logstream << log4cpp::Priority::ALERT << CODE_LOCATION
#define LOG_CRIT(__logstream)  __logstream << log4cpp::Priority::CRIT << CODE_LOCATION
#define LOG_ERROR(__logstream)  __logstream << log4cpp::Priority::ERROR << CODE_LOCATION
#define LOG_WARN(__logstream)  __logstream << log4cpp::Priority::WARN << CODE_LOCATION
#define LOG_NOTICE(__logstream)  __logstream << log4cpp::Priority::NOTICE << CODE_LOCATION
#define LOG_INFO(__logstream)  __logstream << log4cpp::Priority::INFO << CODE_LOCATION
#define LOG_DEBUG(__logstream)  __logstream << log4cpp::Priority::DEBUG << CODE_LOCATION

class Log
{
public:
    static log4cpp::CategoryStream Err;
    static log4cpp::CategoryStream Warn;
    static log4cpp::CategoryStream Inf;
    static log4cpp::CategoryStream Not;
    static log4cpp::CategoryStream Dbg;

    static Log *getInstance();
    Log(Log const&)               = delete;
    void operator=(Log const&)    = delete;

private:
    static Log *instance;
    Log();
};

#endif
