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
*   File Name   : bnc-param.h           *
*   Author      : bnc zgzhao           *
*   Create Date : 2016-5-25           *
*   Version     : 1.0           *
*   Function    : .           *
*                                                                             *
******************************************************************************/
#ifndef __BNC_PARAM_H
#define __BNC_PARAM_H
#include "bnc-type.h"
#include "CSmartPtr.h"

class CSwitch;

enum OF_MSG_STATE
{
    OF_MSG_STATE_NEW,        //新消息
    OF_MSG_STATE_OLD         //旧消息，已被处理过
};


enum OF_HANDLER_TYPE
{
    OF_HANDLER_PACKETIN_IP_TCP_DSTIP,
    OF_HANDLER_PACKETIN_IP_UDP_DSTIP,
    OF_HANDLER_PACKETIN_IP_ICMP,
    OF_HANDLER_PACKETIN_ARP_DSTIP,
    OF_HANDLER_PACKETIN_LLDP,
    OF_HANDLER_PACKETIN_IPV6,
    OF_HANDLER_PACKETIN_VLAN,
    OF_HANDLER_DEFAULT,

    OF_HANDLER_INVALID
};


enum OF_HANDLER_DEFAULT_TYPE
{
    //openflow 10
    OF_HANDLER_DEFAULT_HELLO,
    OF_HANDLER_DEFAULT_ERROR,
    OF_HANDLER_DEFAULT_ECHO_REQUEST,
    OF_HANDLER_DEFAULT_ECHO_REPLY,
    OF_HANDLER_DEFAULT_VENDOR,
    OF_HANDLER_DEFAULT_FEATURES_REQUEST,
    OF_HANDLER_DEFAULT_FEATURES_REPLY,
    OF_HANDLER_DEFAULT_GET_CONFIG_REQUEST,
    OF_HANDLER_DEFAULT_GET_CONFIG_REPLY,
    OF_HANDLER_DEFAULT_SET_CONFIG,
    OF_HANDLER_DEFAULT_FLOW_REMOVED,
    OF_HANDLER_DEFAULT_PORT_STATUS,
    OF_HANDLER_DEFAULT_PACKET_OUT,
    OF_HANDLER_DEFAULT_FLOW_MOD,
    OF_HANDLER_DEFAULT_PORT_MOD,
    OF_HANDLER_DEFAULT_STATS_REQUEST,
    OF_HANDLER_DEFAULT_STATS_REPLY,
    OF_HANDLER_DEFAULT_BARRIER_REQUEST,
    OF_HANDLER_DEFAULT_BARRIER_REPLY,
    OF_HANDLER_DEFAULT_QUEUE_GET_CONFIG_REQUEST,
    OF_HANDLER_DEFAULT_QUEUE_GET_CONFIG_REPLY,

    //openflow 13
    OF_HANDLER_DEFAULT13_HELLO,
    OF_HANDLER_DEFAULT13_ERROR,
    OF_HANDLER_DEFAULT13_ECHO_REQUEST,
    OF_HANDLER_DEFAULT13_ECHO_REPLY,
    OF_HANDLER_DEFAULT13_EXPERIMENTER,
    OF_HANDLER_DEFAULT13_FEATURES_REQUEST,
    OF_HANDLER_DEFAULT13_FEATURES_REPLY,
    OF_HANDLER_DEFAULT13_GET_CONFIG_REQUEST,
    OF_HANDLER_DEFAULT13_GET_CONFIG_REPLY,
    OF_HANDLER_DEFAULT13_SET_CONFIG,
    OF_HANDLER_DEFAULT13_FLOW_REMOVED,
    OF_HANDLER_DEFAULT13_PORT_STATUS,
    OF_HANDLER_DEFAULT13_PACKET_OUT,
    OF_HANDLER_DEFAULT13_FLOW_MOD,
    OF_HANDLER_DEFAULT13_GROUP_MOD,
    OF_HANDLER_DEFAULT13_PORT_MOD,
    OF_HANDLER_DEFAULT13_TABLE_MOD,
    OF_HANDLER_DEFAULT13_MULTIPART_REQUEST,
    OF_HANDLER_DEFAULT13_MULTIPART_REPLY,
    OF_HANDLER_DEFAULT13_BARRIER_REQUEST,
    OF_HANDLER_DEFAULT13_BARRIER_REPLY,
    OF_HANDLER_DEFAULT13_QUEUE_GET_CONFIG_REQUEST,
    OF_HANDLER_DEFAULT13_QUEUE_GET_CONFIG_REPLY,
    OF_HANDLER_DEFAULT13_ROLE_REQUEST,
    OF_HANDLER_DEFAULT13_ROLE_REPLY,
    OF_HANDLER_DEFAULT13_GET_ASYNC_REQUEST,
    OF_HANDLER_DEFAULT13_GET_ASYNC_REPLY,
    OF_HANDLER_DEFAULT13_SET_ASYNC,
    OF_HANDLER_DEFAULT13_METER_MOD
};


