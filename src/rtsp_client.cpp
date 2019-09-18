// =====================================================================================
//	Copyright (C) 2019 by Jiaxing Shao.All rights reserved.
//	文 件 名:  rtsp_client.cpp
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年04月02日 18时35分04秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#include "rtsp_client.h"

// OPTIONS命令组包, 返回消息长度 
int rtsp_cmd_options(t_rtsp_info *info, char *buffer, int cmd_seq)
{
	if(info->secret == 1) {
		sprintf(buffer, "OPTIONS %s RTSP/1.0\r\n"
						"CSeq: %d\r\n"
						"Authorization: Basic %s\r\n"
						"\r\n", 
						info->rtsp_url, cmd_seq, info->basic);
	} else {
		sprintf(buffer, "OPTIONS %s RTSP/1.0\r\n"
						"CSeq: %d\r\n"
						"\r\n", 
						info->rtsp_url, cmd_seq);
	}
	return strlen(buffer);
}

// DESCRIBE命令组包, 返回消息长度
int rtsp_cmd_describe(t_rtsp_info *info, char *buffer, int cmd_seq)
{
	if(info->secret == 1) {
		sprintf(buffer, "DESCRIBE %s RTSP/1.0\r\n"
						"CSeq: %d\r\n"
						"Authorization: Basic %s\r\n"
						"\r\n", 
						info->rtsp_url, cmd_seq, info->basic);
	} else if(info->secret == 2) {
		string response = get_md5_response(info, "DESCRIBE", info->rtsp_url);
		sprintf(buffer, "DESCRIBE %s RTSP/1.0\r\n"
						"CSeq: %d\r\n"
						"Authorization: Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\"\r\n"
						"\r\n", 
						info->rtsp_url, cmd_seq, info->username, info->realm, 
						info->nonce, info->rtsp_url, response.c_str());
	} else {
		sprintf(buffer, "DESCRIBE %s RTSP/1.0\r\n"
						"CSeq: %d\r\n"
						"\r\n", info->rtsp_url, cmd_seq);
	}
	return strlen(buffer);
}

// SETUP命令组包, 返回消息长度
int rtsp_cmd_setup(t_rtsp_info *info, char *buffer, int cmd_seq)
{
	if(info->secret == 1) {
		string url = info->channel_data[info->channel_step].url;
		int id = info->channel_step * 2;
		char interleaved[8] = { 0 };
		sprintf(interleaved, "%d-%d", id, id + 1);
		if(strlen(info->session) != 0){
			sprintf(buffer, "SETUP %s RTSP/1.0\r\n"
							"CSeq: %d\r\n"
							"Authorization: Basic %s\r\n"
							"Transport: RTP/AVP/TCP;unicast;interleaved=%s\r\n"
							"\r\n",
							url.c_str(), cmd_seq, info->basic, interleaved);
		}else{
			sprintf(buffer, "SETUP %s RTSP/1.0\r\n"
							"CSeq: %d\r\n"
							"Authorization: Basic %s\r\n"
							"Transport: RTP/AVP/TCP;unicast;interleaved=%s\r\n"
							"\r\n",
							url.c_str(), cmd_seq, info->basic, interleaved);
		}
	} else if(info->secret == 2) {
		string url = info->channel_data[info->channel_step].url;
		int id = info->channel_step * 2;
		char interleaved[8] = { 0 };
		sprintf(interleaved, "%d-%d", id, id + 1);
		string response = get_md5_response(info, "SETUP", url);
		if(strlen(info->session) != 0){
			sprintf(buffer, "SETUP %s RTSP/1.0\r\n"
							"CSeq: %d\r\n"
							"Transport: RTP/AVP/TCP;unicast;interleaved=%s\r\n"
							"Authorization: Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\"\r\n"
							"\r\n",
							url.c_str(), cmd_seq, interleaved, 
							info->username, info->realm,info->nonce, url.c_str(), response.c_str());
		}else{
			sprintf(buffer, "SETUP %s RTSP/1.0\r\n"
							"CSeq: %d\r\n"
							"Transport: RTP/AVP/TCP;unicast;interleaved=%s\r\n"
							"Authorization: Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\"\r\n"
							"\r\n",
							url.c_str(), cmd_seq, interleaved, 
							info->username, info->realm,info->nonce, url.c_str(), response.c_str());
		}
	} else {
		string url = info->channel_data[info->channel_step].url;
		int id = info->channel_step * 2;
		char interleaved[8] = { 0 };
		sprintf(interleaved, "%d-%d", id, id + 1);
		if(strlen(info->session) != 0){
			sprintf(buffer, "SETUP %s RTSP/1.0\r\n"
							"CSeq: %d\r\n"
							"Transport: RTP/AVP/TCP;unicast;interleaved=%s\r\n"
							"\r\n",
							url.c_str(), cmd_seq, interleaved);
		}else{
			sprintf(buffer, "SETUP %s RTSP/1.0\r\n"
							"CSeq: %d\r\n"
							"Transport: RTP/AVP/TCP;unicast;interleaved=%s\r\n"
							"\r\n",
							url.c_str(), cmd_seq, interleaved);
		}
	}
	return strlen(buffer);
}

