#pragma once

#include "string"
#include "map"
#include "list"

using std::string;
using std::map;
using std::list;

class SignUtil
{
public:
	SignUtil();
	string Sign(string method, string url, map<string, string> &headers, map<string, string> formParamMap, string secret, list<string> signHeaderPrefixList);

private:
	 string BuildStringToSign(map<string, string> &headers, string url, map<string, string> formParamMap, string method, list<string> signHeaderPrefixList);
	 string BuildResource(string url, map<string, string> formParamMap);
	 string BuildHeaders(map<string, string> &headers, list<string> signHeaderPrefixList);
	 bool IsHeaderToSign(string headerName, list<string> signHeaderPrefixList);
};