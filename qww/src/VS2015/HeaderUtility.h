#pragma once
#include <vector>
#include <map>
#include <algorithm>
#include "./utility/Base64.h"
#include "./utility/MD5.h"
#include "./http-sign/HttpHeader.h"

// 外面保证strBody是UTF-8编码
std::string md5_base64_encode(const std::string& strBody)
{
	std::string strMd5Bytes = MD5Encode_bytes(strBody.c_str(), strBody.length());
	return base64_encode(strMd5Bytes.c_str(), strMd5Bytes.length());
}

// 查找key是否存在，找到了返回true
bool FindTheKey(const std::string& key, std::map<std::string, std::string>& header)
{
	std::string strKey(key);
	transform(key.begin(), key.end(), strKey.begin(), toupper);
	std::map<std::string, std::string> tmp = header;
	for each (std::pair<std::string, std::string> it in tmp)
	{
		std::string strTmp(it.first);
		transform(it.first.begin(), it.first.end(), strTmp.begin(), toupper);
		if (strTmp == strKey)
		{
			return true;
		}
	}

	return false;
}