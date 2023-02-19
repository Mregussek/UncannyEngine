
#ifndef UNCANNYENGINE_LOG_H
#define UNCANNYENGINE_LOG_H


#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>

#include <memory>


namespace uncanny
{


class FLogger {
public:

  static void init(std::shared_ptr<spdlog::logger>& logger);

  static std::shared_ptr<spdlog::logger> sLoggerPtr;

};


}


#define UTRACE(msg, ...) SPDLOG_LOGGER_TRACE(::uncanny::FLogger::sLoggerPtr, msg, __VA_ARGS__)
#define UDEBUG(msg, ...) SPDLOG_LOGGER_DEBUG(::uncanny::FLogger::sLoggerPtr, msg, __VA_ARGS__)
#define UINFO(msg, ...) SPDLOG_LOGGER_INFO(::uncanny::FLogger::sLoggerPtr, msg, __VA_ARGS__)
#define UWARN(msg, ...) SPDLOG_LOGGER_WARN(::uncanny::FLogger::sLoggerPtr, msg, __VA_ARGS__)
#define UERROR(msg, ...) SPDLOG_LOGGER_ERROR(::uncanny::FLogger::sLoggerPtr, msg, __VA_ARGS__)
#define UFATAL(msg, ...) SPDLOG_LOGGER_CRITICAL(::uncanny::FLogger::sLoggerPtr, msg, __VA_ARGS__)


#endif //UNCANNYENGINE_LOG_H
