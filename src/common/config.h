/**
* @file  config.h
* @brief  配置解析类定义
* @author  zhaozhongguo
* @date  2016-9-12 
* @version  1.0
*/
#ifndef __CONFIG__H
#define __CONFIG__H
#include <map>
#include <string>

namespace common
{
    const char* COMMON_CONFIG_FILE = "../../conf/controller.conf";

    /** 
    * @brief 配置解析工具
    * 用于解析配置文件
    */ 
    class CConfig:
    {
    public:
        CConfig()
        {
            m_filename = "";
        }

        CConfig(const char* filename)
        {
            m_filename = std::string(filename);
        }

        ~CConfig();

        /** 
        * @brief 加载配置文件
        */
        void loadConfig();

        /** 
        * @brief 获取配置项
        * @param block：配置块名称
        * @param key：配置项键值
        * @return 配置项value值
        */
        const char* getConfig(const char* block, const char* key);

        /** 
        * @brief 清空缓存配置
        */
        void clear();

    private:
        std::map<std::string, std::map<std::string, std::string>* > m_mConfigs;
        std::string m_strFilename;
    };

}


#endif