// PLAY命令组包, 返回消息长度
int rtsp_cmd_play(t_rtsp_info *info, char *buffer, int cmd_seq)
{
	if(info->secret == 1) {
		sprintf(buffer, "PLAY %s/ RTSP/1.0\r\n"
						"CSeq: %d\r\n"
						"Authorization: Basic %s\r\n"
						"Session: %s\r\n"
						"Range: npt=0.000-\r\n"
						"\r\n",
						info->rtsp_url, cmd_seq, info->basic, info->session);
	} else if(info->secret == 2) {
		string url = info->rtsp_url;
		string response = get_md5_response(info ,"PLAY", url + "/");
		sprintf(buffer, "PLAY %s/ RTSP/1.0\r\n"
						"CSeq: %d\r\n"
						"Session: %s\r\n"
						"Range: npt=0.000-\r\n"
						"Authorization: Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s/\", response=\"%s\"\r\n"
						"\r\n",
						info->rtsp_url, cmd_seq, info->session, info->username, 
						info->realm, info->nonce, info->rtsp_url, response.c_str());
	} else {
		sprintf(buffer, "PLAY %s/ RTSP/1.0\r\n"
						"CSeq: %d\r\n"
						"Session: %s\r\n"
						"Range: npt=0.000-\r\n"
						"\r\n",
						info->rtsp_url, cmd_seq, info->session);
	}
	return strlen(buffer);
}

// TEARDOWN命令组包, 返回消息长度
int rtsp_cmd_teardown(t_rtsp_info *info, char *buffer, int cmd_seq)
{
	if(info->secret == 1) {
		sprintf(buffer, "TEARDOWN %s RTSP/1.0\r\n"
						"CSeq: %d\r\n"
						"Authorization: Basic %s\r\n"
						"\r\n",
						info->rtsp_url, cmd_seq, info->basic);
	} else if (info->secret == 2) {
		sprintf(buffer, "TEARDOWN %s RTSP/1.0\r\n"
						"CSeq: %d\r\n"
						"\r\n",
						info->rtsp_url, cmd_seq);
	} else {
		sprintf(buffer, "TEARDOWN %s RTSP/1.0\r\n"
						"CSeq: %d\r\n"
						"\r\n",
						info->rtsp_url, cmd_seq);
	}
	return strlen(buffer);
}

