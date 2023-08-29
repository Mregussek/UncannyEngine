
#ifndef UNCANNYENGINE_LOGIMPL_H
#define UNCANNYENGINE_LOGIMPL_H


#define UENABLE_DEBUG_TRACES 1

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#define SPDLOG_USE_STD_FORMAT
#include <spdlog/spdlog.h>

#include <memory>
#include <source_location>
#undef ERROR


namespace uncanny
{


enum class ELogLevel
{
  TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL
};


class FLog
{
public:

  static void create();

  template<typename... Args>
  static void log(const char* msg, ELogLevel level, const char* filename, int line, const char* functionName,
                  Args&&... args)
  {
    spdlog::source_loc sourceLocation{ filename, (int)line, functionName };
    spdlog::level::level_enum spdlogLevel = ConvertLogLevel(level);
    std::string msgStr = std::vformat(msg, std::make_format_args(std::forward<Args>(args)...));
    s_pLogger->log(sourceLocation, spdlogLevel, msgStr);
  }

private:

  static spdlog::level::level_enum ConvertLogLevel(ELogLevel level);

private:

  static std::shared_ptr<spdlog::logger> s_pLogger;

};


}


#define __ULOG_MSG(msg, level, ...) \
{ \
  constexpr std::source_location loc = std::source_location::current(); \
  ::uncanny::FLog::log(msg, level, loc.file_name(), loc.line(), loc.function_name(), __VA_ARGS__); \
}


#if UENABLE_DEBUG_TRACES  
  #define UTRACE(msg, ...) __ULOG_MSG(msg, ::uncanny::ELogLevel::TRACE, __VA_ARGS__)
  #define UDEBUG(msg, ...) __ULOG_MSG(msg, ::uncanny::ELogLevel::DEBUG, __VA_ARGS__)
  #define UINFO(msg, ...) __ULOG_MSG(msg, ::uncanny::ELogLevel::INFO, __VA_ARGS__)
  #define UWARN(msg, ...) __ULOG_MSG(msg, ::uncanny::ELogLevel::WARN, __VA_ARGS__)
  #define UERROR(msg, ...) __ULOG_MSG(msg, ::uncanny::ELogLevel::ERROR, __VA_ARGS__)
  #define UCRITICAL(msg, ...) __ULOG_MSG(msg, ::uncanny::ELogLevel::CRITICAL, __VA_ARGS__)
#else
  #define UTRACE(msg, ...)
  #define UDEBUG(msg, ...)
  #define UINFO(msg, ...)
  #define UWARN(msg, ...)
  #define UERROR(msg, ...)
  #define UCRITICAL(msg, ...)
#endif


#endif //UNCANNYENGINE_LOGIMPL_H
