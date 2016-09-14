/**
* @file  log.cpp
* @brief  日志
* @author  zhaozhongguo
* @date  2016-9-12 
* @version  1.0
*/
#include "configurator.h"
#include "fileappender.h"  
#include "layout.h"


log4cplus::Logger logger;

void initLogger()
{    
    log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("../../conf/log4cplus.properties"));
    logger = log4cplus::Logger::getRoot();
}
