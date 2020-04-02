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

#include "../sdp_codec.h"
#include <iostream>
#include <fstream>
using namespace std;

int main()
{
	char payload[4096] = { 0 };
	fstream file("payload.sdp", ios::in | ios::out);
	if(!file.is_open()){
		return 1;
	}
	file.read(payload, sizeof(payload));
	file.close();
	struct sdp_payload *sdp = sdp_parser(payload);
	std::string sdp_data = sdp_format(sdp);
	if(sdp_data.length() > 0){
		std::cout << sdp_data << std::endl;
	}else{
		std::cout << "error" << std::endl;
	}
	sdp_destroy(sdp);
	return 0;
}

