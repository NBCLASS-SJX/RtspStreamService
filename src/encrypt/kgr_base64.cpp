// =====================================================================================
//	Copyright (C) 2019 by Jiaxing Shao.All rights reserved.
//
//	文 件 名:  kgr_base64.cpp
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年06月19日 02时00分01秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#include "kgr_base64.h"

int Base64Encode(const char *srcBuf, int srcLen, char *dstBuf, int dstMaxLen)
{
	if (srcLen <= 0 || ((srcLen + 2) / 3) * 4 > dstMaxLen) {
		return -1;
	}
	memset(dstBuf, 0, dstMaxLen);
	int dstLen = 0;
	for (int i = 0; i < srcLen; i += 3) {
		if (i + 1 == srcLen) {
			dstBuf[dstLen++] = BASE64_MAP_ASC[(srcBuf[i] & 0xFC) >> 2];
			dstBuf[dstLen++] = BASE64_MAP_ASC[(srcBuf[i] & 0x03) << 4];
			dstBuf[dstLen++] = '=';
			dstBuf[dstLen++] = '=';
		} else if(i + 2 == srcLen) {
			dstBuf[dstLen++] = BASE64_MAP_ASC[(srcBuf[i] & 0xFC) >> 2];
			dstBuf[dstLen++] = BASE64_MAP_ASC[((srcBuf[i] & 0x03) << 4) | ((srcBuf[i + 1] & 0xF0) >> 4)];
			dstBuf[dstLen++] = BASE64_MAP_ASC[(srcBuf[i + 1] & 0x0F) << 2];
			dstBuf[dstLen++] = '=';
		} else {
			dstBuf[dstLen++] = BASE64_MAP_ASC[(srcBuf[i] & 0xFC) >> 2];
			dstBuf[dstLen++] = BASE64_MAP_ASC[((srcBuf[i] & 0x03) << 4) | ((srcBuf[i + 1] & 0xF0) >> 4)];
			dstBuf[dstLen++] = BASE64_MAP_ASC[((srcBuf[i + 1] & 0x0F) << 2) | ((srcBuf[i + 2] & 0xC0) >> 6)];
			dstBuf[dstLen++] = BASE64_MAP_ASC[srcBuf[i + 2] & 0x3F];
		}
	}
	return dstLen;
}

int Base64Decode(const char *srcBuf, int srcLen, char *dstBuf, int dstMaxLen)
{
	if (srcLen % 4 != 0) {
		return -1;
	}
	memset(dstBuf, 0, dstMaxLen);
	int dstLen = 0;
	for (int i = 0; i < srcLen; i += 4) {
		dstBuf[dstLen] = (BASE64_MAP_DESC[(unsigned char)srcBuf[i]] & 0x3F) << 2;
		dstBuf[dstLen++] |= (BASE64_MAP_DESC[(unsigned char)srcBuf[i + 1]] & 0x30) >> 4;
		
		dstBuf[dstLen] = (BASE64_MAP_DESC[(unsigned char)srcBuf[i + 1]] & 0x0F) << 4;
		dstBuf[dstLen++] |= (BASE64_MAP_DESC[(unsigned char)srcBuf[i + 2]] & 0x3C) >> 2;

		dstBuf[dstLen] = (BASE64_MAP_DESC[(unsigned char)srcBuf[i + 2]] & 0x03) << 6;
		dstBuf[dstLen++] |= (BASE64_MAP_DESC[(unsigned char)srcBuf[i + 3]] & 0x3F);
	}

	for (int i = srcLen - 1; i > 0; i--) {
		if (srcBuf[i] == '=') {
			dstLen -= 1;
		} else {
			break;
		}
	}
	return dstLen;
}
