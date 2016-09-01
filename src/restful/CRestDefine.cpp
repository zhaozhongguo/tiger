#include "CRestDefine.h"
#include "comm-util.h"
#include "log.h"

CRestDefine* CRestDefine::m_pInstance = 0;

CRestDefine::CRestDefine()
{
}

CRestDefine::~CRestDefine()
{
}

bnc::restful::http_method CRestDefine::getMethodFromStr(const std::string & raw)
{
	STL_FOR_LOOP(m_methodList, iter)
	{
		if (std::string::npos != raw.find(iter->second))
		{
			return iter->first;
		}
	}

	return (bnc::restful::HTTP_METHOD_OTHER);
}

bnc::restful::http_version CRestDefine::getVersionFromStr(const std::string & raw)
{
	STL_FOR_LOOP(m_versionList, iter)
	{
		if (std::string::npos != raw.find(iter->second))
		{
			return iter->first;
		}
	}

	return (bnc::restful::HTTP_VERSION_OHTER);
}

void CRestDefine::getPathFromStr(const std::string & raw, std::string & path)
{
	std::list<std::string> singleList;
	split(raw, " ", singleList);

	STL_FOR_LOOP(singleList, iter)
	{
		std::string str = *iter;
		if (str[0] == '/')
		{
			path = str;
		}
	}
}

void CRestDefine::getStrFromMethod(bnc::restful::http_method method, std::string & str_method)
{
	method_map::iterator iter = m_methodList.find(method);

	if (m_methodList.end() != iter)
	{
		str_method = iter->second;
	}
}

void CRestDefine::getStrFromVersion(bnc::restful::http_version version, std::string & str_version)
{
	version_map::iterator iter = m_versionList.find(version);

	if (m_versionList.end() != iter)
	{
		str_version = iter->second;
	}
}

void CRestDefine::getStrFromStatus(bnc::restful::http_status status, std::string & str_status)
{
	status_map::iterator iter = m_statusList.find(status);

	if (m_statusList.end() != iter)
	{
		str_status = iter->second;
	}
}

CRestDefine* CRestDefine::getInstance()
{
	if (NULL == m_pInstance) {
		LOG_INFO("initialize Rest Define.");
		m_pInstance = new CRestDefine();
		m_pInstance->init();
	}

	return m_pInstance;
}

void CRestDefine::init()
{
	// 初始化时, 将enum和对应字符串存入list中, 以便于后续的查找对应关系
	m_methodList.insert(method_map::value_type (bnc::restful::HTTP_GET, "GET"));
	m_methodList.insert(method_map::value_type (bnc::restful::HTTP_POST, "POST"));
	m_methodList.insert(method_map::value_type (bnc::restful::HTTP_PUT, "PUT"));
	m_methodList.insert(method_map::value_type (bnc::restful::HTTP_DELETE, "DELETE"));

	m_versionList.insert(version_map::value_type (bnc::restful::HTTP_1_0, "HTTP/1.0"));
	m_versionList.insert(version_map::value_type (bnc::restful::HTTP_1_1, "HTTP/1.1"));

	m_statusList.insert(status_map::value_type (bnc::restful::STATUS_OK, "200 OK"));
	m_statusList.insert(status_map::value_type (bnc::restful::STATUS_BAD_REQUEST, "400 BAD REQUEST"));
	m_statusList.insert(status_map::value_type (bnc::restful::STATUS_UNAUTHORIZED, "401 UNAUTHORIZED"));
	m_statusList.insert(status_map::value_type (bnc::restful::STATUS_FORBIDDEN, "403 FORBIDDEN"));
	m_statusList.insert(status_map::value_type (bnc::restful::STATUS_NOT_FOUND, "404 NOT FOUND"));
}




