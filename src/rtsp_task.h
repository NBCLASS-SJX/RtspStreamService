// =====================================================================================
//	Copyright (C) 2019 by Jiaxing Shao.All rights reserved.
//	文 件 名:  rtsp_task.h
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年07月05日 04时59分27秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#ifndef _RTSP_TASK_H_H_H
#define _RTSP_TASK_H_H_H

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <errno.h>

#include "functions.h"
#include "rtsp_util.h"
#include "rtsp_struct.h"
#include "rtsp_client.h"
#include "rtsp_server.h"

// rtsp请求
bool rtsp_request(t_rtsp_info *info, int sockfd);
// rtsp应答
bool rtsp_response(t_rtsp_info *info, int sockfd);

#endif

