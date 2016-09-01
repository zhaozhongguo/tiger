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
*   File Name   : CConf.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef __CCONF__H
#define __CCONF__H
#include "bnc-type.h"
#include "CRefObj.h"

extern std::string CONFIGURE_FILE;

/*
 * 配置类
 */
class CConf: public CRefObj
{
public:
    CConf();
    CConf(const INT1* filename);
    ~CConf();

    /*
     * 加载配置文件
     */
    void loadConf();

    /*
     * 获取配置项
     * param block: 配置块名称
     * param key: 配置项名称
     * ret: 返回配置项
     */
    const INT1* getConfig(const INT1* block, const INT1* key);

    /*
     * 获取块配置项
     * param block: 配置块名称
     * ret: 返回块配置项
     */
    std::map<std::string, std::string>* getConfig(const INT1* block);

private:
    std::map<std::string, std::map<std::string, std::string>* > m_mapConfigs;
    std::string m_strFilename;
};



#endif
