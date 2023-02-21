
#ifndef UNCANNYENGINE_LOGIMPL_H
#define UNCANNYENGINE_LOGIMPL_H


#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>

#include <memory>


namespace uncanny {


template<typename T>
concept ConceptStringLiteral = std::is_same_v<T, const char*> || std::is_same_v<T, std::string>;


template<typename T>
concept ConceptCodeLine = std::is_arithmetic_v<T>;


class FLogImpl {
public:

  static void init(std::shared_ptr<spdlog::logger>& logger);

  template<ConceptStringLiteral Msg, ConceptCodeLine Line, typename... Args>
  static void trace(Msg msg, Msg filename, Line line, Msg functionName, Args&&... args) {
    s_LoggerPtr->log(spdlog::source_loc{filename, line, functionName}, spdlog::level::trace,
                     msg, std::forward<Args>(args)...);
  }

  template<ConceptStringLiteral Msg, ConceptCodeLine Line, typename... Args>
  static void debug(Msg msg, Msg filename, Line line, Msg functionName, Args&&... args) {
    s_LoggerPtr->log(spdlog::source_loc{filename, line, functionName}, spdlog::level::debug,
                     msg, std::forward<Args>(args)...);
  }

  template<ConceptStringLiteral Msg, ConceptCodeLine Line, typename... Args>
  static void info(Msg msg, Msg filename, Line line, Msg functionName, Args&&... args) {
    s_LoggerPtr->log(spdlog::source_loc{filename, line, functionName}, spdlog::level::info,
                     msg, std::forward<Args>(args)...);
  }

  template<ConceptStringLiteral Msg, ConceptCodeLine Line, typename... Args>
  static void warn(Msg msg, Msg filename, Line line, Msg functionName, Args&&... args) {
    s_LoggerPtr->log(spdlog::source_loc{filename, line, functionName}, spdlog::level::warn,
                     msg, std::forward<Args>(args)...);
  }

  template<ConceptStringLiteral Msg, ConceptCodeLine Line, typename... Args>
  static void error(Msg msg, Msg filename, Line line, Msg functionName, Args&&... args) {
    s_LoggerPtr->log(spdlog::source_loc{filename, line, functionName}, spdlog::level::err,
                     msg, std::forward<Args>(args)...);
  }

  template<ConceptStringLiteral Msg, ConceptCodeLine Line, typename... Args>
  static void critical(Msg msg, Msg filename, Line line, Msg functionName, Args&&... args) {
    s_LoggerPtr->log(spdlog::source_loc{filename, line, functionName}, spdlog::level::critical,
                     msg, std::forward<Args>(args)...);
  }

  static std::shared_ptr<spdlog::logger> s_LoggerPtr;

};


}


#endif //UNCANNYENGINE_LOGIMPL_H
