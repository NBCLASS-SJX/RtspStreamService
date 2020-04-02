// =====================================================================================
//	Copyright (C) 2019 by Jiaxing Shao.All rights reserved.
//	文 件 名:  rtsp_codec.cpp
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年07月05日 01时15分36秒
//	Compiler:  g++
//	描    述:  部分注释摘抄自RFC-7826
// =====================================================================================

#include "rtsp_codec.h"

/* **************************************************************
rtsp-header		= general-header
				/ request-header
				/ response-header
				/ message-body-header

general-header  =  Accept-Ranges
				/  Cache-Control
				/  Connection
				/  CSeq
				/  Date
				/  Media-Properties
				/  Media-Range
				/  Pipelined-Requests
				/  Proxy-Supported
				/  Range
				/  RTP-Info
				/  Scale
				/  Seek-Style
				/  Server
				/  Session
				/  Speed
				/  Supported
				/  Timestamp
				/  Transport
				/  User-Agent
				/  Via
				/  extension-header

request-header  =  Accept
				/  Accept-Credentials
				/  Accept-Encoding
				/  Accept-Language
				/  Authorization
				/  Bandwidth
				/  Blocksize
				/  From
				/  If-Match
				/  If-Modified-Since
				/  If-None-Match
				/  Notify-Reason
				/  Proxy-Authorization
				/  Proxy-Require
				/  Referrer
				/  Request-Status
				/  Require
				/  Terminate-Reason
				/  extension-header

response-header  =  Authentication-Info
                 /  Connection-Credentials
				 /  Location
				 /  MTag
				 /  Proxy-Authenticate
				 /  Proxy-Authentication-Info
				 /  Public
				 /  Retry-After
				 /  Unsupported
				 /  WWW-Authenticate
				 /  extension-header

message-body-header    =  Allow
                    /  Content-Base
					/  Content-Encoding
					/  Content-Language
					/  Content-Length
					/  Content-Location
					/  Content-Type
					/  Expires
					/  Last-Modified
					/  extension-header
 * **************************************************************/



void rtsp_append_header(char *msg, char *fmt, ...)
{

}

/* **************************************************************
 * 追加 rtsp 命令头
 * **************************************************************/
void rtsp_append_header_cmd(string &msg, enum_rtsp_step cmd, char *url)
{
	char buf[LINE_SIZE] = { 0 };
	switch(cmd)
	{
	case ENUM_RTSP_OPTIONS:	
		sprintf_r(buf, LINE_SIZE, "OPTIONS %s RTSP/1.0\r\n", url);break;
	case ENUM_RTSP_DESCRIBE:
		sprintf_r(buf, LINE_SIZE, "DESCRIBE %s RTSP/1.0\r\n", url);break;
	case ENUM_RTSP_SETUP:
		sprintf_r(buf, LINE_SIZE, "SETUP %s RTSP/1.0\r\n", url);break;
	case ENUM_RTSP_PLAY:
		sprintf_r(buf, LINE_SIZE, "PLAY %s/ RTSP/1.0\r\n", url);break;
	case ENUM_RTSP_TEARDOWN:
		sprintf_r(buf, LINE_SIZE, "TEARDOWN %s RTSP/1.0\r\n", url);break;
	case ENUM_RTSP_PAUSE:
		sprintf_r(buf, LINE_SIZE, "PAUSE %s RTSP/1.0\r\n", url);break;
	case ENUM_RTSP_ANNOUNCE:
		sprintf_r(buf, LINE_SIZE, "ANNOUNCE %s RTSP/1.0\r\n", url);break;
	case ENUM_RTSP_RECORD:
		sprintf_r(buf, LINE_SIZE, "RECORD %s RTSP/1.0\r\n", url);break;
	}
	msg.appedn(buf);
}

/* **************************************************************
 * 追加 rtsp 回复头
 * **************************************************************/
void rtsp_append_header_reply(string &msg, int status_code)
{
	char buf[LINE_SIZE] = { 0 };
	if(status_code == 200){
		sprintf_r(buf, LINE_SIZE, "REPLY/1.0 %d OK\r\n", status_code);
	}else if(status_code == 404){
		sprintf_r(buf, LINE_SIZE, "REPLY/1.0 %d NOT FOUND\r\n", status_code);
	}else{
		sprintf_r(buf, LINE_SIZE, "REPLY/1.0 %d ERROR\r\n", status_code);
	}
	msg.append(buf);
}

/* **************************************************************
 * 追加 rtsp 序号
 * **************************************************************/
void rtsp_append_header_cseq(string &msg, int cmd_seq)
{
	char buf[LINE_SIZE] = { 0 };
	sprintf_r(buf, LINE_SIZE, "Cseq: %d\r\n", cmd_seq);
	msg.append(buf);
}

/* **************************************************************
 * 追加 rtsp 基本认证信息
 * **************************************************************/
void rtsp_append_header_authorization_basic(string &msg, char *basic)
{
	char buf[LINE_SIZE] = { 0 };
	sprintf_r(buf, LINE_SIZE, "Authorization: Basic %s\r\n", basic);
	msg.append(buf);
}

