
#ifndef UNCANNYENGINE_LOG_H
#define UNCANNYENGINE_LOG_H


#define UENABLE_DEBUG_TRACES 1

#include "LogImpl.h"


namespace uncanny
{


class FLog
{
public:

  static void create()
  {
    FLogImpl::init(uncanny::FLogImpl::s_pLogger);
  }

};


#if UENABLE_DEBUG_TRACES
  #define UTRACE(msg, ...)    __ULOG_MSG(msg, ::uncanny::ELogLevel::TRACE, __VA_ARGS__)
  #define UDEBUG(msg, ...)    __ULOG_MSG(msg, ::uncanny::ELogLevel::DEBUG, __VA_ARGS__)
  #define UINFO(msg, ...)    __ULOG_MSG(msg, ::uncanny::ELogLevel::INFO, __VA_ARGS__)
  #define UWARN(msg, ...)    __ULOG_MSG(msg, ::uncanny::ELogLevel::WARN, __VA_ARGS__)
  #define UERROR(msg, ...)    __ULOG_MSG(msg, ::uncanny::ELogLevel::ERROR, __VA_ARGS__)
  #define UCRITICAL(msg, ...)    __ULOG_MSG(msg, ::uncanny::ELogLevel::CRITICAL, __VA_ARGS__)
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
