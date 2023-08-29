
#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>


namespace uncanny
{

std::shared_ptr<spdlog::logger> FLog::s_pLogger{ nullptr };


void FLog::create()
{
  spdlog::set_pattern("%^%T:[%s:%#]:[%!]: %v%$");
  s_pLogger = spdlog::stdout_color_mt("UncannyEngine");
  s_pLogger->set_level(spdlog::level::trace);
}


spdlog::level::level_enum FLog::ConvertLogLevel(ELogLevel level)
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


}
