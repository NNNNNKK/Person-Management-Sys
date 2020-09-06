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
	
	//��ȡUUID�ַ���
	static string GetUuidString(void);
	//�ַ����ָ�
	static void split(string s, string delimiter, vector<string >& ret);
	//��ȡ1970��1��1�������ĺ�����
	static string GetMsTime();
};
