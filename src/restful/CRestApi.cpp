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
*   File Name   : CRestApiHandler.cpp          *
*   Author      : bnc xflu           		*
*   Create Date : 2016-7-21           		*
*   Version     : 1.0           			*
*   Function    : .           				*
*                                                                             *
******************************************************************************/
#include "CRestApi.h"
#include "log.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include <sstream>
#include "bnc-type.h"
#include "comm-util.h"
#include "CServer.h"
#include "CHostMgr.h"
#include "CRestManager.h"
#include "log.h"

using namespace rapidjson;

void api_list(CRestRequest* request, CRestResponse* response)
{
	std::list<std::string> list;
	CRestManager::getInstance()->getRestApiMgr()->getRestApiPathList(list);

	StringBuffer strBuff;
	Writer<StringBuffer> writer(strBuff);

	writer.StartObject();
	writer.Key("registered api list");
	writer.StartArray();

	STL_FOR_LOOP(list, iter)
	{
		writer.StartObject();

		writer.Key("Path: ");
		writer.String((*iter).c_str());

		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();

	std::string body(strBuff.GetString());

	response->setResponse(request->getVersion(), bnc::restful::STATUS_OK, body);
}


void api_list_detail(CRestRequest* request, CRestResponse* response)
{
	// std::list<std::string> list;
	std::list<restVar> list;
	CRestManager::getInstance()->getRestApiMgr()->getRestApiPathList(list);

	StringBuffer strBuff;
	Writer<StringBuffer> writer(strBuff);

	writer.StartObject();
	writer.Key("registered api list");
	writer.StartArray();

	STL_FOR_LOOP(list, iter)
	{
		writer.StartObject();

		restVar var = *iter;
		std::string str_method;
		CRestManager::getInstance()->getRestDefine()->getStrFromMethod(var.second, str_method);

		writer.Key("Method: ");
		writer.String(str_method.c_str());

		writer.Key("Path: ");
		writer.String(var.first.c_str());

		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();

	std::string body(strBuff.GetString());

	response->setResponse(request->getVersion(), bnc::restful::STATUS_OK, body);
}


void api_echo(CRestRequest* request, CRestResponse* response)
{
	response->setResponse(request->getVersion(), bnc::restful::STATUS_OK, request->getBody());
}

void api_default(CRestRequest* request, CRestResponse* response)
{
	std::string body("request: "  + request->getHeaderFirstLine() + "\n" +
			"not found, you can use method:GET and uri:/list to list all api");
	response->setResponse(request->getVersion(), bnc::restful::STATUS_NOT_FOUND, body);
}

void api_json_example(CRestRequest* request, CRestResponse* response)
{
	std::string body;

    const char json[] = " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, "
    		"\"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";

	body.append("original json is ");
	body.append(json);
	body.append("\n");

	Document document;
	if (document.Parse(json).HasParseError())
	{
		LOG_INFO("json parse error");
		return ;
	}

	// 测试字符串查找和读取
	if (document.HasMember("hello") && document["hello"].IsString())
	{
		body.append("hello member is : ");
		body.append(document["hello"].GetString());
		body.append("\n");
	}

	// 测试文本查找和读取
	if (document.HasMember("i") && document["i"].IsNumber())
	{
		std::stringstream stream;
		stream<<document["i"].GetInt();
		std::string str = stream.str();

		body.append("i member is : ");
		body.append(str);
		body.append("\n");
	}

	// 测试遍历
    static const char* kTypeNames[] = { "Null", "False", "True",
    		"Object", "Array", "String", "Number" };
    Value::ConstMemberIterator itr = document.MemberBegin();
    for (; itr != document.MemberEnd(); ++itr)
    {
    	char str_temp[300] = {0};
    	sprintf(str_temp, "Type of member %s is %s\n",
    			itr->name.GetString(), kTypeNames[itr->value.GetType()]);
    	body.append(str_temp);
    }

    // 测试修改
    document["i"]  = 100;
    if (document.HasMember("i") && document["i"].IsNumber())
	{

		std::stringstream stream;
		stream<<document["i"].GetInt();
		std::string str = stream.str();

		body.append("new i member is : ");
		body.append(str);
		body.append("\n");
	}

    // 测试在数组中增加新元素
    Value& a = document["a"];   // This time we uses non-const reference.
    Document::AllocatorType& allocator = document.GetAllocator();
	for (int i = 5; i <= 10; i++)
	{
		a.PushBack(i, allocator);
	}

	// 另一种在数组中增加元素的方式
	a.PushBack("abc", allocator).PushBack("def", allocator);

	// 修改字符串
	document["hello"] = "new world";

	// 增加新的member
	Value newmember;
	{
		char buffer2[20];
		int len = sprintf(buffer2, "%s %s", "new member", "new member");
		newmember.SetString(buffer2, static_cast<SizeType>(len), document.GetAllocator());

		memset(buffer2, 0, sizeof(buffer2));
	}
	document.AddMember("new member", newmember, document.GetAllocator());

	// 将dom转换成string
	StringBuffer sb;
	PrettyWriter<StringBuffer> writer(sb);
	document.Accept(writer);
	body.append(sb.GetString());

	response->setResponse(request->getVersion(), bnc::restful::STATUS_OK, body);
}

void api_switches(CRestRequest* request, CRestResponse* response)
{
	std::string body;

	std::list<CSmartPtr<CSwitch> > swList = CServer::getInstance()->getSwitchMgr()->getSwitchList();

	StringBuffer strBuff;
	Writer<StringBuffer> writer(strBuff);

	writer.StartObject();
	writer.Key("switch list");
	writer.StartArray();

	STL_FOR_LOOP(swList, iter)
	{
		INT1 strIP[24] = {0};
		number2ip(htonl(iter->getPtr()->getSwIp()), strIP);

		writer.StartObject();

		writer.Key("ip");
		writer.String(strIP);
		writer.Key("dpid");
		writer.Int64(iter->getPtr()->getDpid());

		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();

	body.append(strBuff.GetString());

	response->setResponse(request->getVersion(), bnc::restful::STATUS_OK, body);
}

void api_hosts(CRestRequest* request, CRestResponse* response)
{
	std::string body;
	std::list<CHost*> hostList = CHostMgr::getInstance()->getHostList();

	StringBuffer strBuff;
	Writer<StringBuffer> writer(strBuff);

	writer.StartObject();
	writer.Key("host list");
	writer.StartArray();

	STL_FOR_LOOP(hostList, iter)
	{
		INT1 strIP[24] = {0};
		number2ip((*iter)->getIp(), strIP);

		writer.StartObject();

		writer.Key("ip");
		writer.String(strIP);
		writer.Key("dpid");
		writer.Int64((*iter)->getDpid());
		writer.Key("inport");
		writer.Int((*iter)->getPortNo());

		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();

	body.append(strBuff.GetString());

	response->setResponse(request->getVersion(), bnc::restful::STATUS_OK, body);
}

void api_get_switch_info(CRestRequest* request, CRestResponse* response)
{
	std::string body;
	INT1 json_tmp[1024] = {0};

	std::list<CSmartPtr<CSwitch> > swList = CServer::getInstance()->getSwitchMgr()->getSwitchList();

	StringBuffer strBuff;
	Writer<StringBuffer> writer(strBuff);

	writer.StartObject();
	writer.Key("switchInfo");
	writer.StartArray();

	STL_FOR_LOOP(swList, iter)
	{
		writer.StartObject();

		CSwitch* sw = iter->getPtr();

		writer.Key("DPID");
		{
			UINT1 dpid[8];
			sprintf(json_tmp, "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", dpid[0],
							   dpid[1], dpid[2], dpid[3], dpid[4], dpid[5], dpid[6],
							   dpid[7]);
			uint8ToStr(sw->getDpid(), dpid);
		}
		writer.String(json_tmp);

		writer.Key("inetAddr");
		number2ip(ntohl(sw->getSwIp()), json_tmp);
		sprintf(json_tmp, "%s:%d", json_tmp,
		                    ntohs(sw->getSwPort()));
		writer.String(json_tmp);

		writer.Key("mfrDesc");
		writer.String(sw->getSwDesc().mfr_desc);

		writer.Key("hwDesc");
		writer.String(sw->getSwDesc().hw_desc);

		writer.Key("swDesc");
		writer.String(sw->getSwDesc().sw_desc);

		writer.Key("serialNum");
		writer.String(sw->getSwDesc().serial_num);

		writer.Key("dpDesc");
		writer.String(sw->getSwDesc().dp_desc);

		writer.Key("buffers");
		writer.Int(sw->getNBuffers());

		writer.Key("ports");
		writer.StartArray();
		{
			sw->lockPorts();
			std::vector<gn_port_t> portList = sw->getPorts();
			UINT4 i_port;
			for (i_port = 0; i_port <= portList.size(); ++i_port)
			{
				gn_port_t *sw_port = &(portList[i_port]);
				if (i_port == portList.size())
				{
					sw_port = &sw->getLoPort();
				}

				writer.StartObject();

				writer.Key("name");
				writer.String(sw_port->name);

				writer.Key("type");
				writer.Int(sw_port->type);

				writer.Key("state");
				writer.Int(sw_port->state);

				writer.Key("hwAddr");
				sprintf(json_tmp, "%02x:%02x:%02x:%02x:%02x:%02x",
							sw_port->hw_addr[0], sw_port->hw_addr[1],
							sw_port->hw_addr[2], sw_port->hw_addr[3],
							sw_port->hw_addr[4], sw_port->hw_addr[5]);
				writer.String(json_tmp);

				writer.Key("portNo");
				sprintf(json_tmp, "%x", sw_port->port_no);
				writer.String(json_tmp);

				writer.Key("config");
				writer.Int(sw_port->config);

				writer.Key("currentFeatures");
				writer.Int(sw_port->curr);

				writer.Key("advertisedFeatures");
				writer.Int(sw_port->advertised);

				writer.Key("supportedFeatures");
				writer.Int(sw_port->supported);

				writer.Key("peerFeatures");
				writer.Int(sw_port->peer);

				writer.EndObject();
			}
			sw->unlockPorts();
		}

		writer.EndArray();

		writer.Key("openflow");
		if (sw->getOfVersion() == 0x01)
		{
			 writer.String("of1.0");
		}
		else
		{
			writer.String("of1.3");
		}

		writer.Key("connectedSince");
		// sprintf(json_tmp, "%llu", sw->connected_since);
		writer.Int(0);

		writer.EndObject();
	}

	writer.EndArray();

	writer.Key("retCode");
	writer.Int(0);

	writer.Key("retMsg");
	writer.String("OK");

	writer.EndObject();

	body.append(strBuff.GetString());

	response->setResponse(request->getVersion(), bnc::restful::STATUS_OK, body);
}
