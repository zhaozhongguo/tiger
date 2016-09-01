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
*   File Name   : COfMsgUtil.cpp           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-6-22           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#include "COfMsgUtil.h"
#include "comm-util.h"
#include "CBuffer.h"
#include "log.h"
#include "bnc-param.h"

std::map<INT4, CSmartPtr<CBuffer> > COfMsgUtil::g_mapSendBuf;

const UINT8 MASK_SET = 1;

BOOL COfMsgUtil::convertMsgToPacketIn(void* data, packet_in_info_t& packetIn)
{
    struct ofp_header* header = (struct ofp_header*)data;
    if (OFPT13_PACKET_IN == header->type)
    {
        struct ofp13_packet_in* of_pkt = (struct ofp13_packet_in *)data;
        struct ofp_oxm_header* oxm_ptr = (struct ofp_oxm_header*)(of_pkt->match.oxm_fields);

        packetIn.xid = ntohl(of_pkt->header.xid);
        packetIn.buffer_id = ntohl(of_pkt->buffer_id);
        packetIn.inport = ntohl(*(UINT4 *)(oxm_ptr->data));
        packetIn.data_len = ntohs(of_pkt->header.length) - sizeof(struct ofp13_packet_in)
                        - ALIGN_8(htons(of_pkt->match.length)) + sizeof(struct ofpx_match) - 2;

        UINT4 match_len = 0;
        UINT4 pkt_ofs = 0;
        match_len = ALIGN_8(htons(of_pkt->match.length));
        match_len -= sizeof(of_pkt->match);
        pkt_ofs = (sizeof(*of_pkt) + match_len + 2);
        packetIn.data = ((UINT1 *)(of_pkt) + pkt_ofs);

        return TRUE;
    }
    else if (OFPT_PACKET_IN == header->type)
    {
        struct ofp_packet_in* of_pkt = (struct ofp_packet_in*)data;
        packetIn.xid = ntohl(of_pkt->header.xid);
        packetIn.buffer_id = ntohl(of_pkt->buffer_id);
        packetIn.inport = (UINT4)ntohs(of_pkt->in_port);
        packetIn.data_len = ntohs(of_pkt->header.length) - offsetof(struct ofp_packet_in, data);
        packetIn.data = of_pkt->data;

        return TRUE;
    }

    return FALSE;
}


BOOL COfMsgUtil::sendOfpMsg(INT4 fd, UINT1 ofVersion, UINT1 type, UINT2 length, UINT4 xid)
{
    struct ofp_header ofMsg;
    ofMsg.version = ofVersion;
    ofMsg.type = type;
    ofMsg.length = htons(length);
    ofMsg.xid = xid;
    INT4 ret = sendMsgOut(fd, (INT1*)&ofMsg, length);
    if (ret < 0)
    {
        LOG_ERROR("send ofmsg failure");
        return FALSE;
    }

    return TRUE;
}

BOOL COfMsgUtil::sendofp13SetConfig(INT4 fd, UINT4 xid)
{
    struct ofp13_switch_config conf;
    UINT4 length = sizeof(conf);
    conf.header.version = OFP13_VERSION;
    conf.header.type = OFPT13_SET_CONFIG;
    conf.header.length = htons(length);
    conf.header.xid = xid;
    conf.flags = htons(OFPC_FRAG_NORMAL);
    conf.miss_send_len = htons(1500);
    INT4 ret = sendMsgOut(fd, (INT1*)&conf, length);
    if (ret < 0)
    {
        LOG_ERROR("send config msg failure");
        return FALSE;
    }

    return TRUE;
}

BOOL COfMsgUtil::sendofp13MultiPartRequest(INT4 fd, UINT1* mtp_req)
{
    //获取发送缓冲区
    std::map<INT4, CSmartPtr<CBuffer> >::iterator iter = g_mapSendBuf.find(fd);
    if (g_mapSendBuf.end() == iter)
    {
        LOG_ERROR("no send buffer");
        return FALSE;
    }
    CSmartPtr<CBuffer>& buffer = iter->second;
    buffer->getMutex()->lock();
    buffer->clear();

    //准备multipart消息
    stats_req_info_t *stats_req_info = (stats_req_info_t *)mtp_req;
    UINT2 total_len = sizeof(struct ofp_multipart_request);
    struct ofp_multipart_request *ofp_mr = (struct ofp_multipart_request *)buffer->getData();
    ofp_mr->header.version = OFP13_VERSION;
    ofp_mr->header.type = OFPT13_MULTIPART_REQUEST;
    ofp_mr->header.length = htons(total_len);
    ofp_mr->header.xid = stats_req_info->xid;
    ofp_mr->type = htons(stats_req_info->type);
    ofp_mr->flags = htons(stats_req_info->flags);
    switch(stats_req_info->type)
    {
        case OFPMP_FLOW:
        {
            flow_stats_req_data_t *flow_stats_req_data = (flow_stats_req_data_t *)(stats_req_info->data);
            struct ofp13_flow_stats_request *ofp_fsr = (struct ofp13_flow_stats_request *) (ofp_mr->body);
            ofp_fsr->table_id = flow_stats_req_data->table_id;
            ofp_fsr->out_port = htonl(flow_stats_req_data->out_port);
            ofp_fsr->out_group = htonl(flow_stats_req_data->out_group);
            ofp_fsr->cookie = 0x0;
            ofp_fsr->cookie_mask = 0x0;
            ofp_fsr->match.type = htons(OFPMT_OXM);
            ofp_fsr->match.length = htons(4);
            memset(ofp_fsr->pad, 0x0, 3);
            memset(ofp_fsr->pad2, 0x0, 4);

            total_len += sizeof(struct ofp13_flow_stats_request);
            break;
        }
        case OFPMP_PORT_STATS:
        {
            port_stats_req_data_t *port_stats_req_data = (port_stats_req_data_t *)(stats_req_info->data);
            struct ofp13_port_stats_request *ofp_psr = (struct ofp13_port_stats_request *)(ofp_mr->body);
            ofp_psr->port_no = htonl(port_stats_req_data->port_no);
            memset(ofp_psr->pad, 0x0, 4);

            total_len += sizeof(struct ofp13_port_stats_request);
            break;
        }
        default:
        {
             break;
        }
    }
    ofp_mr->header.length = htons(total_len);

    //发送消息
    INT4 ret = sendMsgOut(fd, (INT1*)ofp_mr, total_len);
    if (ret < 0)
    {
        LOG_ERROR("send multipart_request msg failure");
        return FALSE;
    }
    buffer->getMutex()->unlock();

    return TRUE;
}


