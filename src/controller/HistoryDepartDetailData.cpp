#include "HistoryDepartDetailData.h"
#include<QFile>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>
#include<QCoreApplication>
#include<QDate>
#include<QtConcurrent>
#include<QFuture>
#include<QThread>
#include<QDateTime>
#include"datadeal/hikdatarequest.h"
HistoryDepartDetailData::HistoryDepartDetailData(QObject *parent) : HttpRequestHandler(parent)
{
}
void HistoryDepartDetailData::service(HttpRequest &request, HttpResponse &response)
{
	QString requestType = request.getParameter("type");
    int page = request.getParameter("page").toInt();
	int limit = 1000;// request.getParameter("limit").toInt();
	QString dept_id = request.getParameter("deptid");
	QString date = request.getParameter("date");
	QJsonDocument jsondoc;
	if (requestType == tr("odetail"))
	   jsondoc = backData(page, limit, dept_id, date);
	if (requestType == tr("pdetail"))
	{
		QString personid = request.getParameter("personid");
		jsondoc=backtyData(page, limit, dept_id, personid, date);
	}
	if (requestType == tr("ndetail"))
	{
		QString filename = QCoreApplication::applicationDirPath() + "/Data/HistoryData/" + date + "/" + dept_id + ".data";
		QJsonObject detailMsg = DataStore::openJsonDocument(filename)[dept_id].toObject();//读取数据
		detailMsg.remove("data");
		jsondoc.setObject(detailMsg);
	}
	response.setHeader("Content-Type", "application/json; charset=UTF-8");
	response.write(jsondoc.toJson());
}

QJsonDocument HistoryDepartDetailData::backData(int&page, int&limit, QString &dept_id, QString &date)
{
	QDateTime current = QDateTime::currentDateTime();//当前时间
	QString cdate = QDate::currentDate().toString("yyyyMM");
	QString filename = QCoreApplication::applicationDirPath() + "/Data/HistoryData/" + date +"/"+ dept_id+".data";
	QJsonObject subobject; 
	subobject["count"] = 0;
	QJsonObject detailMsg = DataStore::openJsonDocument(filename)[dept_id].toObject();//读取数据
	//判断是否为当月数据
	if (date == cdate)
	{
		//如果没有该数据,则执行查询操作
		
		if (detailMsg.count()==0)
		{
			HikdataRequest().historyDataDeal(date, dept_id);
		}
		else
		{
			QDateTime updatetime = QDateTime::fromString(detailMsg["updatetime"].toString(), "yyyy-MM-ddThh:mm:ss");
			if (updatetime.secsTo(current) > 600)
				HikdataRequest().historyDataDeal(date, dept_id);
		}
	}
	//时间不是当前月份
	else
	{
		if (detailMsg.size()==0)
			HikdataRequest().historyDataDeal(date, dept_id);
	}
	detailMsg = DataStore::openJsonDocument(filename)[dept_id].toObject();
	int totalCount = detailMsg["count"].toInt();//数据总数
	QJsonArray data,readData=detailMsg["data"].toArray();
	for (int i = (page - 1)*limit; i < (page*limit < totalCount ? page * limit : totalCount); i++)
	{
		data.append(readData.at(i).toObject());
	}
	QJsonDocument jsondoc;
	subobject["code"] = 0;
	subobject["data"] = data;
	subobject["deptid"] = dept_id;
	subobject["actualWkT"] = detailMsg["actualWkT"];
	subobject["attendanceRatio"] = detailMsg["attendanceRatio"];
	subobject["turnoverRate"] = detailMsg["turnoverRate"];
	subobject["orgNmae"] = detailMsg["orgName"];
	subobject["msg"] = "sucess";
	jsondoc.setObject(subobject);
	return jsondoc;
}

QJsonDocument HistoryDepartDetailData::backtyData(int & page, int & limit, QString &dept_id,QString & personid, QString &date)
{
	QString filename = QCoreApplication::applicationDirPath() + "/Data/HistoryData/" + date + "/" + dept_id + ".data";
	QJsonObject readRoot= DataStore::openJsonDocument(filename)[dept_id].toObject();//打开文件，读取文件类容
	QJsonArray readArray = readRoot["data"].toArray();
	QJsonObject subObj;
	subObj["code"] = 0;
	subObj["count"] = 0;
	subObj["data"] = QJsonArray();
	subObj["msg"] = "success";
	for (auto var : readArray)
	{
		QJsonObject sub = var.toObject();
		if (sub["personId"].toString() == personid)
		{
			  subObj["data"] = sub["detail"].toArray();
			  subObj["count"] = sub["datail"].toArray().count();
			  break;
		}
	}
	QJsonDocument jsondoc;
	jsondoc.setObject(subObj);
	return jsondoc;
}

