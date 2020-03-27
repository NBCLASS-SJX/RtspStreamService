// =====================================================================================
//  Copyright (C) 2020 by Jiaxing Shao. All rights reserved
//
//  文 件 名:  sdp_codec.h
//  作    者:  Jiaxing Shao, 13315567369@163.com
//  版 本 号:  1.0
//  创建时间:  2020年03月25日
//  Compiler:  g++
//  描    述:  
// =====================================================================================

#ifndef _SDP_CODEC_H_H_H
#define _SDP_CODEC_H_H_H

#include <string>

struct sdp_origin {
	char *username;
	long long int sess_id;
	long long int sess_version;
	char *nettype;
	char *addrtype;
	char *addr;
};

struct sdp_connection {
	char *nettype;
	char *addrtype;
	char *address;
};

struct sdp_bandwidth {
    char *bwtype;
    char *bandwidth;
};

struct sdp_repeat{
	time_t interval;
	time_t duration;
	time_t *offsets;
	int offsets_count;
};

struct sdp_time {
	time_t starttime;
	time_t stoptime;
	struct sdp_repeat *repeat;
	int repeat_count;
};

struct sdp_timezone_adjustments {
	time_t adjust;
	time_t offset;
};

struct sdp_mediainfo{
	char *type;
	int port;
	int port_n;
	char *proto;
	int *fmt;
	int fmt_count;
};

struct encryption_key{
	char *method;
	char *key;
};

struct sdp_media{
	struct sdp_mediainfo info;
	char *title;
	struct sdp_connection conn;
	struct bandwidth *bw;
	int bw_count;
	struct encryption_key encrypt;
	char **attributes;
	int attributes_count;
};

typedef struct {
	unsigned char proto_version;
	struct sdp_origin origin;
	char *session_name;
	char *session_info;
	char *uri;
	char **emails;
	int emails_count;
	char **phones;
	int phones_count;
	struct sdp_connection conn;
	struct sdp_bandwidth *bw;
	int bw_count;
	struct sdp_time *times;
	int times_count;
	struct sdp_timezone_adjustments *timezone_adj;
	int timezone_adj_count;
	struct encryption_key encrypt;
	char **attributes;
	int attributes_count;
	struct sdp_media *medias;
	int medias_count;
}sdp_payload;

struct sdp_payload *sdp_parser(const char *payload);
std::string str_format(const char *fmt, ...);
std::string sdp_format(const struct sdp_payload *sdp);
void sdp_destroy(struct sdp_payload *sdp);

#endif

