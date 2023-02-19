
#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>


namespace uncanny {


std::shared_ptr<spdlog::logger> FLoggerImpl::s_LoggerPtr{ nullptr };


void FLoggerImpl::init(std::shared_ptr<spdlog::logger>& logger) {
  spdlog::set_pattern("%^%T:[%s:%#]:[%!]: %v%$");
  logger = spdlog::stdout_color_mt("MAREngine");
  logger->set_level(spdlog::level::trace);
}


FLoggerImpl FLogger::s_LoggerImpl{};


}
