
#include "simple_http_client.h"

SimpleHttpClient::SimpleHttpClient(const char* method, const char *url, int timeout/*= 5*/)
     :m_httpHandle(NULL)
     ,m_httpHeaders(NULL)
     ,m_pRedirectHeaders(NULL)
{
	m_httpHandle = curl_easy_init();
	if (m_httpHandle)
	{
		do 
		{
			if (curl_easy_setopt(m_httpHandle, CURLOPT_TIMEOUT, timeout)!=CURLE_OK)
			{
				break;
			}

            if(std::string("POST") == std::string(method))
            {
                if (curl_easy_setopt(m_httpHandle, CURLOPT_POST, 1L)!=CURLE_OK)
			    {
				    break;
			    }
            }
            else if (std::string("GET") == std::string(method))
            {
                if (curl_easy_setopt(m_httpHandle, CURLOPT_CUSTOMREQUEST, method)!=CURLE_OK)
                {
                    break;
                }
            }
            else if (std::string("PUT") == std::string(method))
            {
                if (curl_easy_setopt(m_httpHandle, CURLOPT_PUT, 1L)!=CURLE_OK)
                {
                    break;
                }
            }
            else if (std::string("DELETE") == std::string(method))
            {
                if (curl_easy_setopt(m_httpHandle, CURLOPT_CUSTOMREQUEST, method)!=CURLE_OK)
                {
                    break;
                }
            }
            /*else if ("OPTIONS" == method)
            {
                if (curl_easy_setopt(m_httpHandle, CURLOPT_HTTPGET, 1L)!=CURLE_OK)
                {
                    break;
                }
            }
            else if ("CONNECT" == method)
            {
                if (curl_easy_setopt(m_httpHandle, CURLOPT_HTTPGET, 1L)!=CURLE_OK)
                {
                    break;
                }
            }*/
            else if (std::string("HEAD") == std::string(method))
            {
                if (curl_easy_setopt(m_httpHandle, CURLOPT_CUSTOMREQUEST, method)!=CURLE_OK)
                {
                    break;
                }
            }
            /*else if ("TRANCE" == method)
            {
                if (curl_easy_setopt(m_httpHandle, CURLOPT_HTTPGET, 1L)!=CURLE_OK)
                {
                    break;
                }
            }*/
            else
            {
                break;
            }

			if (curl_easy_setopt(m_httpHandle, CURLOPT_URL, url)!=CURLE_OK)
			{
				break;
			}

			return;
		} while (false);

		doClear();
		throw std::runtime_error("initial http client error."); 
	}
}

SimpleHttpClient::~SimpleHttpClient(void)
{
	 doClear(); //lint !e1551 
}

bool SimpleHttpClient::initialHttpClient()
{
    return curl_global_init(CURL_GLOBAL_DEFAULT) == CURLE_OK;
}

void SimpleHttpClient::finishHttpClient()
{
    curl_global_cleanup();
}

void SimpleHttpClient::doClear()
{
    if (m_httpHeaders)
    {
        curl_slist_free_all(m_httpHeaders);
        m_httpHeaders = NULL;
    }

    if (m_pRedirectHeaders)
    {
        curl_slist_free_all(m_pRedirectHeaders);
        m_pRedirectHeaders=NULL;
    }

    if (m_httpHandle) 
    {
        curl_easy_cleanup(m_httpHandle);
        m_httpHandle = NULL;
    }
}

bool SimpleHttpClient::setHttpHeader( const std::string &httpHeader, const std::string &httpValue )
{
    std::string headerLine =  httpHeader + ": " + httpValue;
    m_httpHeaders = curl_slist_append(m_httpHeaders, headerLine.c_str());
    return curl_easy_setopt(m_httpHandle, CURLOPT_HTTPHEADER, m_httpHeaders) == CURLE_OK;
}

bool SimpleHttpClient::setHttpContent( const std::string &body )
{

    if ( CURLE_OK != curl_easy_setopt(m_httpHandle, CURLOPT_POSTFIELDS, body.c_str()) )
    {
        return false;
    }

    if ( CURLE_OK != curl_easy_setopt(m_httpHandle, CURLOPT_POSTFIELDSIZE, body.length()) )
    {
        return false;
    }
    return true;
}

