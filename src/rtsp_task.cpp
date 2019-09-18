// =====================================================================================
//	Copyright (C) 2019 by Jiaxing Shao.All rights reserved.
//	文 件 名:  rtsp_task.cpp
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年07月05日 04时59分45秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#include "rtsp_task.h"

// RTSP视频流请求封装
bool rtsp_request(t_rtsp_info *info, int sockfd)
{
	char sndbuf[MAX_BUF_SIZE] = { 0 };
	char rcvbuf[MAX_BUF_SIZE] = { 0 };
	int result = 0;
	int length = 0;

	int seq = 0;
	int step = 0;

	while(true) {
		memset(sndbuf, 0, MAX_BUF_SIZE);
		memset(rcvbuf, 0, MAX_BUF_SIZE);

		length = rtsp_cmd(info, sndbuf, step, seq++);
		result = send_rtsp_message(sockfd, sndbuf, length);
		if(result < 0) {
			log_debug("send rtsp command error:%d\n"
					  "send message:\n"
					  "%s", 
					  result, sndbuf);
			close(sockfd);
			return false;
		}

		length = recv_rtsp_message(sockfd, rcvbuf, MAX_BUF_SIZE);
		if(length <= 0) {
			log_debug("recv rtsp response error:%d\n"
					  "send message:\n"
					  "%s", 
					  length, sndbuf);
			close(sockfd);
			return false;
		}

		result = rtsp_parse_reply(info, rcvbuf, length, step);

		if(step == ENUM_RTSP_OPTIONS) {
			if(result == 0) {
				step = ENUM_RTSP_DESCRIBE;
			} else if(result == 1) {
				continue;
			} else if(result == -1) {
				log_debug("parse options error, send message:\n"
						"%s\n"
						"recv message:\n"
						"%s", sndbuf, rcvbuf);
				close(sockfd);
				return false;
			}
		} else if(step == ENUM_RTSP_DESCRIBE) {
			if(result == 0) {
				step = ENUM_RTSP_SETUP;
			} else if(result == 1) {
				continue;
			} else if(result == -1) {
				log_debug("parse describe error, send message:\n"
						"%s\n"
						"recv message:\n"
						"%s", sndbuf, rcvbuf);
				close(sockfd);
				return false;
			}
		} else if(step == ENUM_RTSP_SETUP) {
			if(result == 0) {
				if(info->channel_step != info->channel_count) {
					continue;
				} else {
					step = ENUM_RTSP_PLAY;
				}
			} else if(result == -1) {
				log_debug("parse setup error, send message:\n"
						"%s\n"
						"recv message:\n"
						"%s", sndbuf, rcvbuf);
				close(sockfd);
				return false;
			}
		} else if(step == ENUM_RTSP_PLAY) {
			if(result == 0){
				break;
			} else if(result == -1) {
				log_debug("parse play error, send message:\n"
						"%s\n"
						"recv message:\n"
						"%s", sndbuf, rcvbuf);
				close(sockfd);
				return false;
			}
		}
	}
	return true;
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

