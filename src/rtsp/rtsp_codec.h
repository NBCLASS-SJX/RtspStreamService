// =====================================================================================
//	Copyright (C) 2019 by Jiaxing Shao.All rights reserved.
//	文 件 名:  rtsp_codec.h
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年07月05日 01时15分28秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#ifndef _RTSP_CODEC_H_H_H
#define _RTSP_CODEC_H_H_H

#define LINE_SIZE 128

#include "rtsp_struct.h"
#include <string.h>
//#include <cstring>


void rtsp_append_header(char *msg, char *fmt, ...);

void rtsp_append_header_cmd(char *msg, enum_rtsp_step cmd, char *url);
void rtsp_append_header_reply(char *msg, int status_code);
void rtsp_append_header_cseq(char *msg, int cmd_seq);
void rtsp_append_header_authorization_basic(char *msg, char *basic);
void rtsp_append_header_authorization_digest(char *msg, const char *user, const char *realm, const char *nonce, const char *url, const char *response);
void rtsp_append_header_session(char *msg, const char *session);
void rtsp_append_header_range(char *msg, double npt_start, double npt_end);
void rtsp_append_header_transport(char *msg, enum_rtsp_transport_type type, int streamOutgoing, int chn_start, int chn_end, const char *ssrc);
void rtsp_append_header_date(string &msg);
void rtsp_append_header_public(string &msg);
void rtsp_append_header_rtpinfo(string &msg, const char *url, unsigned int seq, unsigned int rtptime);
void rtsp_append_content_type(string &msg);
void rtsp_append_content_base(string &msg, const char *url);
void rtsp_append_content_length(string &msg, unsigned int length);

int encode_rtsp_cmd_message(t_rtsp_info *info, enum_rtsp_step step, char *pData, int nSize, int cmd_seq);
int encode_rtsp_cmd_message(t_rtsp_info *info, enum_rtsp_step step, char *pData, int nSize, int cmd_seq, int stream_id);
int encode_rtsp_reply_message(t_rtsp_info *info, char *pData, int nSize, int cmd_seq);

#endif