typedef struct packet_in_info
{
    UINT4 xid;
    UINT4 buffer_id;
    UINT4 inport;
    UINT4 data_len;
    UINT1 *data;
} packet_in_info_t;


typedef struct switch_desc {
   INT1 mfr_desc[256];       /* Manufacturer description. */
   INT1 hw_desc[256];        /* Hardware description. */
   INT1 sw_desc[256];        /* Software description. */
   INT1 serial_num[32];      /* Serial number. */
   INT1 dp_desc[256];        /* Human readable description of datapath. */
} switch_desc_t;

//负载状态
typedef enum
{
    LOAD_IDLE = 0,  // 0% - 25%
    LOAD_LIGHT = 1, // 25% - 50
    LOAD_BUSY = 2,  // 50% - 75%
    LOAD_HEAVY = 3, // 75% - 100%
    LOAD_FULL = 4   // 100%
}LOAD_STATUS;

typedef struct port_stats
{
    LOAD_STATUS rx_status;
    LOAD_STATUS tx_status;
    UINT1 pad[2];
    double rx_kbps;        //rxkB/s
    double tx_kbps;        //txkB/s
    double rx_kpps;        //rxpck/s
    double tx_kpps;        //txpck/s
    UINT8 rx_packets;      //Number of received packets.
    UINT8 tx_packets;      //Number of transmitted packets.
    UINT8 rx_bytes;        //Number of received bytes.
    UINT8 tx_bytes;        //Number of transmitted bytes.
    UINT4 max_speed;       //Max port bitrate in kbps
    UINT4 duration_sec;    //
    UINT4 timestamp;       //
}port_stats_t;

struct gn_port;
typedef struct neighbor
{
    CSmartPtr<CSwitch> src_sw;
    CSmartPtr<CSwitch> dst_sw;
    UINT4 src_port_no;
    UINT4 dst_port_no;
    UINT8 weight;
}neighbor_t;

//交换机端口类型
typedef enum
{
    PORT_TYPE_SWITCH,
    PORT_TYPE_HOST,
    PORT_TYPE_NONE,
}SW_PORT_TYPE;

typedef struct gn_port
{
    UINT4 port_no;
    UINT1 hw_addr[6];
    UINT1 pad[2];
    INT1 name[16];
    UINT4 curr;
    UINT4 advertised;
    UINT4 supported;
    UINT4 peer;
    UINT4 config;
    UINT4 state;
    UINT1 type;
    port_stats_t stats;
} gn_port_t;

typedef struct stats_req_info
{
    UINT2 type;
    UINT2 flags;
    UINT4 xid;
    UINT1 *data;
}stats_req_info_t;

typedef struct flow_stats_req_data
{
    UINT4 out_port;
    UINT4 out_group;
    UINT1 table_id;
}flow_stats_req_data_t;

