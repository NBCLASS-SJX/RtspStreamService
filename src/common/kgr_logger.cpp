// =====================================================================================
//	Copyright (C) 2019 by Jiaxing Shao.All rights reserved.
//
//	文 件 名:  kgr_logger.cpp
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年02月15日 15时43分08秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#include "kgr_logger.h"

static int log_count = 0;
static LOG_QUEUE *log_queues[MAX_LOG_QUEUE_COUNT];
static char log_root_directory[MAX_LOG_PATH_LEN];
static std::mutex log_queue_lock;
#if defined(__GNUC__)
static pthread_t log_thread_id;
#elif defined(_MSC_VER)
static HANDLE log_thread_id;
#endif
static bool log_stop_flag = false;

void check_directory(const char *directory)
{
#if defined(__GNUC__)
	DIR *dir = opendir(directory);
	if(dir == NULL){
		mkdir(directory, S_IRWXU);
	}else{
		closedir(dir);
	}
#elif defined(_MSC_VER)
	if (_access(directory, 0) == -1) {
		CreateDirectoryA(directory, NULL);
	}
#endif
}

void logfile_open(LOG_QUEUE *log_queue)
{
	char log_file_path[MAX_LOG_PATH_LEN] = { 0 };
	char log_file_name[MAX_LOG_PATH_LEN] = { 0 };
	struct tm local;
#if defined(__GNUC__)
	localtime_r(&log_queue->lasttime, &local);
	sprintf(log_file_path, "%s/%0.4d%0.2d%0.2d", log_root_directory, 
			local.tm_year + 1900, local.tm_mon + 1, local.tm_mday);
	sprintf(log_file_name, "%s/%s.txt", log_file_path, log_queue->logname);
#elif defined(_MSC_VER)
	localtime_s(&local, &log_queue->lasttime);
	sprintf_s(log_file_path, MAX_LOG_PATH_LEN, "%s\\%0.4d%0.2d%0.2d", log_root_directory, 
			local.tm_year + 1900, local.tm_mon + 1, local.tm_mday);
	sprintf_s(log_file_name, MAX_LOG_PATH_LEN, "%s\\%s.txt", log_file_path, log_queue->logname);
#endif
	check_directory(log_file_path);
	log_queue->file.open(log_file_name, std::ios::in | std::ios::out | std::ios::app);
}

#if defined(__GNUC__)
static void *log_worker_func(void *arg)
#elif defined(_MSC_VER)
static unsigned int __stdcall log_worker_func(void *arg)
#endif
{
	while(true)
	{
		if(log_stop_flag){
			break;
		}

		time_t curtime = time(NULL);
		int count = 0;
		for(int i = 0; i < log_count; i++)
		{
			LOG_QUEUE *queue = log_queues[i];
			LOG *log = GET_CONSUMER(queue);
			if(log == NULL){
				count++;
				continue;
			}

			if(curtime - queue->lasttime >= 86400) {
				queue->file.close();
				queue->lasttime = curtime;
				logfile_open(queue);
			}
			queue->file.write(log->buf, strlen(log->buf));
			queue->file.flush();
			PUT_CONSUMER(queue);
		}

		if(log_count == count){
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}
#if defined(_MSC_VER)
	return 0;
#endif
}

void Kgr_LogThreadStart(const char *root_directory)
{
	check_directory(root_directory);
	memset(log_root_directory, 0, MAX_LOG_PATH_LEN);
	memcpy(log_root_directory, root_directory, strlen(root_directory));
#if defined(__GNUC__)
	pthread_create(&log_thread_id, 0, log_worker_func, NULL);
#elif defined(_MSC_VER)
	log_thread_id = (HANDLE)_beginthreadex(NULL, 0, log_worker_func, NULL, 0, NULL);
#endif
}

void Kgr_LogThreadStop()
{
	log_stop_flag = true;
#if defined(__GNUC__)
	pthread_join(log_thread_id, NULL);
#elif defined(_MSC_VER)
	WaitForSingleObject(log_thread_id, INFINITE);
#endif
	for(int i = 0; i < log_count; i++){
		Kgr_LogQueueFree(log_queues[i]);
	}
	log_count = 0;
}

LOG_QUEUE *Kgr_LogQueueCreate(const char *log_name)
{
	std::lock_guard<std::mutex> lock(log_queue_lock);
	if(log_count >= MAX_LOG_QUEUE_COUNT){
		return NULL;
	}
	LOG_QUEUE *log_queue = new LOG_QUEUE;
	if(log_queue == NULL){
		return NULL;
	}

	log_queue->producer = 0;
	log_queue->consumer = 0;
	memset(log_queue->logname, 0, MAX_LOG_PATH_LEN);
	memset(log_queue->items, 0, MAX_LOG_QUEUE_SIZE * sizeof(LOG));
	log_queue->size = MAX_LOG_QUEUE_SIZE;
	memcpy(log_queue->logname, log_name, strlen(log_name));
	log_queue->lasttime = time(NULL);
	logfile_open(log_queue);
	log_queues[log_count++] = log_queue;
	return log_queue;
}

void Kgr_LogQueueFree(LOG_QUEUE *&log_queue)
{
	if(log_queue == NULL){
		return;
	}
	log_queue->file.close();
	delete log_queue;
	log_queue = NULL;
}

int Kgr_RecordLog(LOG_QUEUE *log_queue, const char *format, ...)
{
	LOG *log = GET_PRODUCER(log_queue);
	if(log == NULL) {
		return -1;
	}

	va_list args;
	va_start(args, format);
#if defined(__GNUC__)
	vsprintf(log->buf, format, args);
#elif defined(_MSC_VER)
	vsprintf_s(log->buf, format, args);
#endif
	va_end(args);
	PUT_PRODUCER(log_queue);
	return 0;
}