bool SimpleHttpClient::setHttpCookie(const std::string &cookie)
{
	if (CURLE_OK != curl_easy_setopt(m_httpHandle, CURLOPT_COOKIE, cookie.c_str()))
	{
		return false;
	}
	return true;
}

size_t SimpleHttpClient::recvData( char *pbuffer, size_t size, size_t nmemb, std::string *strRsp )
{
    if (strRsp == NULL)
    {
        return 0;
    }
    strRsp->append(pbuffer, size * nmemb);
    return size*nmemb;
}

bool SimpleHttpClient::sendHttpRequest(bool bGetHeader, bool bSSL)
{
    if ( CURLE_OK != curl_easy_setopt(m_httpHandle, CURLOPT_WRITEFUNCTION, &recvData) )
    {
        return false;
    }

    if ( CURLE_OK != curl_easy_setopt(m_httpHandle, CURLOPT_WRITEDATA, &m_httpCotent) )
    {
        return false;
    }

    if ( CURLE_OK != curl_easy_setopt(m_httpHandle, CURLOPT_NOSIGNAL, 1L) ) 
    {
        return false;
    }

	//设置请求中允许重定向请求
	if ( CURLE_OK != curl_easy_setopt(m_httpHandle, CURLOPT_FOLLOWLOCATION, true))
    {
        //LOG_ERROR("curl_easy_setopt[CURLOPT_FOLLOWLOCATION]");
        return false;
    }

	if (bGetHeader && CURLE_OK != curl_easy_setopt(m_httpHandle, CURLOPT_HEADER, 1L))
	{
		return false;  
	}
	
	if (bSSL && CURLE_OK != curl_easy_setopt(m_httpHandle, CURLOPT_SSL_VERIFYPEER, 0L))
	{
		return false;
	}

	if (bSSL && CURLE_OK != curl_easy_setopt(m_httpHandle, CURLOPT_SSL_VERIFYHOST, 0L))
	{
		return false;
	}

	//设置重定向后的请求头
    if (bSSL && CURLE_OK != curl_easy_setopt(m_httpHandle, CURLOPT_HEADERFUNCTION, &header_callback))
    {
        return false;
    }

    if (bSSL && CURLE_OK != curl_easy_setopt(m_httpHandle, CURLOPT_HEADERDATA, this))
    {
        return false;
    }
    if (!setHttpRedirectHeader())
    {
        return false;
    }

    if ( CURLE_OK != curl_easy_perform(m_httpHandle) )
    {
        return false;
    }
    //NOTE: curl_easy_getinfo must pass a pointer to long!!! see curl_easy_getinfo AVAILABLE INFORMATION:
    
    /*
    CURLINFO_RESPONSE_CODE 

    Pass a pointer to a long to receive the last received HTTP, FTP or SMTP response code. This option was previously known as CURLINFO_HTTP_CODE in libcurl 7.10.7 and earlier. The value will be zero if no server response code has been received. Note that a proxy's CONNECT response should be read with CURLINFO_HTTP_CONNECTCODE and not this.

    Support for SMTP responses added in 7.25.0. 

    */
    long http_code = -1;
    curl_easy_getinfo(m_httpHandle, CURLINFO_RESPONSE_CODE, &http_code);

    return  (http_code == 200);
}