typedef struct port_stats_req_data
{
    UINT4 port_no;
}port_stats_req_data_t;


typedef struct packout_req_info
{
    UINT4 xid;
    UINT4 buffer_id;
    UINT4 inport;
    UINT4 outport;
    UINT2 max_len;
    UINT2 data_len;
    UINT1 *data;
}packout_req_info_t;

typedef struct flow_stats
{
    LOAD_STATUS status;
    UINT4 kbps;
    UINT4 kpps;
    UINT8 byte_count;
    UINT8 packet_count;
    UINT4 max_speed;      //Max port bitrate in kbps
    UINT4 duration_sec;   //port 存活时长
    UINT4 timestamp;      //处理成取样时间间隔的整数倍
}flow_stats_t;



typedef struct gn_oxm
{
    UINT8 mask;
    UINT4 in_port;        /* Switch input port. */
    UINT4 in_phy_port;    /* Switch physical input port. */
    UINT8 metadata;       /* Metadata passed between tables. */
    UINT1 eth_dst[6];     /* Ethernet destination address. */
    UINT1 eth_src[6];     /* Ethernet source address. */
    UINT2 eth_type;       /* Ethernet frame type. */
    UINT2 vlan_vid;       /* VLAN id. */
    UINT1 vlan_pcp;       /* VLAN priority. */
    UINT1 ip_dscp;        /* IP DSCP (6 bits in ToS field). */
    UINT1 ip_ecn;         /* IP ECN (2 bits in ToS field). */
    UINT1 ip_proto;       /* IP protocol. */
    UINT4 ipv4_src;       /* IPv4 source address. */
    UINT4 ipv4_dst;       /* IPv4 destination address. */
    UINT2 tcp_src;        /* TCP source port. */
    UINT2 tcp_dst;        /* TCP destination port. */
    UINT2 udp_src;        /* UDP source port. */
    UINT2 udp_dst;        /* UDP destination port. */
//    UINT2 sctp_src;       /* SCTP source port. */
//    UINT2 sctp_dst;       /* SCTP destination port. */
    UINT1 icmpv4_type;    /* ICMP type. */
    UINT1 icmpv4_code;    /* ICMP code. */
    UINT1 arp_op;         /* ARP opcode. */
    UINT4 arp_spa;        /* ARP source IPv4 address. */
    UINT4 arp_tpa;        /* ARP target IPv4 address. */
    UINT1 arp_sha[6];        /* ARP source hardware address. */
    UINT1 arp_tha[6];        /* ARP target hardware address. */
    UINT1 ipv6_src[16];       /* IPv6 source address. */
    UINT1 ipv6_dst[16];       /* IPv6 destination address. */
//    UINT4 ipv6_flabel;    /* IPv6 Flow Label */
//    UINT1 icmpv6_type;    /* ICMPv6 type. */
//    UINT1 icmpv6_code;    /* ICMPv6 code. */
//    UINT1 ipv6_nd_target; /* Target address for ND. */
//    UINT1 ipv6_nd_sll;    /* Source link-layer for ND. */
//    UINT1 ipv6_nd_tll;    /* Target link-layer for ND. */
    UINT4 mpls_label;     /* MPLS label. */
//    UINT1 mpls_tc;        /* MPLS TC. */
//    UINT1 ofpxmt_ofp_mpls_bos; /* MPLS BoS bit. */
//    UINT1 pbb_isid;       /* PBB I-SID. */
    UINT4 tunnel_id;      /* Logical Port Metadata. */
//    UINT1 ipv6_exthdr;    /* IPv6 Extension Header pseudo-field */
    UINT4 ipv4_src_prefix;
    UINT4 ipv4_dst_prefix;
    UINT4 ipv6_src_prefix;
    UINT4 ipv6_dst_prefix;
    UINT8 metadata_mask;
}gn_oxm_t;


