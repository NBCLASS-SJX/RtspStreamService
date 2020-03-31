// =====================================================================================
//  Copyright (C) 2020 by Jiaxing Shao. All rights reserved
//
//  文 件 名:  main.cpp
//  作    者:  Jiaxing Shao, 13315567369@163.com
//  版 本 号:  1.0
//  创建时间:  2020年03月30日
//  Compiler:  g++
//  描    述:  
// =====================================================================================

#include "sdp_codec.h"
#include <iostream>

int main()
{
	char payload[1024] = { 0 };
	int fd = open("payload.sdp", O_RDONLY);
	int n = read(fd, payload, sizeof(payload));
	close(fd);
	if(n < 0){
		perror("read");
		return 1;
	}
	struct sdp_payload *sdp = sdp_parser(payload);
	std::string sdp_data = sdp_format(sdp);
	sdp_destroy(sdp);
	if(sdp_data.length() > 0){
		std::cout << sdp_data << std::endl;
	}else{
		std::cout << "error" << std::endl;
	}
	return 0;
}