gn_port_t* COfMsgUtil::of13PortConvertter(UINT1 *of_port, gn_port_t *new_sw_port)
{
    const struct ofp13_port *ofp = (struct ofp13_port *)of_port;
    memset(new_sw_port, 0, sizeof(gn_port_t));

    new_sw_port->port_no = ntohl(ofp->port_no);
    new_sw_port->curr = ntohl(ofp->curr);
    new_sw_port->advertised = ntohl(ofp->advertised);
    new_sw_port->supported = ntohl(ofp->supported);
    new_sw_port->peer = ntohl(ofp->peer);
    new_sw_port->config = ntohl(ofp->config);
    new_sw_port->state = ntohl(ofp->state);
    memcpy(new_sw_port->name, ofp->name, OFP_MAX_PORT_NAME_LEN);
    memcpy(new_sw_port->hw_addr, ofp->hw_addr, OFP_ETH_ALEN);

    return new_sw_port;
}


BOOL COfMsgUtil::of13MsgPacketOut(INT4 fd, UINT1 *pktout_req)
{
    //获取发送缓冲区
    std::map<INT4, CSmartPtr<CBuffer> >::iterator iter = g_mapSendBuf.find(fd);
    if (g_mapSendBuf.end() == iter)
    {
        LOG_ERROR("no send buffer");
        return FALSE;
    }
    CSmartPtr<CBuffer>& buffer = iter->second;
    buffer->getMutex()->lock();
    buffer->clear();

    //准备packetout
    packout_req_info_t *packout_req_info = (packout_req_info_t *)pktout_req;
    UINT2 total_len = sizeof(struct ofp13_packet_out) + sizeof(struct ofp13_action_output);
    if (packout_req_info->buffer_id == OFP_NO_BUFFER)
    {
        total_len += packout_req_info->data_len;
    }
    struct ofp13_packet_out *of13_out = (struct ofp13_packet_out*)buffer->getData();
    of13_out->header.version = OFP13_VERSION;
    of13_out->header.type = OFPT13_PACKET_OUT;
    of13_out->header.length = htons(total_len);
    of13_out->header.xid = packout_req_info->xid;
    of13_out->buffer_id = htonl(packout_req_info->buffer_id);
    of13_out->in_port = htonl(packout_req_info->inport);
    of13_out->actions_len = htons(sizeof(struct ofp13_action_output));

    struct ofp13_action_output *ofp13_act = (struct ofp13_action_output *)of13_out->actions;
    ofp13_act->type = htons(OFPAT13_OUTPUT);
    ofp13_act->len = htons(sizeof(struct ofp13_action_output));
    ofp13_act->port = htonl(packout_req_info->outport);
    ofp13_act->max_len = htons(packout_req_info->max_len);
    memset(ofp13_act->pad, 0x0, 6);

    if (packout_req_info->buffer_id == OFP_NO_BUFFER)
    {
        memcpy(ofp13_act->pad+6, packout_req_info->data, packout_req_info->data_len);
    }

    //发送消息
    INT4 ret = sendMsgOut(fd, (INT1*)of13_out, total_len);
    if (ret < 0)
    {
        LOG_ERROR("send multipart_request msg failure");
        return FALSE;
    }
    buffer->getMutex()->unlock();

    return TRUE;
}


BOOL COfMsgUtil::of10MsgPacketOut(INT4 fd, UINT1 *pktout_req)
{
    //获取发送缓冲区
    std::map<INT4, CSmartPtr<CBuffer> >::iterator iter = g_mapSendBuf.find(fd);
    if (g_mapSendBuf.end() == iter)
    {
        LOG_ERROR("no send buffer");
        return FALSE;
    }
    CSmartPtr<CBuffer>& buffer = iter->second;
    buffer->getMutex()->lock();
    buffer->clear();

    packout_req_info_t *packout_req_info = (packout_req_info_t *)pktout_req;
    UINT2 total_len = sizeof(struct ofp_packet_out) + sizeof(struct ofp_action_output);
    if (packout_req_info->buffer_id == OFP_NO_BUFFER)
    {
        total_len += packout_req_info->data_len;
    }
    struct ofp_packet_out *of10_out = (struct ofp_packet_out*)buffer->getData();
    of10_out->header.version = OFP10_VERSION;
    of10_out->header.type = OFPT_PACKET_OUT;
    of10_out->header.length = htons(total_len);
    of10_out->header.xid = packout_req_info->xid;
    of10_out->buffer_id = htonl(packout_req_info->buffer_id);
    of10_out->in_port = htonl(packout_req_info->inport);
    of10_out->actions_len = htons(sizeof(struct ofp_action_output));

    struct ofp_action_output *ofp10_act = (struct ofp_action_output *)of10_out->actions;
    ofp10_act->type = htons(OFPAT_OUTPUT);
    ofp10_act->len = htons(sizeof(struct ofp_action_output));
    ofp10_act->port = htonl(packout_req_info->outport);
    ofp10_act->max_len = htons(packout_req_info->max_len);
    if (packout_req_info->buffer_id == OFP_NO_BUFFER)
    {
        memcpy(of10_out->data + sizeof(struct ofp_action_output), packout_req_info->data, packout_req_info->data_len);
    }

    //发送消息
    INT4 ret = sendMsgOut(fd, (INT1*)of10_out, total_len);
    if (ret < 0)
    {
        LOG_ERROR("send multipart_request msg failure");
        return FALSE;
    }
    buffer->getMutex()->unlock();
    return TRUE;
}

/*
 * 以下代码从C版本DCFabric中拷贝过来
 * 目前只用于简单测试, 后续会修改
 */
