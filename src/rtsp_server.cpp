// =====================================================================================
//	Copyright (C) 2019 by Jiaxing Shao.All rights reserved.
//	文 件 名:  rtsp_server.cpp
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年04月10日 10时13分41秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#include "rtsp_server.h"
#include "functions.h"
#include "rtsp_struct.h"
#include "rtsp_util.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <errno.h>

int create_server_socket(int port)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sockfd == -1) {
		return -1;
	}

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	int opt = 1;
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt)) == -1) {
		return -1;
	}

	if(bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
		return -1;
	}

	if(listen(sockfd, 5) == -1) {
		return -1;
	}

	return sockfd;
}

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

void free_rtsp_serv_info(t_rtsp_info *&info)
{
	if(info == NULL)
		return;

	free(info);
}

int rtsp_parse_cmd_options(t_rtsp_info *info, char *buffer, int buflen, int &cmd_seq)
{
	int ret = 0;
	string message = buffer;
	int nLineCount = 0;
	string *strLines = get_part_string(message, "\r\n", nLineCount);
	do{
		if(strLines == NULL || nLineCount <= 0) {
			ret = -1;
			break;
		}

		int nNodeCount = 0;
		string *strNodes = get_part_string(strLines[0], " ", nNodeCount);
		if(strcmp(strNodes[0].c_str(), "OPTIONS") != 0) {
			ret = -1;
			free_part_string(strNodes);
			break;
		}
		free_part_string(strNodes);

		for(int i = 1; i < nLineCount; i++) {
			int nNodeCount = 0;
			string *strNodes = get_part_string(strLines[i], " ", nNodeCount);
			if(strcmp(strNodes[0].c_str(), "CSeq:") == 0)
				cmd_seq = atoi(strNodes[1].c_str());
			free_part_string(strNodes);
		}
	}while(0);
	free_part_string(strLines);
	return ret;
}

int rtsp_parse_cmd_describe(t_rtsp_info *info, char *buffer, int buflen, int &cmd_seq)
{
	int ret = 0;
	string message = buffer;
	int nLineCount = 0;
	string *strLines = get_part_string(message, "\r\n", nLineCount);
	do{
		if(strLines == NULL || nLineCount <= 0) {
			ret = -1;
			break;
		}

		int nNodeCount = 0;
		string *strNodes = get_part_string(strLines[0], " ", nNodeCount);
		if(strcmp(strNodes[0].c_str(), "DESCRIBE") != 0) {
			ret = -1;
			free_part_string(strNodes);
			break;
		}
		free_part_string(strNodes);

		for(int i = 1; i < nLineCount; i++) {
			int nNodeCount = 0;
			string *strNodes = get_part_string(strLines[i], " ", nNodeCount);
			if(strcmp(strNodes[0].c_str(), "CSeq:") == 0) {
				cmd_seq = atoi(strNodes[1].c_str());
			}
			free_part_string(strNodes);
		}
	}while(0);
	free_part_string(strLines);
	return ret;
}

int rtsp_parse_cmd_setup(t_rtsp_info *info, char *buffer, int buflen, int &cmd_seq)
{
	int ret = 0;
	string message = buffer;
	int nLineCount = 0;
	int channel = -1;
	string *strLines = get_part_string(message, "\r\n", nLineCount);
	do {
		if(strLines == NULL || nLineCount <= 0) {
			ret = -1;
			break;
		}

		int nNodeCount = 0;
		string *strNodes = get_part_string(strLines[0], " ", nNodeCount);
		if(strcmp(strNodes[0].c_str(), "SETUP") != 0 || nNodeCount <= 1) {
			ret = -1;
			free_part_string(strNodes);
			break;
		}

		for(int i = 0; i < info->channel_count; i++) {
			if(strcmp(strNodes[1].c_str(), info->channel_data[i].url) == 0) {
				channel = i;
				break;
			}
		}

		if(channel == -1) {
			ret = -1;
			free_part_string(strNodes);
			break;
		}
		free_part_string(strNodes);

		for(int i = 1; i < nLineCount; i++) {
			int nNodeCount = 0;
			string *strNodes = get_part_string(strLines[i], " ", nNodeCount);
			if(strcmp(strNodes[0].c_str(), "CSeq:") == 0)
				cmd_seq = atoi(strNodes[1].c_str());
			else if(strcmp(strNodes[0].c_str(), "Transport:") == 0)
				memcpy(info->channel_data[channel].transport, strNodes[1].c_str(), strNodes[1].length());
			free_part_string(strNodes);
		}
		ret = channel;
	}while(0);
	free_part_string(strLines);
	return ret;
}

