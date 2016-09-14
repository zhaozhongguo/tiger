/**
* @file  config.cpp
* @brief  配置解析类定义
* @author  zhaozhongguo
* @date  2016-9-12 
* @version  1.0
*/
#include <fstream>
#include "config.h"
#include "log.h"
namespace common
{

    CConfig::~CConfig()
    {
        CConfig::clear();
    }


    void CConfig::loadConfig()
    {
        if (m_strFilename.empty())
        {
            LOG_ERROR_FMT("config filename is empty!");
            return;
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
                    m_mConfigs.insert(std::pair<std::string, std::map<std::string, std::string>* >(line.substr(1, line.find(']') - 1), block));
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
        LOG_INFO_FMT("load %s file success", m_strFilename.c_str());
    }


    const char* CConfig::getConfig(const char* block, const char* key)
    {
        std::map<std::string, std::map<std::string, std::string>* >::iterator iter = m_mConfigs.find(std::string(block));
        if (iter != m_mConfigs.end())
        {
            std::map<std::string, std::string>::iterator it = iter->second->find(std::string(key));
            if (it != iter->second->end())
            {
                return (it->second).c_str();
            }
        }

        return NULL;
    }


    void CConfig::clear()
    {
        std::map<std::string, std::map<std::string, std::string>* >::iterator iter;
        std::map<std::string, std::string>::iterator it;
        for (iter = m_mConfigs.begin(); iter != m_mConfigs.end(); iter++)
        {
            iter->second->erase(iter->second->begin(), iter->second->end());
            delete iter->second;
        }
        m_mConfigs.erase(m_mConfigs.begin(), m_mConfigs.end());
    }

}