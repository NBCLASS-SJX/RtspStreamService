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

int rtsp_parse_cmd_announce(t_rtsp_info *info, char *buffer, int buflen, int &cmd_seq)
{
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
					"Public: OPTIONS, DESCRIBE, ANNOUNCE, SETUP, PLAY\r\n"
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
	char mode[32] = { 0 };
	if(true){
		sprintf(mode, "play");
	}else{
		sprintf(mode, "record");
	}

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
					"CSeq: %d\r\n"
					"\r\n", cmd_seq);
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

