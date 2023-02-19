
#ifndef UNCANNYENGINE_LOGIMPL_H
#define UNCANNYENGINE_LOGIMPL_H


#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>

#include <memory>


namespace uncanny {


template<typename T>
concept ConceptLogMessage = std::is_same_v<T, const char*>;


class FLogImpl {
public:

  static void init(std::shared_ptr<spdlog::logger>& logger);

  template<ConceptLogMessage Msg, typename... Args>
  static void trace(Msg msg, Args&&... args) {
    SPDLOG_LOGGER_TRACE(s_LoggerPtr, msg, std::forward<Args>(args)...);
  }

  template<ConceptLogMessage Msg, typename... Args>
  static void debug(Msg msg, Args&&... args) {
    SPDLOG_LOGGER_DEBUG(s_LoggerPtr, msg, std::forward<Args>(args)...);
  }

  template<ConceptLogMessage Msg, typename... Args>
  static void info(Msg msg, Args&&... args) {
    SPDLOG_LOGGER_INFO(s_LoggerPtr, msg, std::forward<Args>(args)...);
  }

  template<ConceptLogMessage Msg, typename... Args>
  static void warn(Msg msg, Args&&... args) {
    SPDLOG_LOGGER_WARN(s_LoggerPtr, msg, std::forward<Args>(args)...);
  }

  template<ConceptLogMessage Msg, typename... Args>
  static void error(Msg msg, Args&&... args) {
    SPDLOG_LOGGER_ERROR(s_LoggerPtr, msg, std::forward<Args>(args)...);
  }

  template<ConceptLogMessage Msg, typename... Args>
  static void critical(Msg msg, Args&&... args) {
    SPDLOG_LOGGER_CRITICAL(s_LoggerPtr, msg, std::forward<Args>(args)...);
  }

  static std::shared_ptr<spdlog::logger> s_LoggerPtr;

};


}


#endif //UNCANNYENGINE_LOGIMPL_H