/* **************************************************************
 * 追加 rtsp 摘要认证信息
 * **************************************************************/
void rtsp_append_header_authorization_digest(string &msg, const char *user, const char *realm, const char *nonce, const char *url, const char *response)
{
	char buf[LINE_SIZE] = { 0 };
	sprintf_r(buf, LINE_SIZE, "Authorization: Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\"\r\n",
			user, realm, nonce, url, response);
	msg.append(buf);
}

/* **************************************************************
 * 追加 rtsp 信息
 * **************************************************************/
void rtsp_append_header_session(string &msg, const char *session)
{
	char buf[LINE_SIZE] = { 0 };
	sprintf_r(buf, LINE_SIZE, "Session: %s\r\n", session);
	msg.append(buf);
}

/* **************************************************************
 * 追加 rtsp 信息
 * **************************************************************/
void rtsp_append_header_range(string &msg, const char *npt_start, const char *npt_end)
{
	char buf[LINE_SIZE] = { 0 };
	sprintf_r(buf, LINE_SIZE, "Range: npt=%s-%s\r\n", npt_start, npt_end);
	msg.append(buf);
}

/* **************************************************************
 * 追加 rtsp 数据传输信息
 * **************************************************************/
void rtsp_append_header_transport(string &msg, enum_rtsp_transport_type type, bool streamOutgoing, int chn_start, int chn_end, const char *ssrc)
{
	char buf[LINE_SIZE] = { 0 };
	if(type == ENUM_RTSP_TRANSPORT_UDP){
		if(streamOutgoing){
			sprintf_r(buf, LINE_SIZE, "Transport: RAW/RAW/UDP;client_port=%d-%d;ssrc=%s;mode=\"play\"\r\n", chn_start, chn_end, ssrc);
		}else{
			sprintf_r(buf, LINE_SIZE, "Transport: RAW/RAW/UDP;client_port=%d-%d\r\n", chn_start, chn_end);
		}
	}else{
		if(streamOutgoing){
			sprintf_r(buf, LINE_SIZE, "Transport: RTP/AVP/TCP;unicast;interleaved=%d-%d;ssrc=%s;mode=\"play\"\r\n", chn_start, chn_end, ssrc);
		}else{
			sprintf_r(buf, LINE_SIZE, "Transport: RTP/AVP/TCP;unicast;interleaved=%d-%d\r\n", chn_start, chn_end);
		}
	}
	msg.append(buf);
}

/* **************************************************************
 * 追加 rtsp 日期信息
 * **************************************************************/
void rtsp_append_header_date(string &msg)
{
	char buf[LINE_SIZE] = { 0 };
	time_t timer = time(NULL);
	strftime(buf, LINE_SIZE, "Date: %a, %b %d %Y %H:%M:%S GMT\r\n", gmtime(&timer));
	msg.append(buf);
}

/* **************************************************************
 * 追加 rtsp 命令信息
 * **************************************************************/
void rtsp_append_header_public(string &msg)
{
	char buf[LINE_SIZE] = { 0 };
	sprintf_r(buf, LINE_SIZE, "Public: OPTIONS, DESCRIBE, ANNOUNCE, SETUP, PLAY, TEARDOWN,RECORD\r\n");
	msg.append(buf);
}

/* **************************************************************
 * 追加 rtsp RTP信息
 * **************************************************************/
void rtsp_append_content_base(string &msg, const char *url)
{
	char buf[LINE_SIZE] = { 0 };
	sprintf_r(buf, LINE_SIZE, "Content-Base: %s\r\n", url);
	msg.append(buf);
}

/* **************************************************************
 * 追加 rtsp 信息
 * **************************************************************/
void rtsp_append_content_type(string &msg)
{
	char buf[LINE_SIZE] = { 0 };
	sprintf_r(buf, LINE_SIZE, "Content-Type: application/sdp\r\n");
	msg.append(buf);
}

/* **************************************************************
 * 追加 rtsp content长度信息
 * **************************************************************/
void rtsp_append_content_length(string &msg, unsigned int length)
{
	char buf[LINE_SIZE] = { 0 };
	sprintf_r(buf, LINE_SIZE, "Content-Length: %d\r\n", length);
	msg.append(buf);
}

/* **************************************************************
 * 获取 rtsp 视频信息
 * **************************************************************/
string get_sdp_message()
{
	string content;
	return content;
}

int encode_rtsp_cmd_message(t_rtsp_info *info, enum_rtsp_step step, char *pData, int nSize, int cmd_seq)
{
	return encode_rtsp_cmd_message(info, step, pData, nSize, cmd_seq, -1);
}

