#include "SignUtil.h"
#include "constants.h"
#include "HttpHeader.h"
#include "SystemHeader.h"
#include "algorithm_hmac.h"
#include "WinUtil.h"
#include <utility>
#include "algorithm"

SignUtil::SignUtil()
{
	
}

/**
 * 计算签名
 *
 * @param method               HttpMethod
 * @param url                  Path+Query
 * @param headers              Http头
 * @param formParamMap         POST表单参数
 * @param secret               APP密钥
 * @param signHeaderPrefixList 自定义参与签名Header前缀，符合此前缀的header才会签名
 * @return 签名后的字符串
 */
string SignUtil::Sign(string method, string url, map<string, string> &headers, map<string, string> formParamMap, string secret, list<string> signHeaderPrefixList)
{
	string result;
	string stringToSign = BuildStringToSign(headers, url, formParamMap, method, signHeaderPrefixList);

	unsigned char *signedByte = (unsigned char *)malloc(64);
	memset(signedByte, 0, 64);
	char *strBase64 = (char *)malloc(64);
	memset(strBase64, 0, 64);
	
	unsigned int strSignedLen = 0;

	HmacEncode("SHA256", secret, stringToSign, signedByte, strSignedLen);
	Base64Encode(signedByte, strSignedLen, strBase64, 64);
	
	result = strBase64;

	//去除所有换行符
	std::string::size_type pos = result.find('\n');
	while(pos != std::string::npos)
	{
		result.erase(pos, 1);
		pos = result.find('\n');
	}

	free(signedByte);
	free(strBase64);
	return result;
}

/**
 * 构建待签名字符串
 *
 * @param headers              Http头
 * @param url                  Path+Query
 * @param formParamMap         POST表单参数
 * @param method
 * @param signHeaderPrefixList 自定义参与签名Header前缀
 * @return 签名字符串
 */
string SignUtil::BuildStringToSign(map<string, string> &headers, string url, map<string, string> formParamMap, string method, list<string> signHeaderPrefixList)
{
	string strToSign;
	transform(method.begin(), method.end(), method.begin(), toupper);
	strToSign.append(method);
	strToSign.append(Constants::LF);
	if (headers.find(HttpHeader::HTTP_HEADER_ACCEPT) != headers.end()) 
	{
		strToSign.append(headers[HttpHeader::HTTP_HEADER_ACCEPT]);
		strToSign.append(Constants::LF);
	}
	if (headers.find(HttpHeader::HTTP_HEADER_CONTENT_MD5) != headers.end()) 
	{
		strToSign.append(headers[HttpHeader::HTTP_HEADER_CONTENT_MD5]);
		strToSign.append(Constants::LF);
	}
	if (headers.find(HttpHeader::HTTP_HEADER_CONTENT_TYPE) != headers.end()) 
	{
		strToSign.append(headers[HttpHeader::HTTP_HEADER_CONTENT_TYPE]);
		strToSign.append(Constants::LF);
	}
	if (headers.find(HttpHeader::HTTP_HEADER_DATE) != headers.end()) 
	{
		strToSign.append(headers[HttpHeader::HTTP_HEADER_DATE]);
		strToSign.append(Constants::LF);
	}
	strToSign.append(BuildHeaders(headers, signHeaderPrefixList));
	strToSign.append(BuildResource(url, formParamMap));

	return strToSign;
}

/**
 * 构建待签名Path+Query+FormParams
 *
 * @param url          Path+Query，如http://10.6.131.112:9999/artemis/findControlUnitByUnitCode?userName=test3&unitCode=0
 * @param formParamMap POST表单参数
 * @return 待签名Path+Query+FormParams
 */