// 增加匹配字段
static UINT2 of13_add_oxm_field(UINT1 *buf, gn_oxm_t *oxm_fields)
{
    UINT2 oxm_len = 0;
    struct ofp_oxm_header *oxm = (struct ofp_oxm_header *)buf;
    size_t oxm_field_sz = 0;
    UINT4 netmask = 0;
    UINT4 netmaskv6[4] = {0};

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IN_PORT))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_IN_PORT << 1;
        oxm->length = OFPXMT_OFB_IN_PORT_SZ;
        *(UINT4 *)(oxm->data) = htonl(oxm_fields->in_port);

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_IN_PORT_SZ;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IN_PHY_PORT))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_IN_PHY_PORT << 1;
        oxm->length = OFPXMT_OFB_IN_PORT_SZ;
        *(UINT4 *)(oxm->data) = htonl(oxm_fields->in_phy_port);

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_IN_PORT_SZ;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_METADATA))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_METADATA << 1;
        oxm->length = 8;
        *(UINT8 *)(oxm->data) = htonll(oxm_fields->metadata);

        if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_METADATA_MASK))
        {
            oxm->length = 16;
            *(UINT8 *)(oxm->data + 8) = htonll(oxm_fields->metadata_mask);
        }


        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm_len += sizeof(struct ofp_oxm_header) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);

    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ETH_DST))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_ETH_DST << 1;
        oxm->length = OFPXMT_OFB_ETH_SZ;
        memcpy((UINT1 *)(oxm->data), oxm_fields->eth_dst, OFP_ETH_ALEN);

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_ETH_SZ;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ETH_SRC))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_ETH_SRC << 1;
        oxm->length = OFPXMT_OFB_ETH_SZ;
        memcpy((UINT1 *)(oxm->data), oxm_fields->eth_src, OFP_ETH_ALEN);

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_ETH_SZ;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ETH_TYPE))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_ETH_TYPE << 1;
        oxm->length = OFPXMT_OFB_ETH_TYPE_SZ;
        *(UINT2 *)(oxm->data) = htons(oxm_fields->eth_type);

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_ETH_TYPE_SZ;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_VLAN_VID))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_VLAN_VID << 1;
        oxm->length = OFPXMT_OFB_VLAN_VID_SZ;
        oxm_fields->vlan_vid = OFPVID_PRESENT | oxm_fields->vlan_vid;
        *(UINT2 *)(oxm->data) = htons(oxm_fields->vlan_vid);

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_VLAN_VID_SZ;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_VLAN_PCP))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_VLAN_PCP << 1;
        oxm->length = OFPXMT_OFB_VLAN_PCP_SZ;
        *(UINT1 *)(oxm->data) = (oxm_fields->vlan_pcp);

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_VLAN_PCP_SZ;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IP_DSCP))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_IP_DSCP << 1;
        oxm->length = OFPXMT_OFB_IP_DSCP_SZ;
        *(UINT1 *)(oxm->data) = oxm_fields->ip_dscp;

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_IP_DSCP_SZ;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IP_ECN))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_IP_ECN << 1;
        oxm->length = OFPXMT_OFB_IP_DSCP_SZ;
        *(UINT1 *)(oxm->data) = oxm_fields->ip_ecn;

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_IP_DSCP_SZ;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IP_PROTO))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_IP_PROTO << 1;
        oxm->length = OFPXMT_OFB_IP_PROTO_SZ;
        *(UINT1 *)(oxm->data) = oxm_fields->ip_proto;

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_IP_PROTO_SZ;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV4_SRC))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);

        if (oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV4_SRC_PREFIX))
        {
            oxm->oxm_field_hm = (OFPXMT_OFB_IPV4_SRC << 1) + 1 ;
            oxm->length = OFPXMT_OFB_IPV4_SZ * 2;
            *(UINT4 *)(oxm->data) = htonl(oxm_fields->ipv4_src);

			netmask = ntohl(cidr_to_subnet_mask(oxm_fields->ipv4_src_prefix));
            *(UINT4 *)(oxm->data + 4) = htonl(netmask);
            oxm_len += OFPXMT_OFB_IPV4_SZ;
        }
        else

        {
            oxm->oxm_field_hm = OFPXMT_OFB_IPV4_SRC << 1;
            oxm->length = OFPXMT_OFB_IPV4_SZ;
            *(UINT4 *)(oxm->data) = htonl(oxm_fields->ipv4_src);
        }

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_IPV4_SZ;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV4_DST))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);

        if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV4_DST_PREFIX))
        {
            oxm->oxm_field_hm = (OFPXMT_OFB_IPV4_DST << 1) + 1 ;
            oxm->length = OFPXMT_OFB_IPV4_SZ * 2;
            *(UINT4 *)(oxm->data) = htonl(oxm_fields->ipv4_dst);

			netmask = ntohl(cidr_to_subnet_mask(oxm_fields->ipv4_dst_prefix));
            *(UINT4 *)(oxm->data + 4) = htonl(netmask);
            oxm_len += OFPXMT_OFB_IPV4_SZ;
        }
        else

        {
            oxm->oxm_field_hm = OFPXMT_OFB_IPV4_DST << 1;
            oxm->length = OFPXMT_OFB_IPV4_SZ;
            *(UINT4 *)(oxm->data) = htonl(oxm_fields->ipv4_dst);
        }

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_IPV4_SZ;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_TCP_SRC))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_TCP_SRC << 1;
        oxm->length = OFPXMT_OFB_L4_PORT_SZ;
        *(UINT2 *)(oxm->data) = htons(oxm_fields->tcp_src);

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_L4_PORT_SZ;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_TCP_DST))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_TCP_DST << 1;
        oxm->length = OFPXMT_OFB_L4_PORT_SZ;
        *(UINT2 *)(oxm->data) = htons(oxm_fields->tcp_dst);

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_L4_PORT_SZ;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_UDP_SRC))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_UDP_SRC << 1;
        oxm->length = OFPXMT_OFB_L4_PORT_SZ;
        *(UINT2 *)(oxm->data) = htons(oxm_fields->udp_src);

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_L4_PORT_SZ;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_UDP_DST))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_UDP_DST << 1;
        oxm->length = OFPXMT_OFB_L4_PORT_SZ;
        *(UINT2 *)(oxm->data) = htons(oxm_fields->udp_dst);

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_L4_PORT_SZ;
    }
    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ICMPV4_TYPE))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_ICMPV4_TYPE << 1;
        oxm->length = 1;
        *(UINT1 *)(oxm->data) = oxm_fields->icmpv4_type;

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + 1;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ICMPV4_CODE))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_ICMPV4_CODE << 1;
        oxm->length = 1;
        *(UINT1 *)(oxm->data) = oxm_fields->icmpv4_code;

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + 1;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ARP_OP))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_ARP_OP << 1;
        oxm->length = 1;
        *(UINT1 *)(oxm->data) = oxm_fields->arp_op;

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + 1;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ARP_SPA))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_ARP_SPA << 1;
        oxm->length = OFPXMT_OFB_IPV4_SZ;
        *(UINT4 *)(oxm->data) = htonl(oxm_fields->arp_spa);

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_IPV4_SZ;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ARP_TPA))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_ARP_TPA << 1;
        oxm->length = OFPXMT_OFB_IPV4_SZ;
        *(UINT4 *)(oxm->data) = htonl(oxm_fields->arp_tpa);

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_IPV4_SZ;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ARP_SHA))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_ARP_SHA << 1;
        oxm->length = OFPXMT_OFB_ETH_SZ;
        memcpy((UINT1 *)(oxm->data), oxm_fields->arp_sha, OFP_ETH_ALEN);

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_ETH_TYPE_SZ;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ARP_THA))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_ARP_THA << 1;
        oxm->length = OFPXMT_OFB_ETH_SZ;
        memcpy((UINT1 *)(oxm->data), oxm_fields->arp_tha, OFP_ETH_ALEN);

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_ETH_TYPE_SZ;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV6_SRC))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);

        if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV6_SRC_PREFIX))
        {
            oxm->oxm_field_hm = (OFPXMT_OFB_IPV6_SRC << 1) + 1 ;
            oxm->length = OFPXMT_OFB_IPV6_SZ + OFPXMT_OFB_IPV6_SZ;
            memcpy((UINT1 *)(oxm->data), oxm_fields->ipv6_src, OFPXMT_OFB_IPV6_SZ);

            switch(oxm_fields->ipv6_src_prefix)
            {
                case 8:
                {
                    netmaskv6[0] = htonl(0xff000000);
                    netmaskv6[1] = 0x0;
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 16:
                {
                    netmaskv6[0] = htonl(0xffff0000);
                    netmaskv6[1] = 0x0;
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 24:
                {
                    netmaskv6[0] = htonl(0xffffff00);
                    netmaskv6[1] = 0x0;
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 32:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = 0x0;
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 40:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xff000000);
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 48:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffff0000);
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 56:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffff00);
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 64:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 72:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xff000000);
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 80:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffff0000);
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 88:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffff00);
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 96:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffffff);
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 104:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffffff);
                    netmaskv6[3] = htonl(0xff000000);
                    break;
                }
                case 112:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffffff);
                    netmaskv6[3] = htonl(0xffff0000);
                    break;
                }
                case 120:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffffff);
                    netmaskv6[3] = htonl(0xffffff00);
                    break;
                }
                case 128:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffffff);
                    netmaskv6[3] = htonl(0xffffffff);
                    break;
                }
                default:break;
            }

            memcpy((UINT1 *)(oxm->data + OFPXMT_OFB_IPV6_SZ), netmaskv6, OFPXMT_OFB_IPV6_SZ);
            oxm_len += OFPXMT_OFB_IPV6_SZ;
        }
        else
        {
            oxm->oxm_field_hm = OFPXMT_OFB_IPV6_SRC << 1;
            oxm->length = OFPXMT_OFB_IPV6_SZ;
            memcpy((UINT1 *)(oxm->data), oxm_fields->ipv6_src, 16);
        }

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_IPV6_SZ;
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV6_DST))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);

        if (oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV6_DST_PREFIX))
        {
            oxm->oxm_field_hm = (OFPXMT_OFB_IPV6_DST << 1) + 1 ;
            oxm->length = OFPXMT_OFB_IPV6_SZ + OFPXMT_OFB_IPV6_SZ;
            memcpy((UINT1 *)(oxm->data), oxm_fields->ipv6_dst, OFPXMT_OFB_IPV6_SZ);

            switch(oxm_fields->ipv6_dst_prefix)
            {
                case 8:
                {
                    netmaskv6[0] = htonl(0xff000000);
                    netmaskv6[1] = 0x0;
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 16:
                {
                    netmaskv6[0] = htonl(0xffff0000);
                    netmaskv6[1] = 0x0;
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 24:
                {
                    netmaskv6[0] = htonl(0xffffff00);
                    netmaskv6[1] = 0x0;
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 32:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = 0x0;
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 40:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xff000000);
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 48:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffff0000);
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 56:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffff00);
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 64:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 72:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xff000000);
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 80:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffff0000);
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 88:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffff00);
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 96:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffffff);
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 104:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffffff);
                    netmaskv6[3] = htonl(0xff000000);
                    break;
                }
                case 112:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffffff);
                    netmaskv6[3] = htonl(0xffff0000);
                    break;
                }
                case 120:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffffff);
                    netmaskv6[3] = htonl(0xffffff00);
                    break;
                }
                case 128:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffffff);
                    netmaskv6[3] = htonl(0xffffffff);
                    break;
                }
            }
            memcpy((UINT1 *)(oxm->data + OFPXMT_OFB_IPV6_SZ), netmaskv6, OFPXMT_OFB_IPV6_SZ);
            oxm_len += OFPXMT_OFB_IPV6_SZ;
        }
        else

        {
            oxm->oxm_field_hm = OFPXMT_OFB_IPV6_DST << 1;
            oxm->length = OFPXMT_OFB_IPV6_SZ;
            memcpy((UINT1 *)(oxm->data), oxm_fields->ipv6_dst, 16);
        }

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + OFPXMT_OFB_IPV6_SZ;
    }
    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_MPLS_LABEL))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_MPLS_LABEL << 1;
        oxm->length = 4;
        *(UINT4 *)(oxm->data) = htonl(oxm_fields->mpls_label);

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + 4;
    }
