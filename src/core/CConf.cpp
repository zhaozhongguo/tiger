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
*   File Name   : CConf.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "CConf.h"
#include "log.h"

std::string CONFIGURE_FILE = "../conf/controller.conf";

CConf::CConf()
{
    m_strFilename = CONFIGURE_FILE;
}



CConf::CConf(const INT1* filename):CRefObj(FALSE)
{
    m_strFilename = std::string(filename);
}



CConf::~CConf()
{
    std::map<std::string, std::map<std::string, std::string>* >::iterator iter;
    std::map<std::string, std::string>::iterator it;
    for (iter = m_mapConfigs.begin(); iter != m_mapConfigs.end(); iter++)
    {
        iter->second->erase(iter->second->begin(), iter->second->end());
        delete iter->second;
    }
    m_mapConfigs.erase(m_mapConfigs.begin(), m_mapConfigs.end());
}



void CConf::loadConf()
{
    if (m_strFilename.empty())
    {
        m_strFilename = CONFIGURE_FILE;
    }

    std::ifstream file(m_strFilename.c_str());
    char buf[256] = {0};
    std::map<std::string, std::string>* block = NULL;
    while (!file.eof())
    {
        memset(buf, 0, 256);
        file.getline(buf, 256);
        std::string line(buf);
        switch (buf[0])
        {
            case '\n':
            case '\0':
            case '#':
            case ' ':
            case '\t':
            {
                break;
            }
            case '[':
            {
                block = new std::map<std::string, std::string>();
                m_mapConfigs.insert(std::pair<std::string, std::map<std::string, std::string>* >(line.substr(1, line.find(']') - 1), block));
                break;
            }
            default:
            {
                std::string::size_type pos = line.find('=');
                if (std::string::npos != pos)
                {
                    block->insert(std::pair<std::string, std::string>(line.substr(0, pos), line.substr(pos + 1, line.size())));
                }
                else
                {
                    block->insert(std::pair<std::string, std::string>(line, line));
                }
                break;
            }
        }
    }

    file.close();
    LOG_INFO_FMT("load %s success", m_strFilename.c_str());
}


const INT1* CConf::getConfig(const INT1* block, const INT1* key)
{
    std::map<std::string, std::map<std::string, std::string>* >::iterator iter = m_mapConfigs.find(std::string(block));
    if (iter != m_mapConfigs.end())
    {
        std::map<std::string, std::string>::iterator it = iter->second->find(std::string(key));
        if (it != iter->second->end())
        {
            return (it->second).c_str();
        }
    }

    return NULL;
}


std::map<std::string, std::string>* CConf::getConfig(const INT1* block)
{
    std::map<std::string, std::map<std::string, std::string>* >::iterator iter = m_mapConfigs.find(std::string(block));
    if (iter != m_mapConfigs.end())
    {
        return iter->second;
    }

    return NULL;
}


