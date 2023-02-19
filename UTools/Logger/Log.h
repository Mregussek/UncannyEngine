
#ifndef UNCANNYENGINE_LOG_H
#define UNCANNYENGINE_LOG_H


#include "LogImpl.h"


namespace uncanny {


class FLog {
public:

  static void create() {
    FLogImpl::init(uncanny::FLogImpl::s_LoggerPtr);
  }

  template<ConceptLogMessage Msg, typename... Args>
  static void trace(Msg msg, Args&&... args) {
    FLogImpl::trace(msg, std::forward<Args>(args)...);
  }

  template<ConceptLogMessage Msg, typename... Args>
  static void debug(Msg msg, Args&&... args) {
    FLogImpl::debug(msg, std::forward<Args>(args)...);
  }

  template<ConceptLogMessage Msg, typename... Args>
  static void info(Msg msg, Args&&... args) {
    FLogImpl::info(msg, std::forward<Args>(args)...);
  }

  template<ConceptLogMessage Msg, typename... Args>
  static void warn(Msg msg, Args&&... args) {
    FLogImpl::warn(msg, std::forward<Args>(args)...);
  }

  template<ConceptLogMessage Msg, typename... Args>
  static void error(Msg msg, Args&&... args) {
    FLogImpl::error(msg, std::forward<Args>(args)...);
  }

  template<ConceptLogMessage Msg, typename... Args>
  static void critical(Msg msg, Args&&... args) {
    FLogImpl::critical(msg, std::forward<Args>(args)...);
  }

private:

  static FLogImpl s_LoggerImpl;

};


}


#endif //UNCANNYENGINE_LOG_H
