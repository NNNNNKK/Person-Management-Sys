#pragma once
#include "string"
using std::string;

namespace Constants
{
	//签名算法HmacSha256
	const string HMAC_SHA256 = "HmacSHA256";
	//编码UTF-8
    const string ENCODING = "UTF-8";
	//UserAgent
	const string USER_AGENT = "demo/aliyun/net";
	//换行符
	const string LF = "\n";
	//分隔符1
	const string SPE1 = ",";
	//分隔符2
	const string SPE2 = ":";

	//默认请求超时时间,单位毫秒
	const int DEFAULT_TIMEOUT = 1000;

	//参与签名的系统Header前缀,只有指定前缀的Header才会参与到签名中
	const string CA_HEADER_TO_SIGN_PREFIX_SYSTEM = "x-ca-";
}
