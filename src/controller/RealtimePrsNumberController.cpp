#include "RealtimePrsNumberController.h"
#include"GloableLocker.h"
#include <DataStore.h>
RealtimePrsNumberController::RealtimePrsNumberController(QObject *parent) : HttpRequestHandler(parent)
{
}
void RealtimePrsNumberController::service(HttpRequest &request, HttpResponse &response)
{
       Q_UNUSED(request)
       QJsonObject subobject=QJsonObject();
       subobject["msg"] = "sucess";
       QJsonObject ob = DataStore::getRealTimeData();//打开文件
	   QJsonObject org = DataStore::getOrgnizeData();
       int totalNumber=0,localNumber=0,otherNumber=0;
	   for (auto orgindex : ob.keys())
	   {
		   if (ob[orgindex].isObject())
		   {
              QJsonObject subObj= ob[orgindex].toObject();
			  int rnumber = subObj["realnumber"].toInt(); 
			  QJsonObject obj = org[orgindex].toObject();
			  int contractor = obj["orgEmployeesNumber"].toInt();
			  if (obj["islocal"].toBool() == true)
				  localNumber += rnumber;
			  else
				  otherNumber += rnumber;
		   }   
	   }
	   totalNumber = localNumber + otherNumber;
      QJsonDocument jsonDoc;
      response.setHeader("Content-Type", "application/json; charset=UTF-8");
      subobject["code"]=0;
      subobject["total"]=totalNumber;//场内总人数
      subobject["otherNumber"]=otherNumber;//承包商总人数
      subobject["localNumber"]=localNumber;//本单位总人数
	  subobject["totalInToday"] = DataStore::getTotalToday();
      jsonDoc.setObject(subobject);
     response.write(jsonDoc.toJson());
}