int rtsp_parse_cmd_play(t_rtsp_info *info, char *buffer, int buflen, int &cmd_seq)
{
	int ret = 1;
	string message = buffer;
	int nLineCount = 0;
	string *strLines = get_part_string(message, "\r\n", nLineCount);
	do {
		if(strLines == NULL || nLineCount <= 0) {
			ret = -1;
			break;
		}

		int nNodeCount = 0;
		string *strNodes = get_part_string(strLines[0], " ", nNodeCount);
		if(strcmp(strNodes[0].c_str(), "PLAY") != 0) {
			ret = -1;
			free_part_string(strNodes);
			break;
		}
		free_part_string(strNodes);

		for(int i = 1; i < nLineCount; i++) {
			int nNodeCount = 0;
			string *strNodes = get_part_string(strLines[i], " ", nNodeCount);
			if(strcmp(strNodes[0].c_str(), "CSeq:") == 0)
				cmd_seq = atoi(strNodes[1].c_str());
			free_part_string(strNodes);
		}
	}while(0);
	free_part_string(strLines);
	return ret;
}

int rtsp_parse_cmd_teardown(t_rtsp_info *info, char *buffer, int buflen)
{
	int ret = 0;
	string message = buffer;
	int nLineCount = 0;
	string *strLines = get_part_string(message, "\r\n", nLineCount);
	do {
		if(strLines == NULL || nLineCount <= 0) {
			ret = -1;
			break;
		}

		int nNodeCount = 0;
		string *strNodes = get_part_string(strLines[0], " ", nNodeCount);
		if(strcmp(strNodes[0].c_str(), "TEARDOWN") != 0) {
			ret = -1;
			free_part_string(strNodes);
			break;
		}
		free_part_string(strNodes);

		for(int i = 1; i < nLineCount; i++) {
			int nNodeCount = 0;
			string *strNodes = get_part_string(strLines[i], " ", nNodeCount);
			if(strcmp(strNodes[0].c_str(), "CSeq:") == 0)
				ret = atoi(strNodes[1].c_str());
			free_part_string(strNodes);
		}
	}while(0);

	free_part_string(strLines);
	return ret;
}

int rtsp_reply_options(t_rtsp_info *info, char *buffer, int cmd_seq)
{
	sprintf(buffer, "RTSP/1.0 200 OK\r\n"
					"CSeq: %d\r\n"
					"Public: OPTIONS, DESCRIBE, PLAY, PAUSE, SETUP, TEARDOWN\r\n"
					"\r\n", cmd_seq);
	return strlen(buffer);
}

int rtsp_reply_describe(t_rtsp_info *info, char *buffer, int cmd_seq, bool secret)
{
	char context[1536] = { 0 };
	ostringstream oss;

	if(secret == 2) {
	} else {
		for(int i = 0; i < info->info_count; i++) {
			oss << info->base_info[i] << "\r\n";
		}

		for(int i = 0; i < info->channel_count; i++) {
			for(int j = 0; j < info->channel_data[i].info_count; j++) {
				oss << info->channel_data[i].info[j] << "\r\n";
			}
		}
		sprintf(context, "%s\r\n", oss.str().c_str());
	
		sprintf(buffer, "RTSP/1.0 200 OK\r\n"
				"CSeq: %d\r\n"
				"Content-Type: application/sdp\r\n"
				"Content-Base: %s/\r\n"
				"Content-Length: %d\r\n"
				"\r\n"
				"%s", cmd_seq, info->rtsp_url, strlen(context), context);
	}
	return strlen(buffer);
}

int rtsp_reply_setup(t_rtsp_info *info, char *buffer, int cmd_seq, int channel)
{
	sprintf(buffer, "RTSP/1.0 200 OK\r\n"
					"CSeq: %d\r\n"
					"Session:     %s;timeout=60\r\n"
					"Transport: %s;ssrc=%s;mode=\"play\"\r\n"
					"\r\n",
					cmd_seq, info->session, info->channel_data[channel].transport, info->channel_data[channel].ssrc);
	return strlen(buffer);
}