//    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_MPLS_TC))
//    {
//    }
//    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFP_MPLS_BOS))
//    {
//    }
//    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_PBB_ISID))
//    {
//    }
    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_TUNNEL_ID))
    {
        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_TUNNEL_ID << 1;
        oxm->length = 4;
        *(UINT4 *)(oxm->data) = htonl(oxm_fields->tunnel_id);

        oxm_field_sz += sizeof(*oxm) + oxm->length;
        oxm = (struct ofp_oxm_header *)(buf + oxm_field_sz);
        oxm_len += sizeof(struct ofp_oxm_header) + 4;
    }
//    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV6_EXTHDR))
//    {
//    }

    return oxm_len;
}

// 增加设置字段
static UINT2 of13_add_set_field(UINT1 *buf, gn_oxm_t *oxm_fields)
{
    struct ofp_action_set_field *oa_set = (struct ofp_action_set_field *)buf;
    struct ofp_oxm_header *oxm = NULL;
    size_t oxm_field_sz = 0;
    UINT2 set_field_len = 0;
    UINT4 netmask = 0;
    UINT4 netmaskv6[4] = {0};

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IN_PORT))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_IN_PORT << 1;
        oxm->length = OFPXMT_OFB_IN_PORT_SZ;
        *(UINT4 *)(oxm->data) = htonl(oxm_fields->in_port);

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IN_PHY_PORT))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_IN_PHY_PORT << 1;
        oxm->length = OFPXMT_OFB_IN_PORT_SZ;
        *(UINT4 *)(oxm->data) = htonl(oxm_fields->in_phy_port);

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_METADATA))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_METADATA << 1;
        oxm->length = 8;
        *(UINT8 *)(oxm->data) = htonll(oxm_fields->metadata);

        if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_METADATA_MASK))
        {
            oxm->length = 16;
            *(UINT8 *)(oxm->data + 8) = htonll(oxm_fields->metadata_mask);
        }

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ETH_DST))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_ETH_DST << 1;
        oxm->length = OFPXMT_OFB_ETH_SZ;
        memcpy((UINT1 *)(oxm->data), oxm_fields->eth_dst, OFP_ETH_ALEN);

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ETH_SRC))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_ETH_SRC << 1;
        oxm->length = OFPXMT_OFB_ETH_SZ;
        memcpy((UINT1 *)(oxm->data), oxm_fields->eth_src, OFP_ETH_ALEN);

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ETH_TYPE))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_ETH_TYPE << 1;
        oxm->length = OFPXMT_OFB_ETH_TYPE_SZ;
        *(UINT2 *)(oxm->data) = htons(oxm_fields->eth_type);

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_VLAN_VID))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_VLAN_VID << 1;
        oxm->length = OFPXMT_OFB_VLAN_VID_SZ;
        oxm_fields->vlan_vid = OFPVID_PRESENT | oxm_fields->vlan_vid;
        *(UINT2 *)(oxm->data) = htons(oxm_fields->vlan_vid);

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_VLAN_PCP))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_VLAN_PCP << 1;
        oxm->length = OFPXMT_OFB_VLAN_PCP_SZ;
        *(UINT1 *)(oxm->data) = (oxm_fields->vlan_pcp);

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IP_DSCP))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_IP_DSCP << 1;
        oxm->length = OFPXMT_OFB_IP_DSCP_SZ;
        *(UINT1 *)(oxm->data) = oxm_fields->ip_dscp;

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IP_ECN))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_IP_ECN << 1;
        oxm->length = OFPXMT_OFB_IP_DSCP_SZ;
        *(UINT1 *)(oxm->data) = oxm_fields->ip_ecn;

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IP_PROTO))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_IP_PROTO << 1;
        oxm->length = OFPXMT_OFB_IP_PROTO_SZ;
        *(UINT1 *)(oxm->data) = oxm_fields->ip_proto;

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV4_SRC))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);

        if (oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV4_SRC_PREFIX))
        {
            oxm->oxm_field_hm = (OFPXMT_OFB_IPV4_SRC << 1) + 1 ;
            oxm->length = OFPXMT_OFB_IPV4_SZ * 2;
            *(UINT4 *)(oxm->data) = htonl(oxm_fields->ipv4_src);
            switch(oxm_fields->ipv4_src_prefix)
            {
                case 8:
                {
                    netmask = 0xff000000;
                    break;
                }
                case 16:
                {
                    netmask = 0xffff0000;
                    break;
                }
                case 24:
                {
                    netmask = 0xffffff00;
                    break;
                }
                default:break;
            }

            *(UINT4 *)(oxm->data + 4) = htonl(netmask);
        }
        else
        {
            oxm->oxm_field_hm = OFPXMT_OFB_IPV4_SRC << 1;
            oxm->length = OFPXMT_OFB_IPV4_SZ;
            *(UINT4 *)(oxm->data) = htonl(oxm_fields->ipv4_src);
        }

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV4_DST))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);

        if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV4_DST_PREFIX))
        {
            oxm->oxm_field_hm = (OFPXMT_OFB_IPV4_DST << 1) + 1 ;
            oxm->length = OFPXMT_OFB_IPV4_SZ * 2;
            *(UINT4 *)(oxm->data) = htonl(oxm_fields->ipv4_dst);
            switch(oxm_fields->ipv4_dst_prefix)
            {
                case 8:
                {
                    netmask = 0xff000000;
                    break;
                }
                case 16:
                {
                    netmask = 0xffff0000;
                    break;
                }
                case 24:
                {
                    netmask = 0xffffff00;
                    break;
                }
                default:break;
            }

            *(UINT4 *)(oxm->data + 4) = htonl(netmask);
        }
        else
        {
            oxm->oxm_field_hm = OFPXMT_OFB_IPV4_DST << 1;
            oxm->length = OFPXMT_OFB_IPV4_SZ;
            *(UINT4 *)(oxm->data) = htonl(oxm_fields->ipv4_dst);
        }

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_TCP_SRC))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_TCP_SRC << 1;
        oxm->length = OFPXMT_OFB_L4_PORT_SZ;
        *(UINT2 *)(oxm->data) = htons(oxm_fields->tcp_src);

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_TCP_DST))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_TCP_DST << 1;
        oxm->length = OFPXMT_OFB_L4_PORT_SZ;
        *(UINT2 *)(oxm->data) = htons(oxm_fields->tcp_dst);

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_UDP_SRC))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_UDP_SRC << 1;
        oxm->length = OFPXMT_OFB_L4_PORT_SZ;
        *(UINT2 *)(oxm->data) = htons(oxm_fields->udp_src);

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_UDP_DST))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_UDP_DST << 1;
        oxm->length = OFPXMT_OFB_L4_PORT_SZ;
        *(UINT2 *)(oxm->data) = htons(oxm_fields->udp_dst);

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

