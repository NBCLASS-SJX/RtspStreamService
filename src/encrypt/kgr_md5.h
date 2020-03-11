// =====================================================================================
//  Copyright (C) 2020 by Jiaxing Shao. All rights reserved
//
//  文 件 名:  kgr_md5.h
//  作    者:  Jiaxing Shao, 13315567369@163.com
//  版 本 号:  1.0
//  创建时间:  2020年01月14日
//  Compiler:  g++
//  描    述:  
// =====================================================================================


#ifndef _KGR_MD5_ENCRYPT_H_H_H
#define _KGR_MD5_ENCRYPT_H_H_H

#include <iomanip>
#include <string>
#include <cstring>
#include <stdlib.h>

#define S11 7
#define S12 12
#define S13 17
#define S14 22

#define S21 5
#define S22 9
#define S23 14
#define S24 20

#define S31 4
#define S32 11
#define S33 16
#define S34 23

#define S41 6
#define S42 10
#define S43 15
#define S44 21

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

#define FF(a, b, c, d, x, s, ac)          \
{ \
(a) += F((b), (c), (d)) + (x)+(unsigned int)(ac); \
(a) = ROTATE_LEFT((a), (s)); \
(a) += (b); \
}

#define GG(a, b, c, d, x, s, ac)	\
{	\
(a) += G((b), (c), (d)) + (x)+(unsigned int)(ac);	\
(a) = ROTATE_LEFT((a), (s)); (a) += (b);	\
}

#define HH(a, b, c, d, x, s, ac)          \
{ \
(a) += H((b), (c), (d)) + (x)+(unsigned int)(ac); \
(a) = ROTATE_LEFT((a), (s)); \
(a) += (b); \
}

#define II(a, b, c, d, x, s, ac)          \
{ \
(a) += I((b), (c), (d)) + (x)+(unsigned int)(ac); \
(a) = ROTATE_LEFT((a), (s)); \
(a) += (b); \
}

static const char HEXMAP[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

typedef struct {
	unsigned int state[4];
	unsigned int count[2];
	unsigned char buffer[64];
} MD5Context;

static unsigned char PADDING[64] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

bool MD5Encrypt(unsigned char *dst, unsigned char *src, int len);
std::string MD5EncryptHex(unsigned char *srcBuf, int srcLen);

#endif

