// =====================================================================================
//	Copyright (C) 2018 by Jiaxing Shao.All rights reserved.
//
//	文 件 名:  kgr_bytes.cpp
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2018年06月08日 14时24分25秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#include "kgr_bytes.h"

KgrByteArray *Kgr_ByteArrayCreate(int size)
{
	KgrByteArray *byte_array = new KgrByteArray;
	byte_array->buffer = new unsigned char[size];
	memset(byte_array->buffer, 0, size);
	byte_array->total = size;
	byte_array->size = 0;
	byte_array->head = 0;
	byte_array->tail = 0;
	return byte_array;
}

void Kgr_ByteArrayFree(KgrByteArray* &byte_array)
{
	if(byte_array == NULL)
		return;

	KgrByteArray *tmp_byte_array = byte_array;
	byte_array = NULL;

	if(tmp_byte_array != NULL) {
		if(tmp_byte_array->buffer != NULL) {
			delete [] tmp_byte_array->buffer;
			tmp_byte_array->buffer = NULL;
		}
		delete tmp_byte_array;
		tmp_byte_array = NULL;
	}
}

bool Kgr_ByteArrayPutData(KgrByteArray *byte_array, const char *buf, int len)
{
	if(byte_array == NULL)
		return false;

	if(byte_array->total - byte_array->size < len) {
		return false;
	}
	
	if(byte_array->total - byte_array->tail >= len) {
		memcpy(byte_array->buffer + byte_array->tail, buf, len);
		byte_array->tail = (byte_array->tail + len) % byte_array->total;
	} else {
		int llen = byte_array->total - byte_array->tail;
		int rlen = len - llen;
		memcpy(byte_array->buffer + byte_array->tail, buf, llen);
		memcpy(byte_array->buffer, buf + llen, rlen);
		byte_array->tail = rlen;
	}
	byte_array->size += len;
	return true;
}

bool Kgr_ByteArrayGetData(KgrByteArray *byte_array, char *buf, int len, bool isKeep)
{
	if(byte_array == NULL)
		return false;

	if(byte_array->size < len) {
		return false;
	}
		
	if(byte_array->total - byte_array->head >= len) {
		memcpy(buf, byte_array->buffer + byte_array->head, len);
		if(!isKeep){
			byte_array->head = (byte_array->head + len) % byte_array->total;
		}
	} else {
		int llen = byte_array->total - byte_array->head;
		int rlen = len - llen;
		memcpy(buf, byte_array->buffer + byte_array->head, llen);
		memcpy(buf + llen, byte_array->buffer, rlen);
		if(!isKeep){
			byte_array->head = rlen;
		}
	}

	if(!isKeep){
		byte_array->size -= len;
	}
	return true;
}

int Kgr_ByteArrayGetLen(KgrByteArray *byte_array)
{
	if(byte_array == NULL)
		return -1;
	int length = byte_array->size;
	return length;
}

void Kgr_ByteArrayReset(KgrByteArray *byte_array)
{
	byte_array->size = 0;
	byte_array->head = 0;
	byte_array->tail = 0;
}

