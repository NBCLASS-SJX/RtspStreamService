// =====================================================================================
//	Copyright (C) 2019 by Jiaxing Shao.All rights reserved.
//	文 件 名:  rtsp_struct.h
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年04月10日 16时16分03秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#ifndef _RTSP_STRUCT_H_H_H
#define _RTSP_STRUCT_H_H_H


/**********************************************************
* Status code, 摘抄自RFC 7826
* 
/  "100"  ; Continue
/  "200"  ; OK
/  "301"  ; Moved Permanently
/  "302"  ; Found
/  "303"  ; See Other
/  "304"  ; Not Modified
/  "305"  ; Use Proxy
/  "400"  ; Bad Request
/  "401"  ; Unauthorized
/  "402"  ; Payment Required
/  "403"  ; Forbidden
/  "404"  ; Not Found
/  "405"  ; Method Not Allowed
/  "406"  ; Not Acceptable
/  "407"  ; Proxy Authentication Required
/  "408"  ; Request Timeout
/  "410"  ; Gone
/  "412"  ; Precondition Failed
/  "413"  ; Request Message Body Too Large
/  "414"  ; Request-URI Too Long
/  "415"  ; Unsupported Media Type
/  "451"  ; Parameter Not Understood
/  "452"  ; reserved
/  "453"  ; Not Enough Bandwidth
/  "454"  ; Session Not Found
/  "455"  ; Method Not Valid In This State
/  "456"  ; Header Field Not Valid for Resource
/  "457"  ; Invalid Range
/  "458"  ; Parameter Is Read-Only
/  "459"  ; Aggregate Operation Not Allowed
/  "460"  ; Only Aggregate Operation Allowed
/  "461"  ; Unsupported Transport
/  "462"  ; Destination Unreachable
/  "463"  ; Destination Prohibited
/  "464"  ; Data Transport Not Ready Yet
/  "465"  ; Notification Reason Unknown
/  "466"  ; Key Management Error
/  "470"  ; Connection Authorization Required
/  "471"  ; Connection Credentials Not Accepted
/  "472"  ; Failure to Establish Secure Connection
/  "500"  ; Internal Server Error
/  "501"  ; Not Implemented
/  "502"  ; Bad Gateway
/  "503"  ; Service Unavailable
/  "504"  ; Gateway Timeout
/  "505"  ; RTSP Version Not Supported
/  "551"  ; Option Not Supported
/  "553"  ; Proxy Unavailable
**********************************************************/

#define RTSP_BASE_INFO_SIZE (512)
#define SDP_ATTRIBUTE		(8)
#define SDP_DATA_SIZE		(256)
#define IPADDR_SIZE			(16)
#define RTSP_URL_SIZE		(256)

typedef enum {
	ENUM_RTSP_OPTIONS 	= 0,
	ENUM_RTSP_DESCRIBE 	= 1,
	ENUM_RTSP_SETUP 	= 2,
	ENUM_RTSP_PLAY 		= 3,
	ENUM_RTSP_TEARDOWN 	= 4,
	ENUM_RTSP_PAUSE 	= 5,
	ENUM_RTSP_ANNOUNCE 	= 6,
	ENUM_RTSP_RECORD 	= 7
}enum_rtsp_step;

typedef enum {
	ENUM_RTSP_TRANSPORT_TCP = 0,
	ENUM_RTSP_TRANSPORT_UDP = 1
}enum_rtsp_transport_type;

typedef enum {
	ENUM_RTSP_AUTHORIZATION_NONE 	= 0,
	ENUM_RTSP_AUTHORIZATION_BASIC 	= 1,
	ENUM_RTSP_AUTHORIZATION_DIGEST 	= 2
}enum_rtsp_authorization;

typedef enum {
	ENUM_RTSP_STREAM_VIDEO = 0,
	ENUM_RTSP_STREAM_AUDIO = 1
}enum_rtsp_stream_type;

/* 媒体描述信息 */
typedef struct{
	int  stream_type;
	char url[RTSP_URL_SIZE];
	int  chn[2];
	char ssrc[16];
	int  type;
	int  seq;
	int  rtptime;

	char m[SDP_DATA_SIZE];
	char i[SDP_DATA_SIZE];
	char c[SDP_DATA_SIZE];
	char b[SDP_DATA_SIZE];
	char k[SDP_DATA_SIZE];
	char a[SDP_ATTRIBUTE][SDP_DATA_SIZE];
}t_sdp_media;

/* 基础描述信息, RFC-4566 */
typedef struct{
	char v[SDP_DATA_SIZE];			/* 协议版本 */
	char o[SDP_DATA_SIZE];			/* 所有者/创建者和会话标识符 */ 
	char s[SDP_DATA_SIZE];			/* 会话名称 */
	char i[SDP_DATA_SIZE];			/* 会话信息 */
	char u[SDP_DATA_SIZE];			/* URI描述 */
	char e[SDP_DATA_SIZE];			/* Email地址 */
	char p[SDP_DATA_SIZE];			/* 电话号码 */
	char c[SDP_DATA_SIZE];			/*  */
	char b[SDP_DATA_SIZE];
	char t[SDP_DATA_SIZE];			/* 时间描述信息 */
	char r[SDP_DATA_SIZE];
	char z[SDP_DATA_SIZE];
	char k[SDP_DATA_SIZE];
	char a[SDP_ATTRIBUTE][SDP_DATA_SIZE];
}t_sdp_base;

typedef struct{
	t_sdp_base base;
	t_sdp_media	media[4];
	int media_count;
}t_rtsp_sdp;

typedef struct{
	char rtsp_url[RTSP_URL_SIZE];
	int  transport_type;
	bool isAnnounce;
	char username[RTSP_BASE_INFO_SIZE];
	char password[RTSP_BASE_INFO_SIZE];
	char ipaddr[IPADDR_SIZE];
	int  port;
	int  secret;								// 0. 无认证, 1.基本认证 2.摘要认证
	char session[RTSP_BASE_INFO_SIZE];
	char nonce[RTSP_BASE_INFO_SIZE];
	char realm[RTSP_BASE_INFO_SIZE];
	char basic[RTSP_BASE_INFO_SIZE];			// 基本认证

	t_rtsp_sdp sdp;
}t_rtsp_info;

#endif

