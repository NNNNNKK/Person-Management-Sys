#pragma once
#pragma comment(lib, "Rpcrt4.lib")

#include "string"
#include "vector"
#include "Rpc.h"

using std::string;
using std::vector;

class WinUtil
{
public:
	WinUtil(void);
	~WinUtil(void);
	
	//获取UUID字符串
	static string GetUuidString(void);
	//字符串分割
	static void split(string s, string delimiter, vector<string >& ret);
	//获取1970年1月1日以来的毫秒数
	static string GetMsTime();
};
