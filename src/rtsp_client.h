// =====================================================================================
//	Copyright (C) 2019 by Jiaxing Shao.All rights reserved.
//	文 件 名:  rtsp_client.h
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年04月02日 18时34分55秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#ifndef _RTSP_CLIENT_H_H_H
#define _RTSP_CLIENT_H_H_H

#include "rtsp_struct.h"

// 连接到服务器
int connect_server(char *ipaddr, int port);
// 获取对象
t_rtsp_info *create_rtsp_clnt_info(const char*url);
// 释放对象
void free_rtsp_clnt_info(t_rtsp_info *&info);
// rtsp视频流请求
int rtsp_cmd(t_rtsp_info *info, char *buffer, int step, int cmd_seq);
int rtsp_parse_reply(t_rtsp_info *info, char *buffer, int buflen, int step);
bool rtsp_request(t_rtsp_info *info, int sockfd);

#endif

