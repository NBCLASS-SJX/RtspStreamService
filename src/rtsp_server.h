// =====================================================================================
//	Copyright (C) 2019 by Jiaxing Shao.All rights reserved.
//	文 件 名:  rtsp_server.h
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年04月10日 10时13分32秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#ifndef RTSP_SERVER_H_H_H
#define RTSP_SERVER_H_H_H

#include "rtsp_util.h"

int rtsp_process_options(t_rtsp_info *info, char *buffer, int &buflen);
int rtsp_process_describe(t_rtsp_info *info, char *buffer, int &buflen);
int rtsp_process_setup(t_rtsp_info *info, char *buffer, int &buflen);
int rtsp_process_play(t_rtsp_info *info, char *buffer, int &buflen);

#endif