//    if(oxm_fields->wildcards & (mask_set << OFPXMT_OFB_SCTP_SRC))
//    {
//
//    }
//
//    if(oxm_fields->wildcards & (mask_set << OFPXMT_OFB_SCTP_DST))
//    {
//
//    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ICMPV4_TYPE))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_ICMPV4_TYPE << 1;
        oxm->length = 1;
        *(UINT1 *)(oxm->data) = oxm_fields->icmpv4_type;

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ICMPV4_CODE))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_ICMPV4_CODE << 1;
        oxm->length = 1;
        *(UINT1 *)(oxm->data) = oxm_fields->icmpv4_code;

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ARP_OP))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_ARP_OP << 1;
        oxm->length = 1;
        *(UINT1 *)(oxm->data) = oxm_fields->arp_op;

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ARP_SPA))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_ARP_SPA << 1;
        oxm->length = OFPXMT_OFB_IPV4_SZ;
        *(UINT4 *)(oxm->data) = htonl(oxm_fields->arp_spa);

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ARP_TPA))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_ARP_TPA << 1;
        oxm->length = OFPXMT_OFB_IPV4_SZ;
        *(UINT4 *)(oxm->data) = htonl(oxm_fields->arp_tpa);

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ARP_SHA))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_ARP_SHA << 1;
        oxm->length = OFPXMT_OFB_ETH_SZ;
        memcpy((UINT1 *)(oxm->data), oxm_fields->arp_sha, OFP_ETH_ALEN);

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ARP_THA))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_ARP_THA << 1;
        oxm->length = OFPXMT_OFB_ETH_SZ;
        memcpy((UINT1 *)(oxm->data), oxm_fields->arp_tha, OFP_ETH_ALEN);

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV6_SRC))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);

        if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV6_SRC_PREFIX))
        {
            oxm->oxm_field_hm = (OFPXMT_OFB_IPV6_SRC << 1) + 1 ;
            oxm->length = OFPXMT_OFB_IPV6_SZ + OFPXMT_OFB_IPV6_SZ;
            memcpy((UINT1 *)(oxm->data), oxm_fields->ipv6_src, OFPXMT_OFB_IPV6_SZ);

            switch(oxm_fields->ipv6_src_prefix)
            {
                case 8:
                {
                    netmaskv6[0] = htonl(0xff000000);
                    netmaskv6[1] = 0x0;
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 16:
                {
                    netmaskv6[0] = htonl(0xffff0000);
                    netmaskv6[1] = 0x0;
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 24:
                {
                    netmaskv6[0] = htonl(0xffffff00);
                    netmaskv6[1] = 0x0;
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 32:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = 0x0;
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 40:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xff000000);
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 48:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffff0000);
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 56:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffff00);
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 64:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 72:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xff000000);
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 80:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffff0000);
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 88:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffff00);
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 96:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffffff);
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 104:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffffff);
                    netmaskv6[3] = htonl(0xff000000);
                    break;
                }
                case 112:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffffff);
                    netmaskv6[3] = htonl(0xffff0000);
                    break;
                }
                case 120:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffffff);
                    netmaskv6[3] = htonl(0xffffff00);
                    break;
                }
                case 128:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffffff);
                    netmaskv6[3] = htonl(0xffffffff);
                    break;
                }
                default:break;
            }

            memcpy((UINT1 *)(oxm->data + OFPXMT_OFB_IPV6_SZ), netmaskv6, OFPXMT_OFB_IPV6_SZ);
        }
        else   //鏃犳帺锟??
        {
            oxm->oxm_field_hm = OFPXMT_OFB_IPV6_SRC << 1;
            oxm->length = OFPXMT_OFB_IPV6_SZ;
            memcpy((UINT1 *)(oxm->data), oxm_fields->ipv6_src, 16);
        }

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }

    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV6_DST))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);

        if (oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV6_DST_PREFIX))
        {
            oxm->oxm_field_hm = (OFPXMT_OFB_IPV6_DST << 1) + 1 ;
            oxm->length = OFPXMT_OFB_IPV6_SZ + OFPXMT_OFB_IPV6_SZ;
            memcpy((UINT1 *)(oxm->data), oxm_fields->ipv6_dst, OFPXMT_OFB_IPV6_SZ);

            switch(oxm_fields->ipv6_dst_prefix)
            {
                case 8:
                {
                    netmaskv6[0] = htonl(0xff000000);
                    netmaskv6[1] = 0x0;
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 16:
                {
                    netmaskv6[0] = htonl(0xffff0000);
                    netmaskv6[1] = 0x0;
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 24:
                {
                    netmaskv6[0] = htonl(0xffffff00);
                    netmaskv6[1] = 0x0;
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 32:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = 0x0;
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 40:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xff000000);
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 48:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffff0000);
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 56:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffff00);
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 64:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = 0x0;
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 72:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xff000000);
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 80:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffff0000);
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 88:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffff00);
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 96:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffffff);
                    netmaskv6[3] = 0x0;
                    break;
                }
                case 104:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffffff);
                    netmaskv6[3] = htonl(0xff000000);
                    break;
                }
                case 112:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffffff);
                    netmaskv6[3] = htonl(0xffff0000);
                    break;
                }
                case 120:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffffff);
                    netmaskv6[3] = htonl(0xffffff00);
                    break;
                }
                case 128:
                {
                    netmaskv6[0] = htonl(0xffffffff);
                    netmaskv6[1] = htonl(0xffffffff);
                    netmaskv6[2] = htonl(0xffffffff);
                    netmaskv6[3] = htonl(0xffffffff);
                    break;
                }
            }
            memcpy((UINT1 *)(oxm->data + OFPXMT_OFB_IPV6_SZ), netmaskv6, OFPXMT_OFB_IPV6_SZ);
        }
        else
        {
            oxm->oxm_field_hm = OFPXMT_OFB_IPV6_DST << 1;
            oxm->length = OFPXMT_OFB_IPV6_SZ;
            memcpy((UINT1 *)(oxm->data), oxm_fields->ipv6_dst, 16);
        }

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }
//    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV6_FLABEL))
//    {
//    }
//    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ICMPV6_TYPE))
//    {
//    }
//    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_ICMPV6_CODE))
//    {
//    }
//    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV6_ND_TARGET))
//    {
//    }
//    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV6_ND_SLL))
//    {
//    }
//    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV6_ND_TLL))
//    {
//    }
    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_MPLS_LABEL))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_MPLS_LABEL << 1;
        oxm->length = 4;
        *(UINT4 *)(oxm->data) = htonl(oxm_fields->mpls_label);

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }
//    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_MPLS_TC))
//    {
//    }
//    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFP_MPLS_BOS))
//    {
//    }
//    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_PBB_ISID))
//    {
//    }
    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_TUNNEL_ID))
    {
        oa_set->type = htons(OFPAT13_SET_FIELD);
        oxm = (struct ofp_oxm_header *)oa_set->field;

        oxm->oxm_class = htons(OFPXMC_OPENFLOW_BASIC);
        oxm->oxm_field_hm = OFPXMT_OFB_TUNNEL_ID << 1;
        oxm->length = 4;
        *(UINT4 *)(oxm->data) = htonl(oxm_fields->tunnel_id);

        oxm_field_sz = ALIGN_8(sizeof(struct ofp_action_set_field) - 4 + sizeof(struct ofp_oxm_header) + oxm->length);
        set_field_len += oxm_field_sz;
        oa_set->len = htons(oxm_field_sz);
        oa_set = (struct ofp_action_set_field *)(buf + set_field_len);
    }