int rtsp_reply_play(t_rtsp_info *info, char *buffer, int cmd_seq)
{
	ostringstream oss;
	for(int i = 0; i < info->channel_count; i++) {
		char msg[256] = { 0 };
		sprintf(msg, "url=%s;seq=%d;rtptime=%d,", info->channel_data[i].url, 
				info->channel_data[i].seq, info->channel_data[i].rtptime);
		oss << msg;
	}
	string rtpinfo = oss.str().substr(0, oss.str().length() - 1);

	sprintf(buffer, "RTSP/1.0 200 OK\r\n"
					"CSeq: %d\r\n"
					"Session:       %s\r\n"
					"RTP-Info: %s\r\n"
					"\r\n",
					cmd_seq, info->session, rtpinfo.c_str());
	return strlen(buffer);
}

int rtsp_reply_teardown(t_rtsp_info *info, char *buffer, int cmd_seq)
{
	sprintf(buffer, "RTSP/1.0 200 OK\r\n"
					"CSeq: 1\r\n"
					"Public: OPTIONS, DESCRIBE, PLAY, PAUSE, SETUP, TEARDOWN\r\n"
					"\r\n");
	return strlen(buffer);
}

int rtsp_process_options(t_rtsp_info *info, char *buffer, int &buflen)
{
	int seq = 0;
	int ret = 0;
	ret = rtsp_parse_cmd_options(info, buffer, buflen, seq);
	if(ret == -1) {
		return ret;
	}
	memset(buffer, 0, buflen);
	buflen = rtsp_reply_options(info, buffer, seq);
	return ret;
}

int rtsp_process_describe(t_rtsp_info *info, char *buffer, int &buflen)
{
	int seq = 0;
	int ret = 0;
	bool secret = false;
	ret = rtsp_parse_cmd_describe(info, buffer, buflen, seq);
	if(ret == -1) {
		return ret;
	}
	memset(buffer, 0, buflen);
	buflen = rtsp_reply_describe(info, buffer, seq, secret);
	return ret;
}

int rtsp_process_setup(t_rtsp_info *info, char *buffer, int &buflen)
{
	int seq = 0;
	int ret = 0;
	ret = rtsp_parse_cmd_setup(info, buffer, buflen, seq);
	if(ret == -1) {
		return ret;
	}
	memset(buffer, 0, buflen);
	buflen = rtsp_reply_setup(info, buffer, seq, ret);
	return ret;
}

int rtsp_process_play(t_rtsp_info *info, char *buffer, int &buflen)
{
	int seq = 0;
	int ret = 0;
	ret = rtsp_parse_cmd_play(info, buffer, buflen, seq);
	memset(buffer, 0, buflen);
	buflen = rtsp_reply_play(info, buffer, seq);
	return ret;
}

bool rtsp_response(t_rtsp_info *info, int sockfd)
{
	char buffer[MAX_BUF_SIZE] = { 0 };
	int length = 0;
	int result = 0;
	int step = ENUM_RTSP_OPTIONS;
	int last_step = ENUM_RTSP_OPTIONS;
	bool conn = false;
	int count = 0;
	
	while(true) {
		memset(buffer, 0, MAX_BUF_SIZE);
		length = recv_rtsp_message(sockfd, buffer, MAX_BUF_SIZE);
		if(step == ENUM_RTSP_OPTIONS) {
			result = rtsp_process_options(info, buffer, length);
			if(result == -1) {
				log_debug("rtep_process_options error, \n%s", buffer);
				return false;
			}
			step++;
		} else if(step == ENUM_RTSP_DESCRIBE) {
			result = rtsp_process_describe(info, buffer, length);
			if(result == -1) {
				log_debug("rtep_process_describe error, \n%s", buffer);
				return false;
			} else if(result == 0) {
				step++;
			}
		} else if(step == ENUM_RTSP_SETUP) {
			result = rtsp_process_setup(info, buffer, length);
			if(result == -1) {
				log_debug("rtep_process_setup error, \n%s", buffer);
				return false;
			}

			count += 1;
			if(info->channel_count == count) {
				step++;
			}
		} else if(step == ENUM_RTSP_PLAY) {
			result = rtsp_process_play(info, buffer, length);
			if(result != -1) {
				conn = true;
			}
		} else {
			log_debug("rtep_process error, unknown message type, \n%s", buffer);
			return false;
		}

		result = send_rtsp_message(sockfd, buffer, length);
		if(conn) {
			break;
		}
	}
	return true;
}

