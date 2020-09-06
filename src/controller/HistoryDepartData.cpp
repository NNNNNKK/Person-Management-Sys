#include "HistoryDepartData.h"
#include<QFile>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>
#include<QCoreApplication>
#include"datadeal/hikdatarequest.h"
HistoryDepartData::HistoryDepartData(QObject *parent) : HttpRequestHandler(parent)
{
}

void HistoryDepartData::service(HttpRequest &request, HttpResponse &response)
{   
    int page=request.getParameter("page").toInt();
    int limitnumber=request.getParameter("limit").toInt();
	int backislocal = request.getParameter("islocal").toInt();
	QString date = request.getParameter("date");
	QString cdate = request.getParameter("cdate");
	QString sdate;
	if (date == QString())
		sdate = cdate;
	else
		sdate = date;
    QJsonArray mainArray;
    int  count=0;
	QJsonObject orgObj = DataStore::getOrgnizeData();//打开json文档
	QJsonArray selectOrg;//筛选与需要的一样的
	for (auto var : orgObj)
	{ 
		QJsonObject son = var.toObject();
		int islocalflag = son["islocal"].toBool();
		if (backislocal == islocalflag)
		{
           selectOrg.append(son);
		}	
	}
	QJsonObject res;
	int totalHours = HikdataRequest::contractHours(sdate);
	for (int i = (page - 1)*limitnumber; i < (page*limitnumber< selectOrg.count()? page * limitnumber: selectOrg.count()); i ++)
	{
          QJsonObject sub,varObj= selectOrg.at(i).toObject();
		  sub["departId"] = varObj["orgIndexCode"];
		  sub["departName"] = varObj["orgName"];
		  sub["contractorPersonNumber"] = varObj["orgEmployeesNumber"];
		  sub["totalHours"] = totalHours * 8 * varObj["orgEmployeesNumber"].toInt();//计算合同工作时
		  sub["attendanceNumber"] = varObj["employeelist"].toArray().size();
		  mainArray.append(sub);
	}
    QJsonDocument jsondoc;
    response.setHeader("Content-Type", "application/json; charset=UTF-8");
	res["code"]=0;
	res["count"]= selectOrg.size();
	res["data"]=mainArray;
    if(count>0)
		res["msg"]="get history data ok";
    else
		res["msg"]="no history data";
    jsondoc.setObject(res);
    response.write(jsondoc.toJson());
}
