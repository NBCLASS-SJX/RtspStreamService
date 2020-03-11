// =====================================================================================
//	Copyright (C) 2019 by Jiaxing Shao.All rights reserved.
//
//	文 件 名:  kgr_utils.cpp
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年10月15日 02时12分32秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#include "kgr_utils.h"

std::string GetConfigureString(const char *value_name, const char *default_value)
{
#if defined(__GNUC__)
	char filename[] = "./Configure.ini";
#elif defined(_MSC_VER)
	char filename[] = ".\\Configure.ini";
#endif
	return GetConfigureString(value_name, default_value, filename);
}

std::string GetConfigureString(const char *value_name, const char *default_value, const char *filename)
{
	int position = 0;
	std::string value;
	std::ifstream iFile(filename, std::ios::in);

	if (!iFile)
	{
		{
			std::string log_debug_time = GetSystemTime();
#if defined(__GNUC__)
			printf("%s [%s] [line: %d] [debug]: Not Found File %s \n", log_debug_time.c_str(), __FILE__, __LINE__, filename);
#elif defined(_MSC_VER)
			printf("%s [debug]: Not Found File %s \n", log_debug_time.c_str(), filename);
#endif
		}
DEFAULT_VALUE:
		std::ofstream oFile(filename, std::ios::out | std::ios::app);
		char property[1024] = { 0 };
#if defined(__GNUC__)
		sprintf(property, "%s = %s\n", value_name, default_value);
#elif defined(_MSC_VER)
		sprintf_s(property, 1024, "%s = %s\n", value_name, default_value);
#endif
		oFile.write(property, strlen(property));
		oFile.close();
		value = default_value;
		{
			std::string log_debug_time = GetSystemTime();
#if defined(__GNUC__)
			printf("%s [%s] [line: %d]: [%s][%s] Not Found, use default value: %s \n", log_debug_time.c_str(), __FILE__, __LINE__, 
					filename, value_name, default_value);
#elif defined(_MSC_VER)
			printf("%s : [%s][%s] Not Found, use default value: %s \n", log_debug_time.c_str(), filename, value_name, default_value);
#endif
		}
		return value;
	}

	while (!iFile.eof())
	{
		std::string s;
		getline(iFile, s);

		if(s.length() <= 0)
			continue;

		if (s[0] == '#')
			continue;

		position = s.find("=");
		if (position == s.npos) {
			continue;
		}

		std::string key = s.substr(0, position);
		value = s.substr(position + 1, s.npos);
		if(strncmp(value_name, key.c_str(), strlen(value_name)) != 0) {
			continue;
		}

		iFile.close();
		StringTrim(value, ' ');
		return value;
	}

	iFile.close();
	goto DEFAULT_VALUE;
}

std::string ParseMessageHex(const char *src, unsigned int srclen)
{
	if(src == NULL || srclen < 0)
		return "";

	std::ostringstream oss;
	oss << std::setfill('0') << std::uppercase;
	for(int i = 0; i < srclen; i++) {
		oss << std::setw(2) << std::hex << static_cast<int> (src[i] & 0xFF) << std::dec  << ' ';
	}
	std::string str = oss.str().c_str();
	return str;
}

std::string GetSystemTime()
{
	time_t timer = time(NULL);
	char chTime[32] = { 0 };
	struct tm local;
#if defined(__GNUC__)
	localtime_r(&timer, &local);
	sprintf(chTime, "%0.4d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d", local.tm_year + 1900, local.tm_mon + 1, 
			local.tm_mday, local.tm_hour, local.tm_min, local.tm_sec);
#elif defined(_MSC_VER)
	localtime_s(&local, &timer);
	sprintf_s(chTime, 32, "%0.4d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d", local.tm_year + 1900, local.tm_mon + 1, 
			local.tm_mday, local.tm_hour, local.tm_min, local.tm_sec);
#endif
	std::string strTime = chTime;
	return strTime;
}

void TimeSleep(double sec)
{
#if defined(__GNUC__)
	sleep(sec);
#elif defined(_MSC_VER)
	Sleep(sec * 1000);
#endif
}

time_t GetRoundTimer(time_t nowTime, int zone)
{
	struct tm local;
#if defined(__GNUC__)
	localtime_r(&nowTime, &local);
#elif defined(_MSC_VER)
	localtime_s(&local, &nowTime);
#endif
	local.tm_min = (local.tm_min / zone) * zone;
	local.tm_sec = 0;
	time_t timer = mktime(&local);
	return timer;
}

std::string *GetSplitStrings(std::string msg, std::string separator, int &count)
{
	std::list<std::string> list_str;
	int pos = 0;
	count = 0;
	while(true)
	{
		int n = msg.find(separator, pos);
		std::string str = msg.substr(pos, n - pos);
		if(str != "") {
			list_str.push_back(str);
			count += 1;
		}
		if(n == -1)
			break;
		pos = n + separator.length();
	}
	std::string *strs = new std::string[count];
	std::list<std::string>::iterator iter;
	int i = 0;
	for(iter = list_str.begin(); iter != list_str.end(); iter++) {
		strs[i++] = (*iter);
	}
	return strs;
}

void FreeSplitStrings(std::string *&parts)
{
	if(parts == NULL) {
		delete [] parts;
		parts = NULL;
	}
}

void StringReplace(std::string &str, const char *src, const char *dst)
{
	while(str.find(src) != -1) {
		str.replace(str.find(src), strlen(src), dst);
	}
}

void StringTrim(std::string &str, const char ch)
{
	while(str.length() > 0)
	{
		if(str[0] == ch) {
			str.erase(0, 1);
		} else {
			break;
		}
	}
	
	while(str.length() > 0)
	{
		if(str[str.length() - 1] == ch) {
			str.erase(str.length()-1,str.length());
		} else {
			break;
		}
	}
}

std::string StringFindText(std::string &str, const char *head, const char *tail)
{
	std::string retStr("");
	size_t headIdx = 0;
	size_t tailIdx = 0;
	std::string headStr = head;
	std::string tailStr = tail;

	do {
		if (str.length() == 0 || headStr.length() == 0 || tailStr.length() == 0) {
			break;
		}
		if ((headIdx = str.find(headStr.c_str())) == -1) {
			str = "";
			break;
		}
		tailIdx = str.find(tailStr.c_str(), headIdx);
		if (tailIdx == -1) {
			break;
		}
		retStr = str.substr(headIdx, tailIdx - headIdx + tailStr.size());
		str = str.substr(tailIdx + tailStr.size());
	} while (0);
	return retStr;
}

#if defined(__GNUC__)
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

void sig_process(int signo, sighandler_t handler)
{
	struct sigaction sa;
	sa.sa_handler = handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(signo, &sa, NULL);
}

void sig_handler(int signo)
{
	if(signo == SIGPIPE) {
		return;
	}
}

void RunDeamon()
{
	pid_t pid = fork();
	if(pid < 0) {
		exit(1);
	} else if(pid > 0) {
		exit(0);
	}

	setsid();
	sig_process(SIGHUP, SIG_IGN);
	sig_process(SIGTERM, SIG_IGN);
	sig_process(SIGPIPE, sig_handler);
}
#endif

