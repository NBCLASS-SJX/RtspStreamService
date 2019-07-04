// =====================================================================================
//	Copyright (C) 2019 by Jiaxing Shao.All rights reserved.
//	文 件 名:  rtsp_util.cpp
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年04月18日 19时55分32秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#include "functions.h"
#include "rtsp_util.h"
#include "md5.h"
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>

int send_rtsp_message(int sockfd, char *buffer, int buflen)
{
	for(int i = 0; i < 3; i++) {
		int len = send(sockfd, buffer, buflen, 0);
		if(len > 0) {
			break;
		} else {
			if((errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN)) {
				return -1;
			}
		}
	}
	return buflen;
}

int recv_rtsp_message(int sockfd, char *buffer, int buflen)
{
	int length = 0;
	for(int i = 0; i < 3; i++) {
		int len = recv(sockfd, buffer, buflen - length, 0);
		if(len > 0) {
			length += len;
			if(buffer[length - 2] == '\r' && buffer[length - 1] == '\n') {
				break;
			}
		} else {
			if((errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN)) {
				return -1;
			}
		}
	}
	return length;
}

string get_md5_response(t_rtsp_info *info, string cmd, string url)
{
	MD5Encrypt md5;
	char *src_part = (char*)malloc(1024);

	memset(src_part, 0, 1024);
	sprintf(src_part, "%s:%s:%s", info->username, info->realm, info->password);
	string rsp1 = md5.MD5_Encrypt((unsigned char*)src_part, strlen(src_part));

	memset(src_part, 0, 1024);
	sprintf(src_part, "%s:%s", cmd.c_str(), url.c_str());
	string rsp2 = md5.MD5_Encrypt((unsigned char*)src_part, strlen(src_part));

	memset(src_part, 0, 1024);
	sprintf(src_part, "%s:%s:%s", rsp1.c_str(), info->nonce, rsp2.c_str());
	string rsp3 = md5.MD5_Encrypt((unsigned char*)src_part, strlen(src_part));
	free(src_part);

	return rsp3;
}

void print_rtsp_info(t_rtsp_info *info)
{
	log_debug("*************************** rtsp_info ********************************");
	log_debug("RTSP URL : %s", info->rtsp_url);
	log_debug("USERNAME : %s", info->username);
	log_debug("PASSWORD : %s", info->password);
	log_debug("IPADDR   : %s", info->ipaddr);
	log_debug("PORT     : %d", info->port);
	log_debug("SESSION  : %s", info->session);
	log_debug("NONCE    : %s", info->nonce);
	log_debug("REALM    : %s", info->realm);
	log_debug("BASE INFO COUNT: %d", info->info_count);
	for(int i = 0; i < info->info_count; i++) {
		log_debug("BASE INFO[%d]   : %s", i, info->base_info[i]);
	}

	log_debug("CHANNEL COUNT      : %d", info->channel_count);
	for(int i = 0; i < info->channel_count; i++) {
		log_debug("CHANNEL %d URL      : %s", i, info->channel_data[i].url);
		log_debug("CHANNEL %d TRANSPORT: %s", i, info->channel_data[i].transport);
		log_debug("CHANNEL %d SSRC     : %s", i, info->channel_data[i].ssrc);
		log_debug("CHANNEL %d COUNT    : %d", i, info->channel_data[i].info_count);
		log_debug("CHANNEL %d RTPTIME  : %d", i, info->channel_data[i].rtptime);
		for(int j = 0; j < info->channel_data[i].info_count; j++) {
			log_debug("CHANNEL %d DATA[%d]  : %s", i, j, info->channel_data[i].info[j]);
		}
	}
	log_debug("****************************** end ***********************************");
}
