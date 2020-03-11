// =====================================================================================
//	Copyright (C) 2019 by Jiaxing Shao.All rights reserved.
//
//	文 件 名:  kgr_logger.h
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年02月15日 15时43分21秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#ifndef _KGR_LOGGER_H_H_H
#define _KGR_LOGGER_H_H_H

#include "kgr_utils.h"

#define MAX_LOG_QUEUE_COUNT (1024)
#define MAX_LOG_QUEUE_SIZE 	(1024 * 16)
#define MAX_LOG_LINE_LENGTH	(2048)
#define MAX_LOG_PATH_LEN (1024)
#define LOG_CREATE_INTERVAL (60 * 60 * 24)

#define GET_CONSUMER(row) ((row)->producer != (row)->consumer) ? &(row)->items[(row)->consumer] : NULL
#define PUT_CONSUMER(row) (row)->consumer = ((row)->consumer + 1) % (row)->size
#define GET_PRODUCER(row) (((row)->producer + 1) % (row)->size != (row)->consumer) ? &(row)->items[(row)->producer] : NULL
#define PUT_PRODUCER(row) (row)->producer = ((row)->producer + 1) % (row)->size

#if defined(__GNUC__)
#define log_debug(format, args...) do { std::string log_debug_time = GetSystemTime();\
	printf("%s [%s] [line: %d] [debug]: " format "\n", log_debug_time.c_str(), __FILE__, __LINE__, ##args);\
	} while(0)
#define log_info(queue, format, args...) do{ if(queue == NULL) break; std::string log_str = GetSystemTime(); \
	int record_ret = Kgr_RecordLog(queue, "%s [info]: " format "\n", log_str.c_str(), ##args); \
	if(!record_ret) break; }while(true)
#define log_warn(queue, format, args...) do{ if(queue == NULL) break; std::string log_str = GetSystemTime(); \
	int record_ret = Kgr_RecordLog(queue, "%s [warn]: " format "\n", log_str.c_str(), ##args); \
	if(!record_ret) break; }while(true)
#define log_error(queue, format, args...) do{ if(queue == NULL) break; std::string log_str = GetSystemTime(); \
	int record_ret = Kgr_RecordLog(queue, "%s [error]: " format "\n", log_str.c_str(), ##args); \
	if(!record_ret) break; }while(true)
#elif defined(_MSC_VER)
#define log_debug(format, ...) do { std::string log_debug_time = GetSystemTime();\
	printf("%s [debug]: " format "\n", log_debug_time.c_str(), ##__VA_ARGS__);\
	} while(0)
#define log_info(queue, format, ...) do{ if(queue == NULL) break; std::string log_str = GetSystemTime(); \
	int record_ret = Kgr_RecordLog(queue, "%s [info]: " format "\n", log_str.c_str(), ##__VA_ARGS__); \
	if(!record_ret) break; }while(true)
#define log_warn(queue, format, ...) do{ if(queue == NULL) break; std::string log_str = GetSystemTime(); \
	int record_ret = Kgr_RecordLog(queue, "%s [warn]: " format "\n", log_str.c_str(), ##__VA_ARGS__); \
	if(!record_ret) break; }while(true)
#define log_error(queue, format, ...) do{ if(queue == NULL) break; std::string log_str = GetSystemTime(); \
	int record_ret = Kgr_RecordLog(queue, "%s [error]: " format "\n", log_str.c_str(), ##__VA_ARGS__); \
	if(!record_ret) break; }while(true)
#endif

typedef struct
{
	char buf[MAX_LOG_LINE_LENGTH];
} LOG;

typedef struct
{
	std::fstream file;
	int size;
	int producer;
	int consumer;
	time_t lasttime;
	char logname[MAX_LOG_PATH_LEN];
	LOG items[MAX_LOG_QUEUE_SIZE];
} LOG_QUEUE;

void Kgr_LogThreadStart(const char *root_directory);
void Kgr_LogThreadStop();
LOG_QUEUE *Kgr_LogQueueCreate(const char *log_name);
void Kgr_LogQueueFree(LOG_QUEUE *&log_queue);
/* 不在外部使用 */
int Kgr_RecordLog(LOG_QUEUE *queue, const char *format, ...);

#endif
