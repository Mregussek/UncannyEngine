
#ifndef UNCANNYENGINE_LOG_H
#define UNCANNYENGINE_LOG_H


#define UENABLE_DEBUG_TRACES 0

#include "LogImpl.h"


namespace uncanny
{


class FLog
{
public:

  static void create()
  {
    FLogImpl::init(uncanny::FLogImpl::s_LoggerPtr);
  }

  template<ConceptStringLiteral TString, ConceptCodeLine Line, typename... Args>
  static void trace(TString msg, TString filename, Line line, TString functionName,
                    Args&&... args)
  {
    FLogImpl::trace(msg, filename, line, functionName, std::forward<Args>(args)...);
  }

  template<ConceptStringLiteral TString, ConceptCodeLine Line, typename... Args>
  static void debug(TString msg, TString filename, Line line, TString functionName,
                    Args&&... args)
  {
    FLogImpl::debug(msg, filename, line, functionName, std::forward<Args>(args)...);
  }

  template<ConceptStringLiteral TString, ConceptCodeLine Line, typename... Args>
  static void info(TString msg, TString filename, Line line, TString functionName,
                   Args&&... args)
  {
    FLogImpl::info(msg, filename, line, functionName, std::forward<Args>(args)...);
  }

  template<ConceptStringLiteral TString, ConceptCodeLine Line, typename... Args>
  static void warn(TString msg, TString filename, Line line, TString functionName,
                   Args&&... args)
  {
    FLogImpl::warn(msg, filename, line, functionName, std::forward<Args>(args)...);
  }

  template<ConceptStringLiteral TString, ConceptCodeLine Line, typename... Args>
  static void error(TString msg, TString filename, Line line, TString functionName,
                    Args&&... args)
  {
    FLogImpl::error(msg, filename, line, functionName, std::forward<Args>(args)...);
  }

  template<ConceptStringLiteral TString, ConceptCodeLine Line, typename... Args>
  static void critical(TString msg, TString filename, Line line, TString functionName,
                       Args&&... args)
  {
    FLogImpl::critical(msg, filename, line, functionName, std::forward<Args>(args)...);
  }

private:

  static FLogImpl s_LoggerImpl;

};


#ifndef U_FUNCTION_NAME
  #ifdef WIN32   // WINDOWS
    #define U_FUNCTION_NAME   __FUNCTION__
  #else          // UNIX
    #define U_FUNCTION_NAME   __func__
  #endif
#endif


#if UENABLE_DEBUG_TRACES
  #define UTRACE(msg, ...) ::uncanny::FLog::trace(std::string{ msg }, std::string{ __FILE__ }, __LINE__, std::string{ U_FUNCTION_NAME }, __VA_ARGS__)
  #define UDEBUG(msg, ...) ::uncanny::FLog::debug(std::string{ msg }, std::string{ __FILE__ }, __LINE__, std::string{ U_FUNCTION_NAME }, __VA_ARGS__)
  #define UINFO(msg, ...) ::uncanny::FLog::info(std::string{ msg }, std::string{ __FILE__ }, __LINE__, std::string{ U_FUNCTION_NAME }, __VA_ARGS__)
  #define UWARN(msg, ...) ::uncanny::FLog::warn(std::string{ msg }, std::string{ __FILE__ }, __LINE__, std::string{ U_FUNCTION_NAME }, __VA_ARGS__)
  #define UERROR(msg, ...) ::uncanny::FLog::error(std::string{ msg }, std::string{ __FILE__ }, __LINE__, std::string{ U_FUNCTION_NAME }, __VA_ARGS__)
  #define UCRITICAL(msg, ...) ::uncanny::FLog::critical(std::string{ msg }, std::string{ __FILE__ }, __LINE__, std::string{ U_FUNCTION_NAME }, __VA_ARGS__)
#else
  #define UTRACE(msg, ...)
  #define UDEBUG(msg, ...)
  #define UINFO(msg, ...)
  #define UWARN(msg, ...)
  #define UERROR(msg, ...)
  #define UCRITICAL(msg, ...)
#endif


}


#endif //UNCANNYENGINE_LOG_H
