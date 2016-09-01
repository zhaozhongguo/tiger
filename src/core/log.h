/*
 * GNFlush SDN Controller GPL Source Code
 * Copyright (C) 2016, Greenet <greenet@greenet.net.cn>
 *
 * This file is part of the GNFlush SDN Controller. GNFlush SDN
 * Controller is a free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, , see <http://www.gnu.org/licenses/>.
 */

/******************************************************************************
*                                                                             *
*   File Name   : log.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
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

#define LOG_TRACE_FMT(...)              LOG4CPLUS_TRACE_FMT(logger, __VA_ARGS__)
#define LOG_DEBUG_FMT(...)              LOG4CPLUS_DEBUG_FMT(logger, __VA_ARGS__)
#define LOG_INFO_FMT(...)               LOG4CPLUS_INFO_FMT(logger, __VA_ARGS__)
#define LOG_WARN_FMT(...)               LOG4CPLUS_WARN_FMT(logger, __VA_ARGS__)
#define LOG_ERROR_FMT(...)              LOG4CPLUS_ERROR_FMT(logger, __VA_ARGS__)
#define LOG_FATAL_FMT(...)              LOG4CPLUS_FATAL_FMT(logger, __VA_ARGS__)

// 宏, 用来简化, 代替的判断+输出log+return返回值
#define LOG_IF_RETURN(c, r, l) \
	if (c) \
	{ \
		LOG_INFO((l)); \
		return (r);    \
	}

// 宏, 用来简化, 代替的判断+输出log+return
#define LOG_IF_VOID(c, l) \
	if (c) \
	{ \
		LOG_INFO((l)); \
		return ;    \
	}

#endif
