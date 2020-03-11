// =====================================================================================
//	Copyright (C) 2018 by Jiaxing Shao.All rights reserved.
//
//	文 件 名:  kgr_bytes.h
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2018年06月08日 14时10分10秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#ifndef __BYTEARRAY_H_H_H
#define __BYTEARRAY_H_H_H

#define KGR_BYTES_ARRAY_SIZE (1024*1024)

#include <iostream>
#include <cstring>
#include <string>

typedef struct
{
	unsigned char *buffer;
	int size;
	int total;
	int head;
	int tail;
}KgrByteArray;

KgrByteArray *Kgr_ByteArrayCreate(int size = KGR_BYTES_ARRAY_SIZE);
void Kgr_ByteArrayFree(KgrByteArray* &byte_array);
bool Kgr_ByteArrayPutData(KgrByteArray *byte_array, const char *buf, int len);
bool Kgr_ByteArrayGetData(KgrByteArray *byte_array, char *buf, int len, bool isKeep = false);
void Kgr_ByteArrayReset(KgrByteArray *byte_array);
int Kgr_ByteArrayGetLen(KgrByteArray *byte_array);

#endif

