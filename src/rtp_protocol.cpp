// =====================================================================================
//	Copyright (C) 2019 by Jiaxing Shao.All rights reserved.
//	文 件 名:  rtp_protocol.cpp
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年06月25日 19时11分35秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#include "functions.h"
#include "rtp_protocol.h"
#include "sps_decode.h"

#include <list>
#include <string>
#include <cstring>
using namespace std;

rtp_interleaved_frame_t parse_rtp_frame(unsigned char *buffer)
{
	rtp_interleaved_frame_t frame;
	int start_bit = 0;
	memset(&frame, 0, sizeof(rtp_interleaved_frame_t));
	frame.magic 		= U(buffer, start_bit, 8);
	frame.channel_id 	= U(buffer, start_bit, 8);
	frame.length 		= U(buffer, start_bit, 16);
	return frame;
}

rtp_hdr_t parse_rtp_header(unsigned char *buffer)
{
	rtp_hdr_t header;
	int start_bit = 0;
	memset(&header, 0, sizeof(rtp_hdr_t));

	header.version	= U(buffer, start_bit, 2);
	header.p		= U(buffer, start_bit, 1);
	header.x		= U(buffer, start_bit, 1);
	header.cc		= U(buffer, start_bit, 4);
	header.m		= U(buffer, start_bit, 1);
	header.pt		= U(buffer, start_bit, 7);
	header.seq		= U(buffer, start_bit, 16);
	header.ts		= U(buffer, start_bit, 32);
	header.ssrc		= U(buffer, start_bit, 32);
	for(int i = 0; i < header.cc; i++) {
		header.csrc[i] = U(buffer, start_bit, 32);
	}
	return header;
}

rtp_nalu_hdr_t parse_rtp_h264_format(unsigned char *buffer)
{
	rtp_nalu_hdr_t nalu_hdr;
	int start_bit = 0;
	memset(&nalu_hdr, 0, sizeof(rtp_nalu_hdr_t));
	
	nalu_hdr.f		= U(buffer, start_bit, 1);
	nalu_hdr.nri	= U(buffer, start_bit, 2);
	nalu_hdr.type	= U(buffer, start_bit, 5);
	return nalu_hdr;
}

int parse_nalu_stap(rtp_h264_nalu_t *nalu, unsigned char *buffer)
{
	int nalu_size = (buffer[0] << 8) + buffer[1];
	rtp_nalu_hdr_t nalu_hdr = parse_rtp_h264_format(buffer + 2);
	nalu->data[0] = 0x00;
	nalu->data[1] = 0x00;
	nalu->data[2] = 0x00;
	nalu->data[3] = 0x01;
	memcpy(nalu->data + 4, buffer + 2, nalu_size);
	nalu->type = nalu_hdr.type;
	nalu->size = nalu_size + 4;
	nalu->union_type = (nalu_hdr.f << 7) + (nalu_hdr.nri << 5) + nalu_hdr.type;
	nalu->finish = true;
	return nalu_size + 2;
}

int parse_nalu(rtp_h264_nalu_t *nalu, unsigned char *buffer, int length)
{
	int pos = 0;
	rtp_hdr_t header = parse_rtp_header(buffer + pos);
	pos += 12 + header.cc * 4;
	rtp_nalu_hdr_t nalu_hdr = parse_rtp_h264_format(buffer + pos);
	pos += 1;
	if(nalu_hdr.type <= 23) {
		pos -= 1;
		nalu->data[0] = 0x00;
		nalu->data[1] = 0x00;
		nalu->data[2] = 0x00;
		nalu->data[3] = 0x01;
		memcpy(nalu->data + 4, buffer + pos, length - pos);
		nalu->type = nalu_hdr.type;
		nalu->size = length - pos + 4;
		nalu->union_type = (nalu_hdr.f << 7) + (nalu_hdr.nri << 5) + nalu_hdr.type;
		nalu->finish = true;
		return 0;
	} else if(nalu_hdr.type == 24) {
		int n = parse_nalu_stap(nalu, buffer + pos);
		return pos + n;
	} else if(nalu_hdr.type == 25){
		log_debug("nalu type, %d.", nalu_hdr.type);
		nalu->size = 0;
		nalu->finish = false;
	} else if(nalu_hdr.type == 26){
		log_debug("nalu type, %d.", nalu_hdr.type);
		nalu->size = 0;
		nalu->finish = false;
	} else if(nalu_hdr.type == 27){
		log_debug("nalu type, %d.", nalu_hdr.type);
		nalu->size = 0;
		nalu->finish = false;
	} else if(nalu_hdr.type == 28) {
		int s = (buffer[pos] >> 7) & 0x01;
		int e = (buffer[pos] >> 6) & 0x01;
		int r = (buffer[pos] >> 5) & 0x01; // 保留字节
		int type = buffer[pos] & 0x1F;
		if(s) {
			buffer[pos] = nalu->union_type & 0xFF;
			nalu->data[0] = 0x00;
			nalu->data[1] = 0x00;
			nalu->data[2] = 0x00;
			nalu->data[3] = 0x01;
			memcpy(nalu->data + 4, buffer + pos, length - pos);
			nalu->type = type;
			nalu->size = length - pos + 4;
			nalu->union_type = (nalu_hdr.f << 7) + (nalu_hdr.nri << 5) + type;
			nalu->data[4] = nalu->union_type & 0xFF;
			nalu->finish = false;
		} else if(e) {
			if(nalu->size == 0)
				return 0;
			pos += 1;
			memcpy(nalu->data + nalu->size, buffer + pos, length - pos);
			nalu->size = length - pos + nalu->size;
			nalu->finish = true;
		} else {
			if(nalu->size == 0)
				return 0;
			pos += 1;
			memcpy(nalu->data + nalu->size, buffer + pos, length - pos);
			nalu->size = length - pos + nalu->size;
		}
		return 0;
	} else if(nalu_hdr.type == 29){
		log_debug("nalu type, %d.", nalu_hdr.type);
		nalu->size = 0;
		nalu->finish = false;
	} else {
		log_debug("parse_nalu error, unknown message, %d.", nalu_hdr.type);
		nalu->size = 0;
		nalu->finish = false;
	}
	return -1;
}

