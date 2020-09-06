// HttpUtillib.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "HttpUtillib.h"
#include "./http/simple_http_client.h"
#include "./http-sign/SignUtil.h"
#include "./http-sign/HttpHeader.h"
#include "./http-sign/SystemHeader.h"
#include "./http-sign/constants.h"
#include "./http-sign/WinUtil.h"
#include "HeaderUtility.h"
#include <algorithm>

HTTPUTILLIB_API string STDCALL HttpGet(string url, map<string, string> headers, string appKey, string appSecret, int timeout, list<string> signHeaderPrefixList)
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

HTTPUTILLIB_API string STDCALL HttpPost(string url, map<string, string> headers, const char *body, unsigned int bodySize, string appKey, string appSecret, int timeout, list<string> signHeaderPrefixList)
{
	if (NULL == body)
	{
		return "";
	}

	std::string strBody = std::string(body, bodySize);
	if (!strBody.empty())
	{
		std::string strKey = "CONTENT-MD5";
		if (!FindTheKey(strKey, headers))
		{
			std::string strMd5Base64 = md5_base64_encode(strBody);
			headers.insert(std::make_pair(HttpHeader::HTTP_HEADER_CONTENT_MD5, strMd5Base64));
		}
	}

	map<string, string> formParam;
	headers = initialBasicHeader(headers, appKey, appSecret, "POST", url, formParam, signHeaderPrefixList);

	SimpleHttpClient httpClient("POST", url.c_str(), timeout);
	for (map<string, string>::iterator it = headers.begin(); it != headers.end(); it++)
	{
		httpClient.setHttpHeader(it->first, it->second);
	}

	httpClient.setHttpContent(strBody);
	httpClient.sendHttpRequest();
	return httpClient.getHttpResponseBody();
}

HTTPUTILLIB_API string STDCALL HttpPost(string url, map<string, string> headers, map<string, string> bodys, string appKey, string appSecret, int timeout, list<string> signHeaderPrefixList)
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

HTTPUTILLIB_API string STDCALL HttpPost(string url, map<string, string> headers, string body, string appKey, string appSecret, int timeout, list<string> signHeaderPrefixList)
{
	map<string, string> formParam;

	if (!body.empty())
	{
		std::string strKey = "CONTENT-MD5";
		if (!FindTheKey(strKey, headers))
		{
			std::string strMd5Base64 = md5_base64_encode(body);
			headers.insert(std::make_pair(HttpHeader::HTTP_HEADER_CONTENT_MD5, strMd5Base64));
		}
	}

	headers = initialBasicHeader(headers, appKey, appSecret, "GET", url, formParam, signHeaderPrefixList);	

	SimpleHttpClient httpClient("POST", url.c_str(), timeout);
	for (map<string, string>::iterator it = headers.begin(); it != headers.end(); it++)
	{
		httpClient.setHttpHeader(it->first, it->second);
	}
	httpClient.setHttpContent(body);
	httpClient.sendHttpRequest();
	return httpClient.getHttpResponseBody();
}

HTTPUTILLIB_API string STDCALL HttpPut(string url, map<string, string> headers, const char *body, unsigned int bodySize, string appKey, string appSecret, int timeout, list<string> signHeaderPrefixList)
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

HTTPUTILLIB_API string STDCALL HttpDelete(string url, map<string, string> headers, string appKey, string appSecret, int timeout, list<string> signHeaderPrefixList)
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

HTTPUTILLIB_API map<string, string> STDCALL initialBasicHeader(map<string, string> &headers, string appKey, string appSecret, string method, string requestAddress, map<string, string> formParam, list<string> signHeaderPrefixList)
{
	SignUtil signUtil;	

	headers.insert(std::make_pair(SystemHeader::X_CA_TIMESTAMP, WinUtil::GetMsTime()));
	headers.insert(std::make_pair(SystemHeader::X_CA_NONCE, WinUtil::GetUuidString()));
	headers.insert(std::make_pair(SystemHeader::X_CA_KEY, appKey));
	headers.insert(std::make_pair(SystemHeader::X_CA_SIGNATURE, signUtil.Sign(method, requestAddress, headers, formParam, appSecret, signHeaderPrefixList)));

	return headers;
}
