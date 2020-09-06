#pragma once
#include "string"
using std::string;

namespace Constants
{
	//ǩ���㷨HmacSha256
	const string HMAC_SHA256 = "HmacSHA256";
	//����UTF-8
    const string ENCODING = "UTF-8";
	//UserAgent
	const string USER_AGENT = "demo/aliyun/net";
	//���з�
	const string LF = "\n";
	//�ָ���1
	const string SPE1 = ",";
	//�ָ���2
	const string SPE2 = ":";

	//Ĭ������ʱʱ��,��λ����
	const int DEFAULT_TIMEOUT = 1000;

	//����ǩ����ϵͳHeaderǰ׺,ֻ��ָ��ǰ׺��Header�Ż���뵽ǩ����
	const string CA_HEADER_TO_SIGN_PREFIX_SYSTEM = "x-ca-";
}
