
#ifndef UNCANNYENGINE_LOG_H
#define UNCANNYENGINE_LOG_H


#include "LogImpl.h"


namespace uncanny {


class FLog {
public:

  static void create() {
    FLogImpl::init(uncanny::FLogImpl::s_LoggerPtr);
  }

  template<ConceptStringLiteral Msg, ConceptCodeLine Line, typename... Args>
  static void trace(Msg msg, Msg filename, Line line, Msg functionName, Args&&... args) {
    FLogImpl::trace(msg, filename, line, functionName, std::forward<Args>(args)...);
  }

  template<ConceptStringLiteral Msg, ConceptCodeLine Line, typename... Args>
  static void debug(Msg msg, Msg filename, Line line, Msg functionName, Args&&... args) {
    FLogImpl::debug(msg, filename, line, functionName, std::forward<Args>(args)...);
  }

  template<ConceptStringLiteral Msg, ConceptCodeLine Line, typename... Args>
  static void info(Msg msg, Msg filename, Line line, Msg functionName, Args&&... args) {
    FLogImpl::info(msg, filename, line, functionName, std::forward<Args>(args)...);
  }

  template<ConceptStringLiteral Msg, ConceptCodeLine Line, typename... Args>
  static void warn(Msg msg, Msg filename, Line line, Msg functionName, Args&&... args) {
    FLogImpl::warn(msg, filename, line, functionName, std::forward<Args>(args)...);
  }

  template<ConceptStringLiteral Msg, ConceptCodeLine Line, typename... Args>
  static void error(Msg msg, Msg filename, Line line, Msg functionName, Args&&... args) {
    FLogImpl::error(msg, filename, line, functionName, std::forward<Args>(args)...);
  }

  template<ConceptStringLiteral Msg, ConceptCodeLine Line, typename... Args>
  static void critical(Msg msg, Msg filename, Line line, Msg functionName, Args&&... args) {
    FLogImpl::critical(msg, filename, line, functionName, std::forward<Args>(args)...);
  }

private:

  static FLogImpl s_LoggerImpl;

};


#ifndef U_FUNCTION_NAME
  #ifdef WIN32   //WINDOWS
    #define U_FUNCTION_NAME   __FUNCTION__
  #else          //*NIX
    #define U_FUNCTION_NAME   __func__
  #endif
#endif

#define UTRACE(msg, ...) ::uncanny::FLog::trace(msg, __FILE__, __LINE__, U_FUNCTION_NAME, __VA_ARGS__)
#define UDEBUG(msg, ...) ::uncanny::FLog::debug(msg, __FILE__, __LINE__, U_FUNCTION_NAME, __VA_ARGS__)
#define UINFO(msg, ...) ::uncanny::FLog::info(msg, __FILE__, __LINE__, U_FUNCTION_NAME, __VA_ARGS__)
#define UWARN(msg, ...) ::uncanny::FLog::warn(msg, __FILE__, __LINE__, U_FUNCTION_NAME, __VA_ARGS__)
#define UERROR(msg, ...) ::uncanny::FLog::error(msg, __FILE__, __LINE__, U_FUNCTION_NAME, __VA_ARGS__)
#define UCRITICAL(msg, ...) ::uncanny::FLog::critical(msg, __FILE__, __LINE__, U_FUNCTION_NAME, __VA_ARGS__)


}


#endif //UNCANNYENGINE_LOG_H