/* OPTIONS应答解析, 失败返回-1, 成功返回0, 如果有基础认证, 返回1 */
int rtsp_parse_reply_options(t_rtsp_info *info, char *buffer, int buflen)
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
		if(strcmp(strNodes[0].c_str(), "RTSP/1.0") == 0 || strcmp(strNodes[1].c_str(), "RTSP/1.1") == 0) {
			if(strcmp(strNodes[1].c_str(), "401") == 0) {
				info->secret = 1;
				if(strlen(info->username) <= 0 || strlen(info->password) <= 0) {
					ret = -1;
				} else {
					ret = 1;
					char tmp[128] = { 0 };
					sprintf(tmp, "%s:%s", info->username, info->password);
					string basic = Base64Encode(tmp, strlen(tmp));
					strncpy(info->basic, basic.c_str(), basic.length());
				}
			} else if(strcmp(strNodes[1].c_str(), "200") == 0){
				ret = 0;
			} else {
				ret = -1;
			}
		}
		free_part_string(strNodes);
	}while(0);

	free_part_string(strLines);
	return ret;
}

/* DESCRIBE应答解析 失败返回-1, 成功返回0, 如果有摘要认证, 返回1 */
int rtsp_parse_reply_describe(t_rtsp_info *info, char *buffer, int buflen)
{
	int ret = 0;
	string message = buffer;

	int nPartCount = 0;
	string *strParts = get_part_string(message, "\r\n\r\n", nPartCount);
	if(strParts == NULL || nPartCount <= 0) {
		free_part_string(strParts);
		return -1;
	}
		
	int nLineCount = 0;
	string *strLines = get_part_string(strParts[0], "\r\n", nLineCount);
	if(strLines == NULL || nLineCount <= 0) {
		free_part_string(strParts);
		return -1;
	}

	do {
		int nNodeCount = 0;
		string *strNodes = get_part_string(strLines[0], " ", nNodeCount);
		if(strNodes == NULL || nNodeCount <= 0) {
			ret = -1;
			break;
		}

		if(strcmp(strNodes[0].c_str(), "RTSP/1.0") == 0 || strcmp(strNodes[1].c_str(), "RTSP/1.1") == 0) {
			if(strcmp(strNodes[1].c_str(), "200") == 0) {
				ret = 0;
			} else if(strcmp(strNodes[1].c_str(), "401") == 0) {
				info->secret = 2;
				ret = 1;
			} else {
				free_part_string(strNodes);
				ret = -1;
				break;
			}
		}
		free_part_string(strNodes);

		if(ret == 1) {
			for(int i = 1; i < nLineCount; i++) {
				strNodes = get_part_string(strLines[i], " ", nNodeCount);
				if(strcmp(strNodes[0].c_str(), "WWW-Authenticate:") == 0) {
					if(strcmp(strNodes[1].c_str(), "Digest") == 0) {
						for(int j = 2; j < nNodeCount; j++) {
							string_replace(strNodes[j], "\"", "");
							string_replace(strNodes[j], ",", "");
							int count = 0;
							string *strs = get_part_string(strNodes[j], "=", count);
							if(count >= 2) {
								if(strcmp(strs[0].c_str(), "realm") == 0)
									memcpy(info->realm, strs[1].c_str(), strs[1].length());
								else if(strcmp(strs[0].c_str(), "nonce") == 0)
									memcpy(info->nonce, strs[1].c_str(), strs[1].length());
							}
							free_part_string(strs);
						}
					}
				}
				free_part_string(strNodes);
			}
		} else if(ret == 0) {
			free_part_string(strLines);
			strLines = get_part_string(strParts[1], "\r\n", nLineCount);
			for(int i = 0; i < nLineCount; i++) {
				strNodes = get_part_string(strLines[i], " ", nNodeCount);

				// 提取音视频信息
				if(strcmp(strNodes[0].c_str(), "m=video") == 0 || 
						strcmp(strNodes[0].c_str(), "m=audio") == 0) {
					memcpy(info->channel_data[info->channel_count].info[0], strLines[i].c_str(), 
							strLines[i].length()); 
					i++;
					int infoCount = 1;
					for(; i < nLineCount; i++) {
						if(strncmp(strLines[i].c_str(), "m=video", 7) == 0 || 
								strncmp(strLines[i].c_str(), "m=audio", 7) == 0) {
							info->channel_data[info->channel_count].info_count = infoCount;
							i--;
							break;
						} else {
							memcpy(info->channel_data[info->channel_count].info[infoCount],
									strLines[i].c_str(), strLines[i].length()); 
							infoCount++;
							if(strncmp(strLines[i].c_str(), "a=control:", 10) == 0) {
								string aControl = strLines[i].substr(10, strLines[i].length() - 10);
								if(strncmp(aControl.c_str(), info->rtsp_url, strlen(info->rtsp_url)) == 0) {
									sprintf(info->channel_data[info->channel_count].url, "%s", aControl.c_str());
								} else {
									sprintf(info->channel_data[info->channel_count].url, "%s/%s", info->rtsp_url, aControl.c_str());
								}
							}
						}
					}

					if(i == nLineCount)
						info->channel_data[info->channel_count].info_count = infoCount;
					info->channel_count += 1;
				} else {
					for(; i < nLineCount; i++) {
						if(strncmp(strLines[i].c_str(), "m=", 2) == 0) {
							i--;
							break;
						} else {
							memcpy(info->base_info[info->info_count], strLines[i].c_str(),strLines[i].length()); 
							info->info_count++;
						}
					}
				}
				free_part_string(strNodes);
			}
		}
	}while(0);

	free_part_string(strParts);
	free_part_string(strLines);
	return ret;
}

