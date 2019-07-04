// =====================================================================================
//	Copyright (C) 2019 by Jiaxing Shao.All rights reserved.
//	文 件 名:  rtsp_struct.h
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年04月10日 16时16分03秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#ifndef _RTSP_STRUCT_H_H_H
#define _RTSP_STRUCT_H_H_H

#define MAX_BUF_SIZE (2048)

typedef enum {
	ENUM_RTSP_OPTIONS = 0,
	ENUM_RTSP_DESCRIBE = 1,
	ENUM_RTSP_SETUP = 2,
	ENUM_RTSP_PLAY = 3,
	ENUM_RTSP_TEARDOWN = 4
}enum_rtsp_step;

typedef enum {
	ENUM_RTSP_TRANSPORT_TCP = 0,
	ENUM_RTSP_TRANSPORT_UDP = 1
}enum_rtsp_transport_type;

typedef struct
{
	char url[128];
	char transport[256];
	char ssrc[16];
	char info[16][256];
	int info_count;
	int type;
	int seq;
	int rtptime;
}t_rtsp_channel;

typedef struct
{
	// rtsp地址
	char rtsp_url[128];
	// enum_rtsp_transport_type
	int transtype;

	// 只考虑音视频
	int channel_count;
	int channel_step;
	t_rtsp_channel channel_data[2];

	char username[32];
	char password[32];
	char ipaddr[16];
	int port;

	// 0. 无认证, 1.基本认证 2.摘要认证
	int secret;
	char session[64];
	char nonce[64];
	char realm[32];
	char basic[128];

	// 描述信息条数, base
	int info_count;
	// 描述信息
	char base_info[16][256];
}t_rtsp_info;

#endif

