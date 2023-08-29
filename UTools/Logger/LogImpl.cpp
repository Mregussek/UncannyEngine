
#include "LogImpl.h"
#include <spdlog/sinks/stdout_color_sinks.h>


namespace uncanny
{

std::shared_ptr<spdlog::logger> FLogImpl::s_pLogger{ nullptr };


void FLogImpl::init(std::shared_ptr<spdlog::logger>& logger)
{
  spdlog::set_pattern("%^%T:[%s:%#]:[%!]: %v%$");
  logger = spdlog::stdout_color_mt("UncannyEngine");
  logger->set_level(spdlog::level::trace);
}


}