/* SETUP应答解析 失败返回-1, 成功返回0 */
int rtsp_parse_reply_setup(t_rtsp_info *info, char *buffer, int buflen)
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
		if(strcmp(strNodes[0].c_str(), "RTSP/1.0") == 0 || strcmp(strNodes[1].c_str(), "RTSP/1.1") == 0) {
			if(strcmp(strNodes[1].c_str(), "200") != 0)
				ret = -1;
		}
		free_part_string(strNodes);

		for(int i = 1; i < nLineCount; i++) {
			strNodes = get_part_string(strLines[i], " ", nNodeCount);
			if(strcmp(strNodes[0].c_str(), "Session:") == 0) {
				int count = 0;
				string *strs = get_part_string(strNodes[1], ";", count);
				memcpy(info->session, strs[0].c_str(), strs[0].length());
				free_part_string(strs);
			} else if(strcmp(strNodes[0].c_str(), "Transport:") == 0) {
				memcpy(info->channel_data[info->channel_step].transport, strNodes[1].c_str(), strNodes[1].length());
				int count = 0;
				string *strs = get_part_string(strNodes[1], ";", count);
				for(int j = 0; j < count; j++) {
					if(strncmp(strs[j].c_str(), "ssrc=", 5) == 0) {
						string ssrc = strs[j].substr(5, strs[j].length() - 5);
						memcpy(info->channel_data[info->channel_step].ssrc, ssrc.c_str(), ssrc.length());
						break;
					}
				}
				free_part_string(strs);
			}
		}	
	} while(0);
	free_part_string(strLines);
	info->channel_step += 1;
	return ret;
}