int encode_rtsp_cmd_message(t_rtsp_info *info, enum_rtsp_step step, char *pData, int nSize, int cmd_seq, int stream_id)
{
	string msg;
	rtsp_append_header_cmd(msg, step, info->rtsp_url);
	rtsp_append_header_cmd(msg, cmd_seq);

	if(step == ENUM_RTSP_OPTIONS){
		if(info->secret == ENUM_RTSP_AUTHORIZATION_BASIC)
			rtsp_append_header_authorization_basic(msg, info->basic);
	}else if(step == ENUM_RTSP_DESCRIBE){
		if(info->secret == ENUM_RTSP_AUTHORIZATION_BASIC){
			rtsp_append_header_authorization_basic(msg, info->basic);
		}else if(info->secret == ENUM_RTSP_AUTHORIZATION_DIGEST){
			string rtsp_url = info->rtsp_url;
			string response = get_md5_response(info, "DESCRIBE", rtsp_url);
			rtsp_append_header_authorization_digest(msg, info->username, info->realm, info->nonce, info->rtsp_url, response.c_str());
		}
	}else if(step == ENUM_RTSP_SETUP){
		if(strlen(info->session) != 0)
			rtsp_append_header_session(msg, info->session);

		if(info->transport_type == -1 || stream_id >= info->transport_type)
			return -1;

		rtsp_append_header_transport(msg, info->transport_type, info->isAnnounce, info->sdp.media[stream_id].chn[0], 
				info->sdp.media[stream_id].chn[1], NULL);

		if(info->secret == ENUM_RTSP_AUTHORIZATION_BASIC){
			rtsp_append_header_authorization_basic(msg, info->basic);
		}else if(info->secret == ENUM_RTSP_AUTHORIZATION_DIGEST){
			string url = info->sdp.media[stream_id].url;
			string response = get_md5_response(info, "SETUP", url);
			rtsp_append_header_authorization_digest(msg, info->username, info->realm, info->nonce, info->rtsp_url, response.c_str());
		}
	}else if(step == ENUM_RTSP_PLAY){
		if(strlen(info->session) != 0)
			rtsp_append_header_session(msg, info->session);
		rtsp_append_header_range(msg, info->session, "0.000", "");

		if(info->secret == ENUM_RTSP_AUTHORIZATION_BASIC){
		}else if(info->secret == ENUM_RTSP_AUTHORIZATION_DIGEST){
			string rtsp_url = info->rtsp_url;
			string response = get_md5_response(info, "PLAY", rtsp_url + "/");
			rtsp_append_header_authorization_digest(msg, info->username, info->realm, info->nonce, info->rtsp_url, response.c_str());
		}
	}else if(step == ENUM_RTSP_TEARDOWN){
	}else if(step == ENUM_RTSP_PAUSE){
	}else if(step == ENUM_RTSP_ANNOUNCE){
	}else if(step == ENUM_RTSP_RECORD){
	}else{
		return -1;
	}

	msg.append("\r\n");
	if(nSize < msg.length())
		return -1;

	memcpy(buf, msg.c_str(), msg.length());
	return 0;
}

int decode_rtsp_cmd_message(t_rtsp_info *info, enum_rtsp_step step, char *pData, int nSize, int &cmd_seq)
{
}

int encode_rtsp_reply_message(t_rtsp_info *info, enum_rtsp_step step, char *pData, int nSize, int cmd_seq)
{
	string msg;
	rtsp_append_header_reply(msg, 200);
	rtsp_append_header_cmd(msg, cmd_seq);

	if(step == ENUM_RTSP_OPTIONS){
		rtsp_append_header_public(msg);
	}else if(step == ENUM_RTSP_DESCRIBE){
		rtsp_append_content_type(msg);
		rtsp_append_content_base(msg, info->rtsp_url);
		string sdp = get_sdp_message();
		rtsp_append_content_length(msg, sdp.length());
		msg.append("\r\n");
		msg.append(sdp.c_str());
	}else if(step == ENUM_RTSP_SETUP){
		rtsp_append_header_session(msg, info->session);
		rtsp_append_header_transport(msg, info->transport_type, info->isAnnounce, info->sdp.media[stream_id].chn[0], 
				info->sdp.media[stream_id].chn[1], NULL, info->sdp.media[stream_id].ssrc);
	}else if(step == ENUM_RTSP_PLAY){
		rtsp_append_header_session(msg, info->session);

		string rtpinfo;
		for(int i = 0; i < info->sdp.media_count; i++){
			char buf[LINE_SIZE] = { 0 };
			sprintf_r(buf, LINE_SIZE, "url=%s;seq=%d;rtptime=%d,", info->sdp.media[i].url, info->sdp.media[i].seq, info->sdp.media[i].rtptime);
			rtpinfo.append(buf);
		}
		msg += "RTP-Info: " + rtpinfo.substr(0, rtpinfo.length() - 1) + "\r\n";
	}else if(step == ENUM_RTSP_TEARDOWN){
	}else if(step == ENUM_RTSP_PAUSE){
	}else if(step == ENUM_RTSP_ANNOUNCE){
	}else if(step == ENUM_RTSP_RECORD){
	}else{
		return -1;
	}

	msg.append("\r\n");
	if(nSize < msg.length())
		return -1;

	memcpy(buf, msg.c_str(), msg.length());
	return 0;
}

