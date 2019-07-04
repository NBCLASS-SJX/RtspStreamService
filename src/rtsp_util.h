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

#include "rtsp_struct.h"
#include <iostream>
using namespace std;
#include <string.h>

// rtsp util
int send_rtsp_message(int sockfd, char *buffer, int buflen);
int recv_rtsp_message(int sockfd, char *buffer, int buflen);
// md5加密
string get_md5_response(t_rtsp_info *info, string type, string url);
void print_rtsp_info(t_rtsp_info *info);

#endif

