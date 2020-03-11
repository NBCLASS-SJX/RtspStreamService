// =====================================================================================
//	Copyright (C) 2019 by Jiaxing Shao.All rights reserved.
//	文 件 名:  rtsp_util.cpp
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年04月18日 19时55分32秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#include "rtsp_util.h"
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>

int send_rtsp_message(int sockfd, char *buffer, int buflen)
{
	int nLen = 0;
	for(int i = 0; i < 3; i++) {
		nLen = send(sockfd, buffer, buflen, 0);
		if(nLen > 0) {
			break;
		} else {
			if((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
				continue;
			return nLen;
		}
	}
	return nLen;
}

int recv_rtsp_message(int sockfd, char *buffer, int buflen)
{
	int nLen = 0;
	for(int i = 0; i < 3; i++) {
		int len = recv(sockfd, buffer + nLen, buflen - nLen, 0);
		if(len > 0) {
			nLen += len;
			if(buffer[nLen - 2] == '\r' && buffer[nLen - 1] == '\n')
				break;
		} else {
			if((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
				continue;
			return len;
		}
	}
	return nLen;
}

string get_md5_response(t_rtsp_info *info, string cmd, string url)
{
	char *src_part = (char*)malloc(1024);

	memset(src_part, 0, 1024);
	sprintf(src_part, "%s:%s:%s", info->username, info->realm, info->password);
	string rsp1 = MD5EncryptHex((unsigned char*)src_part, strlen(src_part));

	memset(src_part, 0, 1024);
	sprintf(src_part, "%s:%s", cmd.c_str(), url.c_str());
	string rsp2 = MD5EncryptHex((unsigned char*)src_part, strlen(src_part));

	memset(src_part, 0, 1024);
	sprintf(src_part, "%s:%s:%s", rsp1.c_str(), info->nonce, rsp2.c_str());
	string rsp3 = MD5EncryptHex((unsigned char*)src_part, strlen(src_part));
	free(src_part);

	return rsp3;
}

/* **************************************************************
 * 控制台输出 t_rtsp_info 数据信息
 * **************************************************************/
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

/* **************************************************************
 * url  rtsp链接
 * 解析传入的RTSP链接，返回 t_rtsp_info* 类型的对象
 * **************************************************************/
t_rtsp_info *create_rtsp_clnt_info(const char*url)
{
	if(strncmp(url, "rtsp://", 7) != 0)
		return NULL;

	string strUser;
	string strPwd;
	string strAddr;
	string strPort;

	string str = url;
	int count = 0;
	int pos = str.find("/", 7);
	string strBase = str.substr(7, pos - 7);
	string *args = get_part_string(strBase, "@", count);

	if(count == 1) {
		string *parts = get_part_string(args[0], ":", count);
		if(parts != NULL) {
			strAddr = parts[0];
			if(count == 1)
				strPort = "554";
			else
				strPort = parts[1];
		} else {
			free_part_string(args);
			return NULL;
		}
		free_part_string(parts);
		free_part_string(args);
	} else if(count == 2) {
		string *parts = get_part_string(args[0], ":", count);
		if(parts != NULL && count == 2) {
			strUser = parts[0];
			strPwd = parts[1];
		} else {
			free_part_string(parts);
			free_part_string(args);
			return NULL;
		}
		free_part_string(parts);

		parts = get_part_string(args[1], ":", count);
		if(parts != NULL) {
			strAddr = parts[0];
			if(count == 1)
				strPort = "554";
			else
				strPort = parts[1];
		} else {
			free_part_string(args);
			return NULL;
		}
		free_part_string(parts);
		free_part_string(args);
	} else {
		free_part_string(args);
		return NULL;
	}

	t_rtsp_info *info = (t_rtsp_info*)malloc(sizeof(t_rtsp_info));
	memset(info, 0, sizeof(t_rtsp_info));

	memcpy(info->rtsp_url, url, strlen(url));
	sprintf(info->username, "%s", strUser.c_str());
	sprintf(info->password, "%s",  strPwd.c_str());
	sprintf(info->ipaddr,   "%s", strAddr.c_str());
	info->port = atoi(strPort.c_str());
	info->secret = 0;
	info->transmit_type = ENUM_RTSP_TRANSPORT_TCP;
	info->channel_count = 0;
	info->channel_step = 0;
	return info;
}

/* **************************************************************
 * 由原RTSP信息生成新的虚拟RTSP服务地址
 * **************************************************************/
t_rtsp_info *create_rtsp_serv_info(t_rtsp_info *info, const char *itf_url, const char *localip)
{
	char port[8] = "554";
	t_rtsp_info *serv_info = create_rtsp_serv_info(info, itf_url, localip, port);
	return serv_info;
}

t_rtsp_info *create_rtsp_serv_info(t_rtsp_info *info, const char *itf_url, const char *localip, const char *port)
{
	t_rtsp_info *serv_info = (t_rtsp_info*)malloc(sizeof(t_rtsp_info));
	memset(serv_info, 0, sizeof(t_rtsp_info));
	memcpy(serv_info->ipaddr, localip, strlen(localip));
	serv_info->port = atoi(port);
	sprintf(serv_info->rtsp_url, "rtsp://%s:%d%s", serv_info->ipaddr, serv_info->port, itf_url);
	serv_info->secret = false;
	serv_info->channel_count = info->channel_count;
	for(int i = 0; i < serv_info->channel_count; i++) {
		sprintf(serv_info->channel_data[i].url, "%s/trackID=%d", serv_info->rtsp_url, i + 1);
		serv_info->channel_data[i].type = info->channel_data[i].type;
		memcpy(serv_info->channel_data[i].ssrc, info->channel_data[i].ssrc, 16);
		serv_info->channel_data[i].seq = info->channel_data[i].seq;
		serv_info->channel_data[i].rtptime = info->channel_data[i].rtptime;
		serv_info->channel_data[i].info_count = info->channel_data[i].info_count;
		for(int j = 0; j < serv_info->channel_data[i].info_count; j++) {
			if(strncmp(info->channel_data[i].info[j], "a=control:", 10) == 0) {
				sprintf(serv_info->channel_data[i].info[j], "a=control:%s", serv_info->channel_data[i].url);
			} else {
				memcpy(serv_info->channel_data[i].info[j], info->channel_data[i].info[j], strlen(info->channel_data[i].info[j]));
			}
		}
	}
	memcpy(serv_info->session, info->session, 64);
	memcpy(serv_info->nonce, info->nonce, 64);
	memcpy(serv_info->realm, info->realm, 32);
	serv_info->info_count = info->info_count;
	for(int i = 0; i < serv_info->info_count; i++) {
		if(strncmp(info->base_info[i], "o=-", 3) == 0) {
			string message = info->base_info[i];
			int nCount = 0;
			string * strs = get_part_string(message, " ", nCount);
			ostringstream oss;
			for(int m = 0; m < nCount; m++) {
				oss << strs[m] << " ";
				if(strcmp(strs[m].c_str(), "IP4") == 0) {
					string ip = serv_info->ipaddr;
					oss << ip << " ";
					m++;
				}
			}
			free_part_string(strs);
			sprintf(serv_info->base_info[i], "%s", oss.str().c_str());
		} else if(strncmp(info->base_info[i], "a=control:", 10) == 0) {
			sprintf(serv_info->base_info[i], "a=control:%s/", serv_info->rtsp_url);
		} else {
			memcpy(serv_info->base_info[i], info->base_info[i], strlen(info->base_info[i]));
		}
	}
	return serv_info;
}

t_rtsp_info *create_rtsp_serv_info(t_rtsp_info *info, const char *itf_url, const char *localip, const char *username, const char *password)
{
	char port[8] = "554";
	t_rtsp_info *serv_info = create_rtsp_serv_info(info, itf_url, localip, port, username, password);
	return serv_info;
}

t_rtsp_info *create_rtsp_serv_info(t_rtsp_info *info, const char *itf_url, const char *localip, const char *port, 
		const char *username, const char *password)
{
	t_rtsp_info *serv_info = create_rtsp_serv_info(info, itf_url, localip, port);
	memcpy(serv_info->username, username, 32);
	memcpy(serv_info->password, password, 32);
	serv_info->secret = true;
	return serv_info;
}

void free_rtsp_info(t_rtsp_info *&info)
{
	if(info == NULL)
		return;
	free(info);
}

int connect_server(char *ipaddr, int port)
{
	do{
		int sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(sockfd == -1) {
			log_debug("socket() failed: %s", strerror(errno));
			break;
		}

		int recv_buf_size = MAX_RCVBUF_SIZE;
		if(setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char*)&recv_buf_size, sizeof(int))) {
			log_debug("setsockopt() failed: %s", strerror(errno));
			close(sockfd);
			break;
		}

		struct sockaddr_in sock_addr;
		memset(&sock_addr, 0, sizeof(sock_addr));
		sock_addr.sin_family = AF_INET;
		sock_addr.sin_addr.s_addr = inet_addr(ipaddr);
		sock_addr.sin_port = htons(port);

		// 连接到设备
		if(connect(sockfd, (struct sockaddr*)&sock_addr, sizeof(sockaddr_in)) == -1) {
			log_debug("connect() error: %s", strerror(errno));
			close(sockfd);
			break;
		}
		return sockfd;
	}while(0);
	return -1;
}

int create_server_socket(int port)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sockfd == -1)
		return -1;

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	int opt = 1;
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt)) == -1)
		return -1;

	if(bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		return -1;

	if(listen(sockfd, 5) == -1)
		return -1;

	return sockfd;
}

