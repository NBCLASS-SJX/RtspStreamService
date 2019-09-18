// =====================================================================================
//	Copyright (C) 2019 by Jiaxing Shao.All rights reserved.
//	文 件 名:  main.cpp
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年05月07日 23时48分27秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================


#include <iostream>
#include <thread>
#include <list>
using namespace std;
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include "src/functions.h"
#include "src/bytearray.h"
#include "src/rtsp_task.h"
#include "src/rtp_protocol.h"
#include "src/sps_decode.h"

t_byte_array *rtp_array;
pthread_mutex_t clntLock;
list<int> clnts;
t_rtsp_info *clnt_rtsp_info = NULL;
t_rtsp_info *serv_rtsp_info = NULL;
// {1,"SIGHUP"},{2,"SIGINT"},{3,"SIGQUIT"},{6,"SIGABRT"},{8,"SIGFPE"},{11,"SIGSEGV"},{13,"SIGPIPE"};

void signal_handler(int signum);
void *bytes_process(void *arg);
void *serv_worker(void *arg);

int main(int argc, char *argv[])
{
	struct sigaction sa;
	sa.sa_handler = signal_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGABRT, &sa, NULL);
	sigaction(SIGFPE, &sa, NULL);
	sigaction(SIGSEGV, &sa, NULL);
	sigaction(SIGPIPE, &sa, NULL);

	string rtsp_url = GetConfigureString("RTSP.URL", "rtsp://184.72.239.149/vod/mp4:BigBuckBunny_115k.mov", CONFFILE);
	rtp_array = create_byte_array(1024 * 1024 * 32);
	clnt_rtsp_info = create_rtsp_clnt_info(rtsp_url.c_str());
	int sockfd = connect_server(clnt_rtsp_info->ipaddr, clnt_rtsp_info->port);
	if(!rtsp_request(clnt_rtsp_info, sockfd)) {
		log_debug("rtsp connect failed");
		print_rtsp_info(clnt_rtsp_info);
		free_rtsp_info(clnt_rtsp_info);
		free_byte_array(rtp_array);
		return EXIT_FAILURE;
	} else {
		log_debug("rtsp connect succeed");
		print_rtsp_info(clnt_rtsp_info);
	}

	pthread_mutex_init(&clntLock, NULL);
	thread t1(bytes_process, (void*)NULL);
	thread t2(serv_worker, (void*)NULL);

	char buffer[1024 * 1024] = { 0 };
	int n = 0;
	while(true) {
		n = recv(sockfd, buffer, 1024 * 1024, 0);
		if(n <= 0) {
			log_debug("end of stream");
			break;
		}
		put_byte_array(rtp_array, buffer, n);
	}

	log_debug("well done");
	close(sockfd);
	t1.join();
	t2.join();
	pthread_mutex_destroy(&clntLock);
	free_rtsp_info(clnt_rtsp_info);
	
	return EXIT_SUCCESS;
}

void signal_handler(int signum)
{
	if(signum == 1)
		log_debug("SIGHUP");
	else if(signum == 2)
		log_debug("SIGINT");
	else if(signum == 3)
		log_debug("SIGQUIT");
	else if(signum == 6)
		log_debug("SIGABRT");
	else if(signum == 8)
		log_debug("SIGFPE");
	else if(signum == 11)
		log_debug("SIGSEGV");
	else if(signum == 13)
		log_debug("SIGPIPE");
}

