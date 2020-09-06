#include "HttpUtil.h"
#include "iostream"
using namespace std;

void TestHttpPostString();
void TestHttpGet();

//int main()
//{
//	//TestHttpGet();
//	TestHttpPostString();
//	return 0;
//}

void TestHttpGet()
{
	string appkey = "21875141";
	string appsecret = "EXz3KxGmxyAoC4Ny6ahl";

	map<string, string> headers;
	//（必填的Http头）根据期望的Response内容类型设置
	headers.insert(std::make_pair(HttpHeader::HTTP_HEADER_ACCEPT, "application/json"));

	map<string, string> formParams;
	list<string> signHeaderPrefixList;

	string url = "http://10.6.131.112:9999/artemis/videoParam?cameraIndexCode=11000000001310013761&netZoneId=100001";
	
	string http_response = HttpUtil::HttpGet(url, headers, appkey, appsecret, 30, signHeaderPrefixList);

	cout << http_response << endl;
}

void TestHttpPostString()
{
	string appkey = "21875141";
	string appsecret = "EXz3KxGmxyAoC4Ny6ahl";

	map<string, string> headers;
	//（必填的Http头）根据期望的Response内容类型设置
	headers.insert(std::make_pair(HttpHeader::HTTP_HEADER_ACCEPT, "application/json"));
	headers.insert(std::make_pair(HttpHeader::HTTP_HEADER_CONTENT_TYPE, "application/json;charset=UTF-8"));

	list<string> signHeaderPrefixList;

	string url = "http://10.6.131.112:9999/artemis/saveOperationLog";
	string body = "{\"userId\":\"10113322\", \"ip\":\"10.11.33.22\", \"act\":\"Login\", \"content\":\"Login\"}";
	string http_response = HttpUtil::HttpPost(url, headers, body, appkey, appsecret, 30, signHeaderPrefixList);

	cout << http_response << endl;
}