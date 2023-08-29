
#ifndef UNCANNYENGINE_LOGIMPL_H
#define UNCANNYENGINE_LOGIMPL_H


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


class FLogImpl
{
public:

  static void init(std::shared_ptr<spdlog::logger>& logger);

  template<typename... Args>
  static void log(const char* msg, ELogLevel level, const char* filename, int line, const char* functionName,
                  Args&&... args)
  {
    constexpr spdlog::source_loc sourceLocation{ filename, (int)line, functionName };
    spdlog::level::level_enum spdlogLevel = ConvertLogLevel(level);
    std::string msgStr = std::vformat(msg, std::make_format_args(std::forward<Args>(args)...));
    s_pLogger->log(sourceLocation, spdlogLevel, msgStr);
  }

  static std::shared_ptr<spdlog::logger> s_pLogger;

private:

  constexpr static spdlog::level::level_enum ConvertLogLevel(ELogLevel level)
  {
    switch (level)
    {
    case ELogLevel::TRACE:
      return spdlog::level::trace;
    case ELogLevel::DEBUG:
      return spdlog::level::debug;
    case ELogLevel::INFO:
      return spdlog::level::info;
    case ELogLevel::WARN:
      return spdlog::level::warn;
    case ELogLevel::ERROR:
      return spdlog::level::err;
    case ELogLevel::CRITICAL:
      return spdlog::level::critical;
    default:
      return spdlog::level::err;
    }
  }

};


}


#define __ULOG_MSG(msg, level, ...) \
{ \
  constexpr std::source_location loc = std::source_location::current(); \
  ::uncanny::FLogImpl::log(msg, level, loc.file_name(), loc.line(), loc.function_name(), __VA_ARGS__); \
}


#endif //UNCANNYENGINE_LOGIMPL_H
