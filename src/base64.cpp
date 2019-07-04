// =====================================================================================
//	Copyright (C) 2019 by Jiaxing Shao.All rights reserved.
//	文 件 名:  base64.cpp
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年06月19日 02时00分01秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#include "base64.h"
#include <iostream>
#include <sstream>
#include <stdio.h>

std::string ToBase64(const char *msg, int len)
{
	std::string ret("");
	if(len > 3 || len <= 0) {
		return ret;
	}

	char tmp[3] = { 0 };
	memcpy(tmp, msg, len);
	char data[5] = { 0 };

	data[0] = BASE64_MAP_ASC[(tmp[0] & 0xFC) >> 2];
	data[1] = BASE64_MAP_ASC[((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xF0) >> 4)];
	data[2] = BASE64_MAP_ASC[((tmp[1] & 0x0F) << 2) + ((tmp[2] & 0xC0) >> 6)];
	data[3] = BASE64_MAP_ASC[tmp[2] & 0x3F];

	if(len == 1) {
		data[2] = '=';
		data[3] = '=';
	} else if (len == 2) {
		data[3] = '=';
	}
	ret = data;
	return ret;
}

int FromBase64(const char *msg)
{
}

std::string Base64Encode(const char *msg, int len)
{
	std::ostringstream oss;
	int count = len / 3;
	int remain = len % 3;
	for(int i = 0; i < count; i++) {
		oss << ToBase64(msg + i * 3, 3);
	}
	oss << ToBase64(msg + len - remain, remain);
	return oss.str();
}

void Base64Decode(const char *base64)
{
}