typedef struct gn_match
{
    UINT2 type;
    gn_oxm_t oxm_fields;
}gn_match_t;
typedef struct gn_instruction
{
    struct gn_instruction *next;
    UINT2 type;
}gn_instruction_t;

typedef struct gn_flow
{
    struct gn_flow *prev;
    struct gn_flow *next;
    INT1 uuid[40];
    INT1 creater[16];
    UINT8 create_time;
    UINT1 table_id;
    UINT1 status;
    UINT2 idle_timeout;
    UINT2 hard_timeout;
    UINT2 priority;
    flow_stats_t stats;
    gn_match_t match;
    gn_instruction_t *instructions;
}gn_flow_t;

typedef struct gn_action
{
    struct gn_action *next;
    UINT2 type;
}gn_action_t;

typedef struct gn_action_output
{
    struct gn_action *next;
    UINT2 type;
    UINT4 port;
    UINT2 max_len;
}gn_action_output_t;

typedef struct gn_action_set_queue
{
    struct gn_action *next;
    UINT2 type;
    UINT4 queue_id;
}gn_action_set_queue_t;

typedef struct gn_action_group
{
    struct gn_action *next;
    UINT2 type;
    UINT4 group_id;
}gn_action_group_t;

typedef struct gn_action_mpls_ttl
{
    struct gn_action *next;
    UINT2 type;
    UINT1 mpls_tt;
}gn_action_mpls_ttl_t;

typedef struct gn_action_nw_ttl
{
    struct gn_action *next;
    UINT2 type;
    UINT1 nw_tt;
}gn_action_nw_ttl_t;

typedef struct gn_action_push
{
    struct gn_action *next;
    UINT2 type;
    UINT1 mpls_tt;
}gn_action_push_t;

typedef struct gn_action_pop
{
    struct gn_action *next;
    UINT2 type;
    UINT1 mpls_tt;
}gn_action_pop_t;

typedef struct gn_action_set_field
{
    struct gn_action *next;
    UINT2 type;
    gn_oxm_t oxm_fields;
}gn_action_set_field_t;

typedef struct gn_action_experimenter
{
    struct gn_action *next;
    UINT2 type;
    UINT4 experimenter;
}gn_action_experimenter_t;

typedef struct gn_instruction_goto_table
{
    struct gn_instruction *next;
    UINT2 type;
    UINT1 table_id;
}gn_instruction_goto_table_t;

typedef struct gn_instruction_write_metadata
{
    struct gn_instruction *next;
    UINT2 type;
    UINT8 metadata;
    UINT8 metadata_mask;
}gn_instruction_write_metadata_t;

typedef struct gn_instruction_actions
{
    struct gn_instruction *next;
    UINT2 type;
    gn_action_t *actions;
}gn_instruction_actions_t;

typedef struct gn_instruction_meter
{
    struct gn_instruction *next;
    UINT2 type;
    UINT4 meter_id;
}gn_instruction_meter_t;

typedef struct gn_instruction_experimenter
{
    struct gn_instruction *next;
    UINT2 type;
    UINT2 len;
    UINT4 experimenter;
}gn_instruction_experimenter_t;

typedef struct flow_mod_req_info
{
    UINT4 xid;
    UINT4 buffer_id;
    UINT4 out_port;
    UINT4 out_group;
    UINT1 command;
    UINT2 flags;
    gn_flow_t *flow;
}flow_mod_req_info_t;

typedef struct sw_path_node
{
    CSmartPtr<CSwitch> sw;
    UINT4 port_no;
} sw_path_node_t;

typedef struct sw_path
{
    CSmartPtr<CSwitch> src_sw;
    CSmartPtr<CSwitch> dst_sw;
    std::list<sw_path_node_t*> node_list;
} sw_path_t;

typedef struct sw_path_list
{
    CSmartPtr<CSwitch> dst_sw;
    std::list<sw_path_t*> path_list;
} sw_path_list_t;

#endif
