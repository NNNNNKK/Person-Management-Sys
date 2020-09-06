#include "WinUtil.h"
#include "sys/timeb.h"
#include "sstream"

WinUtil::WinUtil(void)
{
}

WinUtil::~WinUtil(void)
{
}

string WinUtil::GetUuidString(void)
{
	UUID new_uuid;
	char *stringUuid;
	UuidCreate(&new_uuid);
	UuidToString(&new_uuid, (RPC_CSTR*)&stringUuid);
	string ret(stringUuid);
	RpcStringFreeA((RPC_CSTR*)&stringUuid);
	
	return ret;
}

void WinUtil::split(string s, string delimiter, vector<string >& ret)  
{  
	int last = 0;  
	std::string::size_type index = s.find_first_of(delimiter,last);
	while (index != string::npos)  
	{  
		ret.push_back(s.substr(last,index-last));  
		last = static_cast<int>(index ) + 1;
		index = s.find_first_of(delimiter,last);  
	}
	if (static_cast<int>(s.size()) > last)
	{  
		ret.push_back(s.substr(last,index-last));  
	}  
} 

string WinUtil::GetMsTime()
{
	std::stringstream ss;
	struct timeb t;
	ftime(&t);
	ss << t.time*1000 << t.millitm;
	return ss.str();
}
