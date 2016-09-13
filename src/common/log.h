/**
* @file  log.h
* @brief  日志
* @author  zhaozhongguo
* @date  2016-9-12 
* @version  1.0
*/
#ifndef __LOG_H
#define __LOG_H

#include "logger.h"
#include "loggingmacros.h"


extern log4cplus::Logger logger;
extern void initLogger(); 

//日志宏
#define LOG_TRACE(logEvent)         LOG4CPLUS_TRACE(logger, logEvent)
#define LOG_DEBUG(logEvent)         LOG4CPLUS_DEBUG(logger, logEvent)
#define LOG_INFO(logEvent)          LOG4CPLUS_INFO(logger, logEvent)
#define LOG_WARN(logEvent)          LOG4CPLUS_WARN(logger, logEvent)
#define LOG_ERROR(logEvent)         LOG4CPLUS_ERROR(logger, logEvent)
#define LOG_FATAL(logEvent)         LOG4CPLUS_FATAL(logger, logEvent)

#define LOG_TRACE_FMT(...)          LOG4CPLUS_TRACE_FMT(logger, __VA_ARGS__)
#define LOG_DEBUG_FMT(...)          LOG4CPLUS_DEBUG_FMT(logger, __VA_ARGS__)
#define LOG_INFO_FMT(...)           LOG4CPLUS_INFO_FMT(logger, __VA_ARGS__)
#define LOG_WARN_FMT(...)           LOG4CPLUS_WARN_FMT(logger, __VA_ARGS__)
#define LOG_ERROR_FMT(...)          LOG4CPLUS_ERROR_FMT(logger, __VA_ARGS__)
#define LOG_FATAL_FMT(...)          LOG4CPLUS_FATAL_FMT(logger, __VA_ARGS__)


#endif