//重定向请求,返回请求头信息
bool SimpleHttpClient::sendHttpRequestRedir( bool bGetHeader, bool bSSL)
{

    if ( CURLE_OK != curl_easy_setopt(m_httpHandle, CURLOPT_FOLLOWLOCATION, true))
    {
        return false;
    }

      if ( CURLE_OK != curl_easy_setopt(m_httpHandle, CURLOPT_WRITEFUNCTION, &recvData) )
    {
        return false;
    }

    if ( CURLE_OK != curl_easy_setopt(m_httpHandle, CURLOPT_WRITEDATA, &m_httpCotent) )
    {
        return false;
    }

    if ( CURLE_OK != curl_easy_setopt(m_httpHandle, CURLOPT_NOSIGNAL, 1L) ) 
    {
        return false;
    }

	if (bGetHeader && CURLE_OK != curl_easy_setopt(m_httpHandle, CURLOPT_HEADER, 1L))
	{
		return false;  
	}
	
	if (bSSL && CURLE_OK != curl_easy_setopt(m_httpHandle, CURLOPT_SSL_VERIFYPEER, 0L))
	{
		return false;
	}

	if (bSSL && CURLE_OK != curl_easy_setopt(m_httpHandle, CURLOPT_SSL_VERIFYHOST, 0L))
	{
		return false;
	}

    if (bSSL && CURLE_OK != curl_easy_setopt(m_httpHandle, CURLOPT_HEADERFUNCTION, &header_callback))
    {
        return false;
    }

    if (bSSL && CURLE_OK != curl_easy_setopt(m_httpHandle, CURLOPT_HEADERDATA, this))
    {
        return false;
    }
    if (!setHttpRedirectHeader())
    {
        return false;
    }


    //NOTE: curl_easy_getinfo must pass a pointer to long!!! see curl_easy_getinfo AVAILABLE INFORMATION:
    
    /*
    CURLINFO_RESPONSE_CODE 

    Pass a pointer to a long to receive the last received HTTP, FTP or SMTP response code. This option was previously known as CURLINFO_HTTP_CODE in libcurl 7.10.7 and earlier. The value will be zero if no server response code has been received. Note that a proxy's CONNECT response should be read with CURLINFO_HTTP_CONNECTCODE and not this.

    Support for SMTP responses added in 7.25.0. 

    */

    if ( CURLE_OK != curl_easy_perform(m_httpHandle) )
    {
        return false;
    }
    long http_code = -1;
    curl_easy_getinfo(m_httpHandle, CURLINFO_RESPONSE_CODE, &http_code);

    
    return  (http_code == 200);
}


bool SimpleHttpClient::setHttpMultipartForm(map<string,string> bodys)
{
    curl_httppost *post = NULL;
    curl_httppost *last = NULL;
    // 文本数据
    for(map<string,string>::iterator it = bodys.begin(); it != bodys.end(); it++)
    {
        curl_formadd(&post, &last, CURLFORM_COPYNAME, it->first.c_str(), CURLFORM_COPYCONTENTS, it->second.c_str(), CURLFORM_END);;
    }

    (void)curl_easy_setopt(m_httpHandle, CURLOPT_HTTPPOST, post);
    return true;
}

void SimpleHttpClient::setHttpUploadData(const char *body, unsigned int bodySize)
{
    //m_httpPutData.assign(body, body+bodySize);
}


size_t SimpleHttpClient::header_callback(char *buffer,   size_t size,   size_t nitems,   void *userdata)
{
    SimpleHttpClient* pUser=reinterpret_cast<SimpleHttpClient*>(userdata);
    if (buffer == NULL || pUser == NULL || buffer[0] == '\0')
    {
        return 0;
    }
    // 响应头数据
    char* pData=(char *)buffer;
    pUser->m_strHeader.append(pData,size*nitems);
    if (NULL != pUser->m_httpHandle)
    {
        //重置重定向后的headers
        if (CURLE_OK != curl_easy_setopt(pUser->m_httpHandle, CURLOPT_HTTPHEADER, pUser->m_pRedirectHeaders))
        {
            return 0;
        }
    }

    return size*nitems;
}

//重定向的请求头更新
 bool SimpleHttpClient::setHttpRedirectHeader()
 {
     std::string strheader="Cache-Control: no-cache";
     m_pRedirectHeaders=curl_slist_append(m_pRedirectHeaders,strheader.c_str());
     strheader="Connection: keep-Alive";
     m_pRedirectHeaders=curl_slist_append(m_pRedirectHeaders,strheader.c_str());
     strheader="Accept: ";
     m_pRedirectHeaders=curl_slist_append(m_pRedirectHeaders,strheader.c_str());
     strheader="Content-Type: ";
     m_pRedirectHeaders=curl_slist_append(m_pRedirectHeaders,strheader.c_str());
     strheader="x-ca-key: ";
     m_pRedirectHeaders=curl_slist_append(m_pRedirectHeaders,strheader.c_str());
     strheader="x-ca-nonce: ";
     m_pRedirectHeaders=curl_slist_append(m_pRedirectHeaders,strheader.c_str());
     return true;
 }



