// =====================================================================================
//	Copyright (C) 2018 by Jiaxing Shao.All rights reserved.
//
//	文 件 名:  kgr_utils.h
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年10月15日 02时12分32秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#ifndef _KGR_UTILS_H_H_H
#define _KGR_UTILS_H_H_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <map>
#include <set>
#include <list>
#include <queue>
#include <mutex>
#include <chrono>
#include <thread>
#include <condition_variable>

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#if defined(__GNUC__)
#include <iconv.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <sys/socket.h>
#include <arpa/inet.h>
typedef void (*sighandler_t)(int);
#elif defined(_MSC_VER)
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif //!WIN32_LEAN_AND_MEAN
#include <Winsock2.h>
#include <Windows.h>
#pragma comment(lib, "Ws2_32.lib")
#undef WIN32_LEAN_AND_MEAN
#endif //!_WIN32
#include <io.h>
#include <process.h>
#endif //!__GNUC__

/* 获取配置信息 */
std::string GetConfigureString(const char *value_name, const char *default_value);
std::string GetConfigureString(const char *value_name, const char *default_value, const char *filename);
/* 返回数据的16进制字符串 */
std::string ParseMessageHex(const char *src, unsigned int srclen);
/* 返回当前系统时间 */
std::string GetSystemTime();
/* 通用sleep, 单位秒 */
void TimeSleep(double sec);
/* 时间取整, zone单位为分钟 */
time_t GetRoundTimer(time_t nowTime, int zone);

/* 对象转字符串 */
template<typename T> std::string ToString(const T &n)
{
	std::ostringstream oss;
	oss << n;
	std::string str = oss.str();
	return str;
}

/* 分割字符数据 */
std::string *GetSplitStrings(std::string msg, std::string separator, int &count);
/* 释放内存 */
void FreeSplitStrings(std::string *&parts);
/* 替换字符串 */
void StringReplace(std::string &str, const char *src, const char *dst);
/* 去首尾字符 */
void StringTrim(std::string &str, const char ch);
/* 指定首尾截取字符串 */
std::string StringFindText(std::string &str, const char *head, const char *tail);

#if defined(__GNUC__)
/* 转换字符集,utf-8转未gb2312 */
int Utf8ToGb2312(char *inbuf, size_t inlen, char *outbuf, size_t outlen);
/* 守护进程 */
void RunDeamon();
#endif

#endif

