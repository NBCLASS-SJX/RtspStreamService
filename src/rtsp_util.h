// =====================================================================================
//	Copyright (C) 2019 by Jiaxing Shao.All rights reserved.
//	文 件 名:  rtsp_util.h
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年04月18日 19时55分39秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#ifndef _RTSP_UTIL_H_H_H
#define _RTSP_UTIL_H_H_H

#include "common/kgr_utils.h"
#include "common/kgr_logger.h"
#include "encrypt/kgr_md5.h"
#include "rtsp_struct.h"
using namespace std;

#define MAX_REVBUF_SIZE (1024 * 1024 * 16)

// rtsp util
int send_rtsp_message(int sockfd, char *buffer, int buflen);
int recv_rtsp_message(int sockfd, char *buffer, int buflen);
// md5加密
string get_md5_response(t_rtsp_info *info, std::string type, std::string url);
// 打印信息
void print_rtsp_info(t_rtsp_info *info);

// 获取RTSP client对象
t_rtsp_info *create_rtsp_clnt_info(const char*url);
// 返回对象
t_rtsp_info *create_rtsp_serv_info(t_rtsp_info *info, const char *itf_url, const char *localip);
t_rtsp_info *create_rtsp_serv_info(t_rtsp_info *info, const char *itf_url, const char *localip, const char *port);
t_rtsp_info *create_rtsp_serv_info(t_rtsp_info *info, const char *itf_url, const char *localip, const char *username, const char *password);
t_rtsp_info *create_rtsp_serv_info(t_rtsp_info *info, const char *itf_url, const char *localip, const char *port, 
		const char *username, const char *password);
// 释放内存
void free_rtsp_info(t_rtsp_info *&info);

// 连接到服务器
int connect_server(char *ipaddr, int port);
// 开启端口
int create_server_socket(int port);

#endif

