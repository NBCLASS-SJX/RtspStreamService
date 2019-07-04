// =====================================================================================
//	Copyright (C) 2018 by Jiaxing Shao.All rights reserved.
//	文 件 名:  functions.cpp
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2018年06月08日 14时10分10秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================



#include "functions.h"


string GetConfigureString(const char *value_name, const char *default_value, const char *filename)
{
	int position = 0;
	string value;
	ifstream iFile(filename, ios::in);

	if (!iFile)
	{
		log_debug("Not Found File %s", filename);
DEFAULT_VALUE:
		ofstream oFile(filename, ios::out | ios::app);
		char property[1024] = { 0 };
		sprintf(property, "%s = %s\n", value_name, default_value);
		oFile.write(property, strlen(property));
		oFile.close();
		value = default_value;
		log_debug("[%s] Not Found, use default value: %s", value_name, default_value);
		return value;
	}

	while (!iFile.eof())
	{
		string s;
		getline(iFile, s);

		if(s.length() <= 0)
			continue;

		if (s[0] == '#')
			continue;

		position = s.find("=");
		if (position == s.npos)
		{
			continue;
		}

		string key = s.substr(0, position);
		value = s.substr(position + 1, s.npos);
		if(strncmp(value_name, key.c_str(), strlen(value_name)) != 0)
		{
			continue;
		}

		iFile.close();
		string_trim(value, ' ');
		return value;
	}

	iFile.close();
	goto DEFAULT_VALUE;
}

string *get_part_string(string msg, string separator, int &count)
{
	list<string> list_str;
	int pos = 0;
	count = 0;

	while(true) {
		int n = msg.find(separator, pos);
		string str = msg.substr(pos, n - pos);
		if(str != "") {
			list_str.push_back(str);
			count += 1;
		}

		if(n == -1)
			break;
		pos = n + separator.length();
	}
	string *strs = new string[count];
	list<string>::iterator iter;
	int i = 0;
	for(iter = list_str.begin(); iter != list_str.end(); iter++)
		strs[i++] = (*iter);
	return strs;
}

void free_part_string(string *&parts)
{
	if(parts == NULL) {
		delete [] parts;
		parts = NULL;
	}
}

void string_replace(string &str, const char *src, const char *dst)
{
	while(str.find(src) != -1)
		str.replace(str.find(src), strlen(src), dst);
}

void string_trim(string &str, const char ch)
{
	while(str.length() > 0) {
		if(str[0] == ch)
			str.erase(0, 1);
		else
			break;
	}
	
	while(str.length() > 0) {
		if(str[str.length() - 1] == ch)
			str.erase(str.length()-1,str.length());
		else
			break;
	}
}

string ParseMessageHex(const char *src, unsigned int srclen)
{
	if(src == NULL || srclen < 0)
		return "";

	ostringstream oss;
	oss << setfill('0') << uppercase;
	for(int i = 0; i < srclen; i++)
		oss << setw(2) << hex << static_cast<int> (src[i] & 0xFF) << dec  << ' ';
	string str = oss.str().c_str();
	return str;
}

string ParseMessageHex(const unsigned char *src, unsigned int srclen)
{
	string str = ParseMessageHex((char*)src, srclen);
	return str;
}

string GetSystemTime()
{
	time_t timer = time(NULL);
	char chTime[32] = { 0 };
	struct tm local;
#ifdef __GNUC__
	localtime_r(&timer, &local);
	sprintf(chTime, "%0.4d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d", local.tm_year + 1900, local.tm_mon + 1, 
			local.tm_mday, local.tm_hour, local.tm_min, local.tm_sec);
#else
	localtime_s(&local, &timer);
	sprintf_s(chTime, 32, "%0.4d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d", local.tm_year + 1900, local.tm_mon + 1, 
			local.tm_mday, local.tm_hour, local.tm_min, local.tm_sec);
#endif
	string strTime = chTime;
	return strTime;
}

#ifdef __GNUC__
int Utf8ToGb2312(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	if(inbuf == NULL || outbuf == NULL)
		return -1;

	iconv_t cd = iconv_open("GB2312", "UTF-8");
	char *pIn = inbuf;
	char *pOut = outbuf;

	// 执行完此函数后，inlen值为转换后剩余长度，outlen为pOut剩余未使用字节长度
	size_t ret = iconv(cd, &pIn, &inlen, &pOut, &outlen);
	iconv_close(cd);
	return inlen;
}
#endif
