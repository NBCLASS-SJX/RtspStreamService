// =====================================================================================
//  Copyright (C) 2020 by Jiaxing Shao. All rights reserved
//
//  文 件 名:  sdp_codec.cpp
//  作    者:  Jiaxing Shao, 13315567369@163.com
//  版 本 号:  1.0
//  创建时间:  2020年03月25日
//  Compiler:  g++
//  描    述:  
// =====================================================================================

#include "sdp_codec.h"

struct sdp_payload *sdp_parser(const char *payload)
{
	sdp_payload *sdp = (sdp_payload*)malloc(sizeof(sdp_payload));
	if(sdp == NULL){
		goto fail;
	}

	return sdp;

fail:
	sdp_destroy(sdp);
	return NULL;
}

std::string sdp_format(const struct sdp_payload *sdp)
{

}

void sdp_destroy(struct sdp_payload *sdp)
{
	if(sdp == NULL){
		return;
	}

}

