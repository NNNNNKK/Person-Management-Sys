#pragma once
#include <iostream>
#include <string>
#include "curl.h"
#include <stdexcept>
#include <string>
#include <map>
using namespace std;

class SimpleHttpClient
{
public:
	explicit SimpleHttpClient(const char* method, const char *url, int timeout = 15);

	virtual ~SimpleHttpClient();

	static bool initialHttpClient();

	static void finishHttpClient();

public:

	bool setHttpHeader(const std::string &httpHeader, const std::string &httpValue);

	bool setHttpContent(const std::string &body);

	bool setHttpCookie(const std::string &cookie);

	bool sendHttpRequest(bool bGetHeader = false, bool bSSL = true);

    bool sendHttpRequestRedir( bool bGetHeader = false, bool bSSL = true);     //重定向请求

	std::string getHttpResponseBody()
	{
		return m_httpCotent;
	}

    std::string getHttpHeader()
    {
        /*return Utf8ToGBK(m_strHeader.c_str());*/
		return m_strHeader;
    }

    bool setHttpMultipartForm(map<string,string> bodys);
    void setHttpUploadData(const char *body, unsigned int bodySize);

private:
	void doClear();

	static size_t recvData(char *pbuffer, size_t size, size_t nmemb, std::string *strRsp);
    static size_t header_callback(char *buffer,   size_t size,   size_t nitems,   void *userdata);
    bool setHttpRedirectHeader();
private:
	std::string     m_httpCotent;
	CURL           *m_httpHandle;
	curl_slist     *m_httpHeaders;

    std::string    m_strHeader;  //回调函数CURLOPT_HEADERFUNCTION返回的数据
    curl_slist     *m_pRedirectHeaders;   //重定向请求头数据

};


//typedef struct _CallBack_User_
//{
//    CURL* m_pCURL;
//    std::string m_strHeader;
//    std::string m_strBody;
//    curl_slist* m_pRedirectHeaders;
//    _CallBack_User_()
//    {
//        m_pCURL=NULL;
//        m_pRedirectHeaders=NULL;
//    }
//}CallBack_User,*PCallBack_User;
//
//static size_t header_callback(char *buffer,   size_t size,   size_t nitems,   void *userdata);
