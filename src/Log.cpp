#include <iostream>
#include "Log.h"

Log *Log::instance = 0;

Log *Log::getInstance()
{
    if (!instance)
        instance = new Log();

    return instance;
}

log4cpp::CategoryStream Log::Err(
        log4cpp::Category::getInstance(std::string("TVARI")).errorStream());
log4cpp::CategoryStream Log::Warn(
        log4cpp::Category::getInstance(std::string("TVARI")).warnStream());
log4cpp::CategoryStream Log::Not(
        log4cpp::Category::getInstance(std::string("TVARI")).noticeStream());
log4cpp::CategoryStream Log::Inf(
        log4cpp::Category::getInstance(std::string("TVARI")).infoStream());
log4cpp::CategoryStream Log::Dbg(
        log4cpp::Category::getInstance(std::string("TVARI")).debugStream());

Log::Log()
{
    const char *file_log4cpp_init = "log4cpp.ini";

    try
    {
        log4cpp::PropertyConfigurator::configure(file_log4cpp_init);
    }
    catch(log4cpp::ConfigureFailure &e)
    {
        std::cout << e.what()
            << " [log4cpp::ConfigureFailure catched] while reading "
            << file_log4cpp_init
            << std::endl;
        return;
    }

    log4cpp::Category &rootc = log4cpp::Category::getRoot();
    log4cpp::Category &mainc = log4cpp::Category::getInstance(std::string("TVARI"));

    rootc << log4cpp::Priority::NOTICE << "Log system init\n";
    if (mainc.isDebugEnabled()) {
        rootc << log4cpp::Priority::NOTICE << "Debug   priority = " << Log::Dbg.getPriority() << log4cpp::eol;
    }
    if (mainc.isInfoEnabled()) {
        rootc << log4cpp::Priority::NOTICE << "Info    priority = " << Log::Inf.getPriority() << log4cpp::eol;
    }
    if (mainc.isNoticeEnabled()) {
        rootc << log4cpp::Priority::NOTICE << "Notice  priority = " << Log::Not.getPriority() << log4cpp::eol;
    }
    if (mainc.isWarnEnabled()) {
        rootc << log4cpp::Priority::NOTICE << "Warning priority = " << Log::Warn.getPriority() << log4cpp::eol;
    }
    if (mainc.isErrorEnabled()) {
        rootc << log4cpp::Priority::NOTICE << "Error   priority = " << Log::Err.getPriority() << log4cpp::eol;
    }
}

