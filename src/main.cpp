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
	char payload[] = "v=0\r\n\
		o=foo 53655765 2353687637 IN IP6 FF15::101/3\r\n\
		s=Example Audio\r\n\
		i=This is a very short description.\r\n\
		e=foo@bar.example.com\r\n\
		c=IN IP4 224.2.0.1/127\r\n\
		t=3149328700 3159328700\r\n\
		r=7d 1h 10m 25h 12 31432 123 12312\r\n\
		r=123 1233 234 12414\r\n\
		t=414123 123123 1231231\r\n\
		r=231 213213 1231\r\n\
		r=123 12090 90 80\r\n\
		r=89 890 09\r\n\
		z=318 213 89043 3122  123\r\n\
		a=X-ConferenceModel: foo 01234567890987654321098765432109876543210\r\n\
		m=audio 3456 RTP/AVP 10 5\r\n\
		a=rtpmap:0 PCMU/8000\r\n\
		a=rtpmap:5 DVI4/8000\r\n";

	struct sdp_payload * sdp = sdp_parser(payload);
	std::string sdp_data = sdp_format(sdp);
	sdp_destroy(sdp);
	std::cout << sdp_data << std::endl;
	return 0;
}
