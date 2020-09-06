#pragma once
#include "http/simple_http_client.h"
#include "SignUtil.h"
#include "HttpHeader.h"
#include "SystemHeader.h"
#include "constants.h"
#include "WinUtil.h"
#include "string"
#include "map"
#include "list"
using std::string;
using std::map;
using std::list;

class HttpUtil
{
public:
	HttpUtil();
	~HttpUtil();
	
	 /**
     * HTTP GET
     *
     * @param url                  http://host+path+query
     * @param headers              Httpͷ
     * @param appKey               APP KEY
     * @param appSecret            APP��Կ
     * @param timeout              ��ʱʱ�䣨�룩
     * @param signHeaderPrefixList �Զ������ǩ��Headerǰ׺
     * @return ���ý��
     */
     static string HttpGet(string url, map<string, string> headers, string appKey, string appSecret, int timeout, list<string> signHeaderPrefixList);

     /**
     * Http POST �ַ���
     *
     * @param url                  http://host+path+query
     * @param headers              Httpͷ
     * @param body                 �ַ���������
     * @param appKey               APP KEY
     * @param appSecret            APP��Կ
     * @param timeout              ��ʱʱ�䣨�룩
     * @param signHeaderPrefixList �Զ������ǩ��Headerǰ׺
     * @return ���ý��
     */
	 static string HttpPost(string url, map<string, string> headers, string body, string appKey, string appSecret, int timeout, list<string> signHeaderPrefixList);

	 /**
     * Http POST �ֽ�����
     *
     * @param url                  http://host+path+query
     * @param headers              Httpͷ
     * @param body                 �ֽ�����
	 * @param bodySize             �ֽ�����Ĵ�С
     * @param appKey               APP KEY
     * @param appSecret            APP��Կ
     * @param timeout              ��ʱʱ�䣨�룩
     * @param signHeaderPrefixList �Զ������ǩ��Headerǰ׺
     * @return ���ý��
     */
	 static string HttpPost(string url, map<string, string> headers, const char *body, unsigned int bodySize, string appKey, string appSecret, int timeout, list<string> signHeaderPrefixList);
	
	 /**
     * Http POST ��
     *
     * @param url                  http://host+path+query
     * @param headers              Httpͷ
     * @param bodys                ������
     * @param appKey               APP KEY
     * @param appSecret            APP��Կ
     * @param timeout              ��ʱʱ�䣨�룩
     * @param signHeaderPrefixList �Զ������ǩ��Headerǰ׺
     * @return ���ý��
     */
	 static string HttpPost(string url, map<string, string> headers, map<string, string> bodys, string appKey, string appSecret, int timeout, list<string> signHeaderPrefixList);

	 /**
     * Http PUT
     *
     * @param url                  http://host+path+query
     * @param headers              Httpͷ
     * @param body                 ���ϴ�����
     * @param bodySize             ���ݴ�С
     * @param appKey               APP KEY
     * @param appSecret            APP��Կ
     * @param timeout              ��ʱʱ�䣨�룩
     * @param signHeaderPrefixList �Զ������ǩ��Headerǰ׺
     * @return ���ý��
     */
	 string HttpPut(string url, map<string, string> headers, const char *body, unsigned int bodySize, string appKey, string appSecret, int timeout, list<string> signHeaderPrefixList);
	 
     /**
     * Http DELETE
     *
     * @param url                  http://host+path+query
     * @param headers              Httpͷ
     * @param appKey               APP KEY
     * @param appSecret            APP��Կ
     * @param timeout              ��ʱʱ�䣨�룩
     * @param signHeaderPrefixList �Զ������ǩ��Headerǰ׺
     * @return ���ý��
     */
	 string HttpDelete(string url, map<string, string> headers, string appKey, string appSecret, int timeout, list<string> signHeaderPrefixList);
private:

	/**
     * ��ʼ������Header
     *
     * @param headers              Httpͷ
     * @param appKey               APP KEY
     * @param appSecret            APP��Կ
     * @param method               Http����
     * @param requestAddress       http://host+path+query
     * @param formParam            ������
     * @param signHeaderPrefixList �Զ������ǩ��Headerǰ׺
     * @return ����Header
     */
	 static map<string, string> initialBasicHeader(map<string, string> &headers, string appKey, string appSecret, string method, string requestAddress, map<string, string> formParam, list<string> signHeaderPrefixList);
};