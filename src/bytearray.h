// =====================================================================================
//	Copyright (C) 2018 by Jiaxing Shao.All rights reserved.
//	文 件 名:  bytearray.h
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2018年06月08日 14时10分10秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#ifndef __BYTEARRAY_H_H_H
#define __BYTEARRAY_H_H_H

#ifdef __GNUC__
#include <pthread.h>
#else
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#endif

typedef struct
{
	int size;
	int total;
	int head;
	int tail;
#ifdef __GNUC__
	pthread_mutex_t lock;
#else
	CRITICAL_SECTION cs;
#endif
	char buffer[0];
}t_byte_array;

// 创建对象, 默认16M
t_byte_array *create_byte_array(int size = 1024 * 1024 * 16);
// 放进队列，失败返回-1
int put_byte_array(t_byte_array *byte_array, const char *buf, int len);
// 要取出的长度,返回取出的消息长度，失败返回 -1
int get_byte_array(t_byte_array *byte_array, char *buf, int len);
// 返回当前队列长度
int get_byte_array_len(t_byte_array *byte_array);
// 重置队列
void reset_byte_array(t_byte_array *byte_array);
// 释放内存
void free_byte_array(t_byte_array* &byte_array);

#endif