//    if(oxm_fields->mask & (MASK_SET << OFPXMT_OFB_IPV6_EXTHDR))
//    {
//    }

    return set_field_len;
}



UINT2 of13_add_action(UINT1 *buf, gn_action_t *action)
{
    UINT2 action_len = 0;
    UINT1 *act = buf;
    gn_action_t *p_action = action;

    while(p_action)
    {
        switch(p_action->type)
        {
            case OFPAT13_OUTPUT:
            {
                gn_action_output_t *p_action_output = (gn_action_output_t *)p_action;
                struct ofp13_action_output *ofp13_ao = (struct ofp13_action_output *)act;
                ofp13_ao->type = htons(OFPAT13_OUTPUT);
                ofp13_ao->len = htons(16);
                ofp13_ao->port = htonl(p_action_output->port);
                ofp13_ao->max_len = htons(p_action_output->max_len);
                memset(ofp13_ao->pad, 0x0, 6);

                act += 16;
                action_len += 16;
                break;
            }
            case OFPAT13_COPY_TTL_OUT:
            {
                break;
            }
            case OFPAT13_COPY_TTL_IN:
            {
                break;
            }
            case OFPAT13_MPLS_TTL:
            {
                break;
            }
            case OFPAT13_DEC_MPLS_TTL:
            {
                break;
            }
            case OFPAT13_PUSH_VLAN:
            {
//                printf("action push vlan\n");
                struct ofp_action_push *oa_push = (struct ofp_action_push *)act;
                oa_push->type = htons(OFPAT13_PUSH_VLAN);
                oa_push->len = htons(sizeof(struct ofp_action_push));
                oa_push->ethertype = htons(ETH_TYPE_VLAN_8021Q);
                memset(oa_push->pad, 0x0, 2);
                act += 8;
                action_len += 8;
                break;
            }
            case OFPAT13_POP_VLAN:
            {
//                printf("action pop vlan\n");
                struct ofp_action_pop_mpls *oa_pop = (struct ofp_action_pop_mpls *)act;
                oa_pop->type = htons(OFPAT13_POP_VLAN);
                oa_pop->len = htons(sizeof(struct ofp_action_pop_mpls));
                oa_pop->ethertype = htons(ETH_TYPE_VLAN_8021Q);
                memset(oa_pop->pad, 0x0, 2);
                act += 8;
                action_len += 8;
                break;
            }
            case OFPAT13_PUSH_MPLS:
            {
//                printf("action push mpls\n");
                struct ofp_action_push *oa_push = (struct ofp_action_push *)act;
                oa_push->type = htons(OFPAT13_PUSH_VLAN);
                oa_push->len = htons(sizeof(struct ofp_action_push));
                oa_push->ethertype = htons(ETH_TYPE_MPLS_8021Q);
                memset(oa_push->pad, 0x0, 2);
                act += 8;
                action_len += 8;
                break;
            }
            case OFPAT13_POP_MPLS:
            {
//                printf("action pop mpls\n");
                struct ofp_action_pop_mpls *oa_pop = (struct ofp_action_pop_mpls *)act;
                oa_pop->type = htons(OFPAT13_POP_VLAN);
                oa_pop->len = htons(sizeof(struct ofp_action_pop_mpls));
                oa_pop->ethertype = htons(ETH_TYPE_MPLS_8021Q);
                memset(oa_pop->pad, 0x0, 2);
                act += 8;
                action_len += 8;
                break;
            }
            case OFPAT13_SET_QUEUE:
            {
				gn_action_set_queue_t *p_action_queue = (gn_action_set_queue_t*)p_action;
				struct ofp_action_set_queue *ofp_queue = (struct ofp_action_set_queue*)act;
				ofp_queue->type = htons(OFPAT13_SET_QUEUE);
				ofp_queue->len = htons(sizeof(struct ofp_action_set_queue));
				ofp_queue->queue_id = htonl(p_action_queue->queue_id);

				act += 8;
				action_len += 8;

                break;
            }
            case OFPAT13_GROUP:
            {
                gn_action_group_t *p_action_group = (gn_action_group_t *)p_action;
                struct ofp_action_group *ofp13_ag = (struct ofp_action_group *)act;
                ofp13_ag->type     = htons(OFPAT13_GROUP);
                ofp13_ag->len      = htons(sizeof(struct ofp_action_group));;
                ofp13_ag->group_id = htonl(p_action_group->group_id);

                act += 8;
                action_len += 8;
                break;
            }
            case OFPAT13_SET_NW_TTL:
            {
                break;
            }
            case OFPAT13_DEC_NW_TTL:
            {
                break;
            }
            case OFPAT13_SET_FIELD:
            {
                gn_action_set_field_t *p_action_set_field = (gn_action_set_field_t *)p_action;
                UINT2 set_field_len = of13_add_set_field(act, &(p_action_set_field->oxm_fields));

                act += set_field_len;
                action_len += set_field_len;
                break;
            }
            case OFPAT13_PUSH_PBB:
            {
                break;
            }
            case OFPAT13_POP_PBB:
            {
                break;
            }
            case OFPAT13_EXPERIMENTER:
            {
                break;
            }
            default:break;
        }
        p_action = p_action->next;
    }

    return action_len;
}



