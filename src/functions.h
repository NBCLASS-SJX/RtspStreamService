// =====================================================================================
//	Copyright (C) 2018 by Jiaxing Shao.All rights reserved.
//	文 件 名:  functions.h
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2018年06月08日 14时10分10秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#ifndef _FUNCTIONS_H_H_H
#define _FUNCTIONS_H_H_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <list>
using namespace std;
#include <stdio.h>
#include <time.h>
#include <string.h>

#ifdef __GNUC__
#include <iconv.h>
#endif

#ifdef __GNUC__
#define CONFFILE "./Configure.ini"
#else
#define CONFFILE ".\\Configure.ini"
#endif

#ifdef __GNUC__
#define log_debug(format, args...) do {\
	string log_debug_time = GetSystemTime();\
	printf("%s [%s] [line: %d] [debug]: " format "\n", log_debug_time.c_str(), __FILE__, __LINE__, ##args);\
	} while(0)
#else
#define log_debug(format, ...) do {\
	string log_debug_time = GetSystemTime();\
	printf("%s [debug]: " format "\n", log_debug_time.c_str(), ##__VA_ARGS__);\
	} while(0)
#endif

/* * * * * * * * * * * * * * * * * * * */
typedef unsigned char 		uint8;
typedef unsigned short 		uint16;
typedef unsigned int 		uint32;
typedef unsigned long long 	uint64;
/* * * * * * * * * * * * * * * * * * * */

// 获取配置信息
string GetConfigureString(const char *value_name, const char *default_value, const char *filename);
// 返回数据的16进制字符串
string ParseMessageHex(unsigned char *src, unsigned int srclen);
string ParseMessageHex(const char *src, unsigned int srclen);
// 返回当前系统时间
string GetSystemTime();

// 数字转字符串，模板
template<typename T> string ToString(const T &n)
{
	ostringstream oss;
	oss << n;
	string str = oss.str();
	return str;
}

// 替换字符串
void string_replace(string &str, const char *src, const char *dst);
// 去首尾字符
void string_trim(string &str, const char ch);
// 分割字符数据
string *get_part_string(string msg, string separator, int &count);
// 释放内存
void free_part_string(string *&parts);

#ifdef __GNUC__
// 转换字符集,utf-8转未gb2312
int Utf8ToGb2312(char *inbuf, size_t inlen, char *outbuf, size_t outlen);
#endif

#endif