string SignUtil::BuildResource(string url, map<string, string> formParamMap)
{
	//去掉 http://10.6.131.112:9999
	std::string::size_type pos = static_cast<int>(url.find("://"));
	if(pos != string::npos)
	{
		url = url.substr(pos + 3);
	}
	pos = static_cast<int>(url.find("/"));
	if(pos != string::npos)
	{
		url = url.substr(pos);
	}

	string path = url;
	string queryString;
	//query 参数放入formParamMap中一起按字典排序
	if(url.find("?") != string::npos)
	{
		path = url.substr(0, url.find_first_of("?"));
		queryString = url.substr(url.find_first_of("?")+1);

		if(!queryString.empty())
		{
			vector<string> vtQueryStr;
			WinUtil::split(queryString, "&", vtQueryStr);
			for(unsigned int i = 0; i < vtQueryStr.size(); i++)
			{
				if(vtQueryStr[i].find("=") != string::npos)
				{
					string key = vtQueryStr[i].substr(0, vtQueryStr[i].find_first_of("="));
					string value = "";
					if(vtQueryStr[i].find_first_of("=") == vtQueryStr[i].find_last_of("="))
					{
						value = vtQueryStr[i].substr(vtQueryStr[i].find_first_of("=") + 1);
					}

					if(formParamMap.find(key) == formParamMap.end())
					{
						formParamMap.insert(std::make_pair(key, value));
					}
				}
			}
		}
	}

	string sb;
	sb.append(path);

	if(formParamMap.size() > 0)
	{
		sb.append("?");
		
		int flag = 0;
		for(map<string, string>::iterator it = formParamMap.begin(); it != formParamMap.end(); it++)
		{
			if(flag != 0)
			{
				sb.append("&");
			}
			flag++;

			string key = it->first;
			string val = it->second;

			if(!key.empty())
			{
				sb.append(key);
			}
			if(!val.empty())
			{
				sb.append("=").append(val);
			}
		}
	}

	return sb;
}

/**
 * 构建待签名Http头
 *
 * @param headers              请求中所有的Http头
 * @param signHeaderPrefixList 自定义参与签名Header前缀
 * @return 待签名Http头
 */
string SignUtil::BuildHeaders(map<string, string> &headers, list<string> signHeaderPrefixList)
{
	map<string, string> headersToSign;

	string signHeadersString;
	string sb;

	signHeaderPrefixList.remove(SystemHeader::X_CA_SIGNATURE);
	signHeaderPrefixList.remove(HttpHeader::HTTP_HEADER_ACCEPT);
	signHeaderPrefixList.remove(HttpHeader::HTTP_HEADER_CONTENT_MD5);
	signHeaderPrefixList.remove(HttpHeader::HTTP_HEADER_CONTENT_TYPE);
	signHeaderPrefixList.remove(HttpHeader::HTTP_HEADER_DATE);

	for(map<string, string>::iterator it = headers.begin(); it != headers.end(); it++)
	{
		if(IsHeaderToSign(it->first, signHeaderPrefixList))
		{
			sb.append(it->first);
			sb.append(Constants::SPE2);
			if(!it->second.empty())
			{
				sb.append(it->second);
			}
			sb.append(Constants::LF);

			if(!signHeadersString.empty())
			{
				signHeadersString.append(",");
			}
			signHeadersString.append(it->first);
		}
	}

	headers.insert(std::make_pair(SystemHeader::X_CA_SIGNATURE_HEADERS, signHeadersString));

	return sb;
}

/**
 * Http头是否参与签名
 * return
 */
bool SignUtil::IsHeaderToSign(string headerName, list<string> signHeaderPrefixList)
{
	if(headerName.empty())
	{
		return false;
	}
	
	if(headerName.find_first_of(Constants::CA_HEADER_TO_SIGN_PREFIX_SYSTEM) == 0)
	{
		return true;
	}

	list<string>::iterator it = signHeaderPrefixList.begin();
	for(; it != signHeaderPrefixList.end(); it++)
	{
		if(headerName.find_first_of(Constants::CA_HEADER_TO_SIGN_PREFIX_SYSTEM) == 0)
		{
			return true;
		}
	}

	return false;
}