UINT2 of13_add_instruction(UINT1 *buf, gn_flow_t *flow)
{
    UINT2 instruction_len = 0;
    UINT2 action_len = 0;
    UINT1 *ins = buf;
    gn_instruction_t *p_ins = flow->instructions;

    if(NULL == p_ins)
    {
        struct ofp_instruction_actions *oin = (struct ofp_instruction_actions *)ins;
        oin->type = ntohs(OFPIT_APPLY_ACTIONS);
        oin->pad[0] = 0;
        oin->pad[1] = 0;
        oin->pad[2] = 0;
        oin->pad[3] = 0;

        oin->len = ntohs(sizeof(struct ofp_instruction_actions));

        ins = ins + 8;
        instruction_len = instruction_len + 8;
        return instruction_len;
    }

    while(p_ins)
    {
        if((p_ins->type == OFPIT_APPLY_ACTIONS) || (p_ins->type == OFPIT_WRITE_ACTIONS))
        {
//            printf("Instruct apply/write actions\n");
            gn_instruction_actions_t *p_ins_actions = (gn_instruction_actions_t *)p_ins;
            struct ofp_instruction_actions *oin = (struct ofp_instruction_actions *)ins;
            oin->type = ntohs(p_ins->type);
            oin->pad[0] = 0;
            oin->pad[1] = 0;
            oin->pad[2] = 0;
            oin->pad[3] = 0;

            action_len = of13_add_action((UINT1 *)(oin->actions), p_ins_actions->actions);
            oin->len = ntohs(sizeof(struct ofp_instruction_actions) + ALIGN_8(action_len));
//            printf("action len: %d, instruction len: %d\n", action_len, sizeof(struct ofp_instruction_actions) + action_len);

            ins = ins + 8 + action_len;
            instruction_len = instruction_len + 8 + action_len;
        }
        else if(p_ins->type == OFPIT_CLEAR_ACTIONS)
        {
//            printf("Instruct clear actions\n");
            struct ofp_instruction_actions *oin = (struct ofp_instruction_actions *)ins;
            oin->type = ntohs(OFPIT_CLEAR_ACTIONS);
            oin->len = ntohs(8);
            oin->pad[0] = 0;
            oin->pad[1] = 0;
            oin->pad[2] = 0;
            oin->pad[3] = 0;

            ins += 8;
            instruction_len += 8;
        }
        else if(p_ins->type == OFPIT_GOTO_TABLE)
        {
//            printf("Instruct goto table\n");
            gn_instruction_goto_table_t *p_ins_goto_table = (gn_instruction_goto_table_t *)p_ins;
            struct ofp_instruction_goto_table *oi_table = (struct ofp_instruction_goto_table *)ins;
            oi_table->type = ntohs(OFPIT_GOTO_TABLE);
            oi_table->len  = ntohs(8);
            oi_table->table_id = p_ins_goto_table->table_id;
            memset(oi_table->pad, 0x0, 3);

            ins += 8;
            instruction_len += 8;
        }
        else if(p_ins->type == OFPIT_METER)
        {
//            printf("Instruct meter\n");
            gn_instruction_meter_t *p_ins_meter = (gn_instruction_meter_t *)p_ins;
            struct ofp_instruction_meter *oi_meter = (struct ofp_instruction_meter *)ins;
            oi_meter->type = htons(OFPIT_METER);
            oi_meter->len = htons(8);
            oi_meter->meter_id = htonl(p_ins_meter->meter_id);

            ins += 8;
            instruction_len += 8;
        }

        p_ins = p_ins->next;
    }

    return instruction_len;
}