/* PLAY应答解析 失败返回-1, 成功返回0 */
int rtsp_parse_reply_play(t_rtsp_info *info, char *buffer, int buflen)
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
		if(strcmp(strNodes[0].c_str(), "RTSP/1.0") == 0 || strcmp(strNodes[1].c_str(), "RTSP/1.1") == 0) {
			if(strcmp(strNodes[1].c_str(), "200") != 0)
				ret = -1;
		}

		free_part_string(strNodes);
		for(int i = 1; i < nLineCount; i++) {
			if(strncmp(strLines[i].c_str(), "RTP-Info:", 9) == 0) {
				string strInfo = strLines[i].substr(9, strLines[i].length() - 9);
				string_trim(strInfo, ' ');

				int nPartCount = 0;
				string *strParts = get_part_string(strInfo, ",", nPartCount);
				for(int j = 0; j < nPartCount; j++) {
					int nArgCount = 0;
					string *args = get_part_string(strParts[j], ";", nArgCount);
					int channel = -1;
					if(strncmp(args[0].c_str(), "url=", 4) == 0) {
						string url = args[0].substr(4, args[0].length() - 4);
						for(int n = 0; n < info->channel_count; n++) {
							char tmpUrl[128] = { 0 };
							if(strncmp(url.c_str(), "trackID=", 8) == 0) {
								sprintf(tmpUrl, "%s/%s", info->rtsp_url, url.c_str());
							} else {
								sprintf(tmpUrl, "%s", url.c_str());
							}

							if(strcmp(tmpUrl, info->channel_data[n].url) == 0) {
								channel = n;
								break;
							}
						}
					}

					if(channel != -1) {
						for(int k = 1; k < nArgCount; k++) {
							if(strncmp(args[k].c_str(), "seq=", 4) == 0) {
								string seq = args[k].substr(4, args[k].length() - 4);
								info->channel_data[channel].seq = atoi(seq.c_str());
							} else if(strncmp(args[k].c_str(), "rtptime=", 8) == 0) {
								string rtptime = args[k].substr(8, args[k].length() - 8);
								info->channel_data[channel].rtptime = atoi(rtptime.c_str());
							}
						}
					}
					free_part_string(args);
				}
				free_part_string(strParts);
			}
		}
	}while(0);
	free_part_string(strLines);
	return ret;
}

/* TEARDOWN应答解析 失败返回-1, 成功返回0 */
int rtsp_parse_reply_teardown(t_rtsp_info *info, char *buffer, int buflen)
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
		if(strcmp(strNodes[0].c_str(), "RTSP/1.0") == 0 || strcmp(strNodes[1].c_str(), "RTSP/1.1") == 0) {
			if(strcmp(strNodes[1].c_str(), "200") != 0)
				ret = -1;
		}
	}while(0);

	free_part_string(strLines);
	return ret;
}

// rtsp命令组包
int rtsp_cmd(t_rtsp_info *info, char *buffer, int step, int cmd_seq)
{
	int ret = 0;
	switch(step) {
		case ENUM_RTSP_OPTIONS:
			ret = rtsp_cmd_options(info, buffer, cmd_seq);
			break;
		case ENUM_RTSP_DESCRIBE:
			ret = rtsp_cmd_describe(info, buffer, cmd_seq);
			break;
		case ENUM_RTSP_SETUP:
			ret = rtsp_cmd_setup(info, buffer, cmd_seq);
			break;
		case ENUM_RTSP_PLAY:
			ret = rtsp_cmd_play(info, buffer, cmd_seq);
			break;
		case ENUM_RTSP_TEARDOWN:
			ret = rtsp_cmd_teardown(info, buffer, cmd_seq);
			break;
		default:
			ret = -1;
			break;
	}
	return ret;
}

// rtsp应答解析
int rtsp_parse_reply(t_rtsp_info *info, char *buffer, int buflen, int step)
{
	int ret = 0;
	switch(step) {
		case ENUM_RTSP_OPTIONS:
			ret = rtsp_parse_reply_options(info, buffer, buflen);
			break;
		case ENUM_RTSP_DESCRIBE:
			ret = rtsp_parse_reply_describe(info, buffer, buflen);
			break;
		case ENUM_RTSP_SETUP:
			ret = rtsp_parse_reply_setup(info, buffer, buflen);
			break;
		case ENUM_RTSP_PLAY:
			ret = rtsp_parse_reply_play(info, buffer, buflen);
			break;
		case ENUM_RTSP_TEARDOWN:
			ret = rtsp_parse_reply_teardown(info, buffer, buflen);
			break;
		default:
			ret = -1;
			break;
	}
	return ret;
}