void *bytes_process(void *arg)
{
	unsigned char buffer[256 * 256] = { 0 };
	int length = 0;
	int ret = 0;

	string savefile = GetConfigureString("savefile", "enable", CONFFILE);
	string filename = GetConfigureString("filename", "test.mp4", CONFFILE);
	int fd = 0;
	bool isSave = false;
	rtp_h264_nalu_t *nalu = NULL;
	if(strcmp(savefile.c_str(), "enable") == 0){
		isSave = true;
		fd = open(filename.c_str(), O_RDWR | O_CREAT, 0666);
		nalu = (rtp_h264_nalu_t*)malloc(sizeof(rtp_h264_nalu_t) + 1024 * 1024 * 32);
		nalu->size = 0;
		nalu->finish = false;
	}


	while(true) {
		ret = get_byte_array(rtp_array, (char*)buffer, 4);
		if(ret <= 0) {
			sleep(0.01);
			continue;
		}

		if(ret > 0) {
			length = buffer[2] * 256 + buffer[3];
			if(buffer[0] != 0x24 || length > 2048) {
				log_debug("reset byte array");
				reset_byte_array(rtp_array);
				continue;
			}

			while(true) {
				ret = get_byte_array(rtp_array, (char*)buffer + 4, length);
				if(ret > 0) {
					break;
				}
				sleep(0.01);
			}
		}

		if(isSave) 
		{
			if(buffer[1] == 0)
			{
				int ret = parse_nalu(nalu, buffer + 4, length);
				if(nalu->finish) {
					if(nalu->type == 7){
						sps_data_t sps;
						memset(&sps, 0, sizeof(sps));
						h264_decode_sps((unsigned char*)nalu->data, nalu->size, sps);
						h264_print_sps(sps);
						if(sps.pic_order_cnt_type == 1)
							delete [] sps.offset_for_ref_frame;
					}
					log_debug("NALU Type %d, Write File Size: %d", nalu->type, nalu->size);
					write(fd, nalu->data, nalu->size);
					nalu->size = 0;
					nalu->finish = false;
				}

				if(ret > 0) {
					while(ret < length) {
						int n = parse_nalu_stap(nalu, buffer + ret + 4);
						if(nalu->type == 7){
							sps_data_t sps;
							memset(&sps, 0, sizeof(sps));
							h264_decode_sps((unsigned char*)nalu->data, nalu->size, sps);
							h264_print_sps(sps);
							if(sps.pic_order_cnt_type == 1)
								delete [] sps.offset_for_ref_frame;
						}

						write(fd, nalu->data, nalu->size);
						log_debug("Write File Size: %d", nalu->size);
						ret += n;
						nalu->size = 0;
						nalu->finish = false;
					}
				}
			}
		}

		if(clnts.size() == 0)
			continue;

		pthread_mutex_lock(&clntLock);
		list<int>::iterator iter;
		for(iter = clnts.begin(); iter != clnts.end(); iter++) {
			int n = send((*iter), buffer, length + 4, 0);
			if(n <= 0)
				log_debug("send error, %d, errno %d.", (*iter), errno);
		}
		pthread_mutex_unlock(&clntLock);
	}

	if(nalu != NULL)
		free(nalu);
	close(fd);
	return NULL;
}

void *serv_worker(void *arg)
{
	string ipaddr = GetConfigureString("RTSP.ADDR", "192.168.136.120", CONFFILE);
	string port = GetConfigureString("RTSP.PORT", "10086", CONFFILE);
	serv_rtsp_info = create_rtsp_serv_info(clnt_rtsp_info, "/video/test", ipaddr.c_str(), port.c_str());

	print_rtsp_info(clnt_rtsp_info);
	print_rtsp_info(serv_rtsp_info);

	log_debug("rtsp url: %s", serv_rtsp_info->rtsp_url);

	int serv_sock = create_server_socket(serv_rtsp_info->port);
	fd_set readfds, fds;
	int maxfd = serv_sock;
	struct timeval timeout;
	int result = 0;
	char buffer[2048] = { 0 };

	FD_ZERO(&readfds);
	FD_SET(serv_sock, &readfds);

	while(true) {
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		FD_ZERO(&fds);
		fds = readfds;
		result = select(maxfd + 1, &fds, NULL, NULL, &timeout);
		if(result > 0) {
			if(FD_ISSET(serv_sock, &fds)) {
				struct sockaddr_in clnt_addr;
				socklen_t clnt_addr_size = sizeof(sockaddr_in);
				memset(&clnt_addr, 0, sizeof(sockaddr_in));
				int sockfd = accept(serv_sock, (sockaddr*)&clnt_addr, &clnt_addr_size);
				if(rtsp_response(serv_rtsp_info, sockfd)) {
					pthread_mutex_lock(&clntLock);
					clnts.push_back(sockfd);
					FD_SET(sockfd, &readfds);
					if(sockfd > maxfd)
						maxfd = sockfd;
					log_debug("客户端连接成功 %d, 当前连接数 %d", sockfd, clnts.size());
					pthread_mutex_unlock(&clntLock);
				}else{
					log_debug("客户端连接失败 %d, 当前连接数 %d", sockfd, clnts.size());
					close(sockfd);
				}

				if(result == 1)
					continue;
			}

			list<int>::iterator iter;
			for(iter = clnts.begin(); iter != clnts.end(); ) {
				if(FD_ISSET((*iter), &fds)) {
					int n = recv((*iter), buffer, 2048, 0);
					if(n <= 0) {
						pthread_mutex_lock(&clntLock);
						int fd = *iter;
						iter = clnts.erase(iter);
						log_debug("客户端关闭连接 %d, 当前连接数 %d.", fd, clnts.size());
						pthread_mutex_unlock(&clntLock);
						FD_CLR(fd, &readfds);
						close(fd);
						continue;
					}
				}
				iter++;
			}
		}
	}
	return NULL;
}