UINT2 of13_add_match(struct ofpx_match *match, gn_match_t *gn_match)
{
    UINT2 oxm_len = 0;
    UINT2 match_len = 0;

    oxm_len = of13_add_oxm_field(match->oxm_fields, &(gn_match->oxm_fields));
    match_len = sizeof(struct ofpx_match) - 4 + oxm_len;
    return match_len;
}

/*
 * 以上代码是从C版本DCFabric中拷贝过来的
 * 后续会修改, 现在只进行简单测试
 */

BOOL COfMsgUtil::of13MsgFlowMod(INT4 fd, UINT1 *flowmod_req)
{
	//获取发送缓冲区
	std::map<INT4, CSmartPtr<CBuffer> >::iterator iter = g_mapSendBuf.find(fd);
	if (g_mapSendBuf.end() == iter)
	{
		LOG_ERROR("no send buffer");
		return FALSE;
	}

	CSmartPtr<CBuffer>& buffer = iter->second;
	buffer->getMutex()->lock();
	buffer->clear();

	UINT2 match_len = 0;
	UINT2 instruction_len = 0;
	UINT2 total_len = sizeof(struct ofp13_flow_mod);
	// UINT1 *data = init_sendbuff(sw, OFP13_VERSION, OFPT13_FLOW_MOD, tot_len, 0);
	flow_mod_req_info_t *mod_info = (flow_mod_req_info_t *)flowmod_req;

	struct ofp13_flow_mod *ofm = (struct ofp13_flow_mod *)buffer->getData();
	ofm->header.version = OFP13_VERSION;
	ofm->header.type = OFPT13_FLOW_MOD;
	ofm->header.length = htons(total_len);
	ofm->header.xid = mod_info->xid;

	ofm->cookie = htonll(0x0);
	ofm->cookie_mask = htonll(0x0);
	ofm->table_id = mod_info->flow->table_id;
	ofm->command = mod_info->command;
	ofm->idle_timeout = htons(mod_info->flow->idle_timeout);
	ofm->hard_timeout = htons(mod_info->flow->hard_timeout);
	ofm->priority = htons(mod_info->flow->priority);
	ofm->buffer_id = htonl(mod_info->buffer_id);                //Default 0xffffffff
	ofm->out_port = htonl(mod_info->out_port);                  //Default 0xffffffff
	ofm->out_group = htonl(mod_info->out_group);                //Default 0xffffffff
	ofm->flags = htons(mod_info->flags);                        //Default OFPFF13_SEND_FLOW_REM
	ofm->pad[0] = 0x0;
	ofm->pad[1] = 0x0;

	ofm->match.type = htons(mod_info->flow->match.type);        //Default OFPMT_OXM
	match_len = of13_add_match(&ofm->match, &(mod_info->flow->match));
	ofm->match.length = htons(match_len);
	total_len = total_len - sizeof(struct ofpx_match) + ALIGN_8(match_len);

	instruction_len = of13_add_instruction((UINT1*)(ofm)+ total_len, mod_info->flow);
	total_len += ALIGN_8(instruction_len);
	ofm->header.length = htons(total_len);

	 //发送消息
	INT4 ret = sendMsgOut(fd, (INT1*)ofm, total_len);
	if (ret < 0)
	{
		LOG_ERROR("send of13 flow mod msg failure");
		return FALSE;
	}
	buffer->getMutex()->unlock();

	return TRUE;
}

