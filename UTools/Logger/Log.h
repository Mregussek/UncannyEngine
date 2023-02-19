
#ifndef UNCANNYENGINE_LOG_H
#define UNCANNYENGINE_LOG_H


#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>

#include <memory>


namespace uncanny {


class FLoggerImpl {
public:

  static void init(std::shared_ptr<spdlog::logger>& logger);

  static std::shared_ptr<spdlog::logger> s_LoggerPtr;

};


template<typename T>
concept MessageConcept = std::is_same_v<T, std::string> ||
                          std::is_same_v<T, const char*>;


class FLogger {
public:

  static void create() {
    FLoggerImpl::init(uncanny::FLoggerImpl::s_LoggerPtr);
  }

  template<MessageConcept Msg, typename... Args>
  static void trace(Msg msg, Args&&... args) {
    SPDLOG_LOGGER_TRACE(s_LoggerImpl.s_LoggerPtr, msg, std::forward<Args>(args)...);
  }

  template<MessageConcept Msg, typename... Args>
  static void debug(Msg msg, Args&&... args) {
    SPDLOG_LOGGER_DEBUG(s_LoggerImpl.s_LoggerPtr, msg, std::forward<Args>(args)...);
  }

  template<MessageConcept Msg, typename... Args>
  static void info(Msg msg, Args&&... args) {
    SPDLOG_LOGGER_INFO(s_LoggerImpl.s_LoggerPtr, msg, std::forward<Args>(args)...);
  }

  template<MessageConcept Msg, typename... Args>
  static void warn(Msg msg, Args&&... args) {
    SPDLOG_LOGGER_WARN(s_LoggerImpl.s_LoggerPtr, msg, std::forward<Args>(args)...);
  }

  template<MessageConcept Msg, typename... Args>
  static void error(Msg msg, Args&&... args) {
    SPDLOG_LOGGER_ERROR(s_LoggerImpl.s_LoggerPtr, msg, std::forward<Args>(args)...);
  }

  template<MessageConcept Msg, typename... Args>
  static void critical(Msg msg, Args&&... args) {
    SPDLOG_LOGGER_CRITICAL(s_LoggerImpl.s_LoggerPtr, msg, std::forward<Args>(args)...);
  }

private:

  static FLoggerImpl s_LoggerImpl;

};


}


#endif //UNCANNYENGINE_LOG_H
