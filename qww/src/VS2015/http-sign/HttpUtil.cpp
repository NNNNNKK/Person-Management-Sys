#include "HttpUtil.h"

HttpUtil::HttpUtil()
{

}

HttpUtil::~HttpUtil()
{

}

string HttpUtil::HttpGet(string url, map<string, string> headers, string appKey, string appSecret, int timeout, list<string> signHeaderPrefixList)
{
	map<string, string> formParam;
	headers = initialBasicHeader(headers, appKey, appSecret, "GET", url, formParam, signHeaderPrefixList);
	SimpleHttpClient httpClient("GET", url.c_str(), timeout);
	for(map<string, string>::iterator it = headers.begin(); it != headers.end(); it++)
	{
		httpClient.setHttpHeader(it->first, it->second);
	}
	httpClient.sendHttpRequest();
	return httpClient.getHttpResponseBody();
}

string HttpUtil::HttpPost(string url, map<string, string> headers, const char *body, unsigned int bodySize, string appKey, string appSecret, int timeout, list<string> signHeaderPrefixList)
{
	map<string, string> formParam;
	headers = initialBasicHeader(headers, appKey, appSecret, "POST", url, formParam, signHeaderPrefixList);

	SimpleHttpClient httpClient("POST", url.c_str(), timeout);
	for(map<string, string>::iterator it = headers.begin(); it != headers.end(); it++)
	{
		httpClient.setHttpHeader(it->first, it->second);
	}
	httpClient.setHttpContent(body/*////, bodySize*/);
	httpClient.sendHttpRequest();
	return httpClient.getHttpResponseBody();
}

string HttpUtil::HttpPost(string url, map<string, string> headers, map<string, string> bodys, string appKey, string appSecret, int timeout, list<string> signHeaderPrefixList)
{
	map<string, string> formParam;
	headers = initialBasicHeader(headers, appKey, appSecret, "POST", url, formParam, signHeaderPrefixList);

	SimpleHttpClient httpClient("POST", url.c_str(), timeout);
	for(map<string, string>::iterator it = headers.begin(); it != headers.end(); it++)
	{
		httpClient.setHttpHeader(it->first, it->second);
	}
	httpClient.setHttpMultipartForm(bodys);
	httpClient.sendHttpRequest();
	return httpClient.getHttpResponseBody();
}

string HttpUtil::HttpPost(string url, map<string, string> headers, string body, string appKey, string appSecret, int timeout, list<string> signHeaderPrefixList)
{
	map<string, string> formParam;
	headers = initialBasicHeader(headers, appKey, appSecret, "POST", url, formParam, signHeaderPrefixList);

	SimpleHttpClient httpClient("POST", url.c_str(), timeout);
	for(map<string, string>::iterator it = headers.begin(); it != headers.end(); it++)
	{
		httpClient.setHttpHeader(it->first, it->second);
	}
	httpClient.setHttpContent(body);
	httpClient.sendHttpRequest();
	return httpClient.getHttpResponseBody();
}

string HttpUtil::HttpPut(string url, map<string, string> headers, const char *body, unsigned int bodySize, string appKey, string appSecret, int timeout, list<string> signHeaderPrefixList)
{
	map<string, string> formParam;
	headers = initialBasicHeader(headers, appKey, appSecret, "PUT", url, formParam, signHeaderPrefixList);

	SimpleHttpClient httpClient("PUT", url.c_str(), timeout);
	for(map<string, string>::iterator it = headers.begin(); it != headers.end(); it++)
	{
		httpClient.setHttpHeader(it->first, it->second);
	}
	httpClient.setHttpUploadData(body, bodySize);
	httpClient.sendHttpRequest();
	return httpClient.getHttpResponseBody();
}

string HttpUtil::HttpDelete(string url, map<string, string> headers, string appKey, string appSecret, int timeout, list<string> signHeaderPrefixList)
{
	map<string, string> formParam;
	headers = initialBasicHeader(headers, appKey, appSecret, "DELETE", url, formParam, signHeaderPrefixList);

	SimpleHttpClient httpClient("DELETE", url.c_str(), timeout);
	for(map<string, string>::iterator it = headers.begin(); it != headers.end(); it++)
	{
		httpClient.setHttpHeader(it->first, it->second);
	}
	httpClient.sendHttpRequest();
	return httpClient.getHttpResponseBody();
}

map<string, string> HttpUtil::initialBasicHeader(map<string, string> &headers, string appKey, string appSecret, string method, string requestAddress, map<string, string> formParam, list<string> signHeaderPrefixList)
{
	SignUtil signUtil;

	headers.insert(std::make_pair(SystemHeader::X_CA_TIMESTAMP, WinUtil::GetMsTime()));
	headers.insert(std::make_pair(SystemHeader::X_CA_NONCE, WinUtil::GetUuidString()));
	headers.insert(std::make_pair(SystemHeader::X_CA_KEY, appKey));
	headers.insert(std::make_pair(SystemHeader::X_CA_SIGNATURE, signUtil.Sign(method, requestAddress, headers, formParam, appSecret, signHeaderPrefixList)));

	return headers;
}