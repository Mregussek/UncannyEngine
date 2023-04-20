
#ifndef UNCANNYENGINE_LOGIMPL_H
#define UNCANNYENGINE_LOGIMPL_H


#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#define SPDLOG_USE_STD_FORMAT
#include <spdlog/spdlog.h>

#include <memory>


namespace uncanny
{


template<typename T>
concept ConceptStringLiteral = std::is_same_v<T, std::string> || std::is_same_v<T, std::string&> ||
    std::is_same_v<T, char[]> || std::is_same_v<T, char*> || std::is_same_v<T, const char*>;


template<typename T>
concept ConceptCodeLine = std::is_arithmetic_v<T>;


class FLogImpl
{
public:

  static void init(std::shared_ptr<spdlog::logger>& logger);

  template<ConceptStringLiteral TString, ConceptCodeLine Line, typename... Args>
  static void trace(TString msg, TString filename, Line line, TString functionName,
                    Args&&... args)
  {
    s_LoggerPtr->log(spdlog::source_loc{filename.c_str(), line, functionName.c_str()}, spdlog::level::trace, msg,
                     std::forward<Args>(args)...);
  }

  template<ConceptStringLiteral TString, ConceptCodeLine Line, typename... Args>
  static void debug(TString msg, TString filename, Line line, TString functionName,
                    Args&&... args)
  {
    s_LoggerPtr->log(spdlog::source_loc{filename.c_str(), line, functionName.c_str()}, spdlog::level::debug, msg,
                     std::forward<Args>(args)...);
  }

  template<ConceptStringLiteral TString, ConceptCodeLine Line, typename... Args>
  static void info(TString msg, TString filename, Line line, TString functionName,
                   Args&&... args)
  {
    s_LoggerPtr->log(spdlog::source_loc{filename.c_str(), line, functionName.c_str()}, spdlog::level::info, msg,
                     std::forward<Args>(args)...);
  }

  template<ConceptStringLiteral TString, ConceptCodeLine Line, typename... Args>
  static void warn(TString msg, TString filename, Line line, TString functionName,
                   Args&&... args)
  {
    s_LoggerPtr->log(spdlog::source_loc{filename.c_str(), line, functionName.c_str()}, spdlog::level::warn, msg,
                     std::forward<Args>(args)...);
  }

  template<ConceptStringLiteral TString, ConceptCodeLine Line, typename... Args>
  static void error(TString msg, TString filename, Line line, TString functionName,
                    Args&&... args)
  {
    s_LoggerPtr->log(spdlog::source_loc{ filename.c_str(), line, functionName.c_str() }, spdlog::level::err, msg,
                     std::forward<Args>(args)...);
  }

  template<ConceptStringLiteral TString, ConceptCodeLine Line, typename... Args>
  static void critical(TString msg, TString filename, Line line, TString functionName,
                       Args&&... args)
  {
    s_LoggerPtr->log(spdlog::source_loc{filename.c_str(), line, functionName.c_str()}, spdlog::level::critical, msg,
                     std::forward<Args>(args)...);
  }

  static std::shared_ptr<spdlog::logger> s_LoggerPtr;

};


}


#endif //UNCANNYENGINE_LOGIMPL_H
