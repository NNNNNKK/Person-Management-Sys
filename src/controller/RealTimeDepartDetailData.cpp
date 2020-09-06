#include "realtimedepartdetaildata.h"
#include<QFile>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>
#include<QCoreApplication>
#include"GloableLocker.h"
RealtimeDepartDetailData::RealtimeDepartDetailData(QObject *parent) : HttpRequestHandler(parent)
{

}
void RealtimeDepartDetailData::service(HttpRequest &request, HttpResponse &response)
{
	QString dept_id = request.getParameter("depid");
	int page = request.getParameter("page").toInt();
	int limit = request.getParameter("limit").toInt();
	QJsonObject subObj = DataStore::getRealTimeData()[dept_id].toObject();
	QJsonArray data, responseMsg;
	for (auto var : subObj)
	{
		if (var.isObject())
		{
			if (var.toObject()["eventPlace"].toString() != QString("no_record"))
				data.append(var.toObject());
		}
	}
	QJsonObject writeObj;
	writeObj["code"] = 0;
	writeObj["msg"] = "suceess";
	for (int i = (page - 1)*limit; i < (page*limit < data.count() ? page * limit : data.count()); i++)
	{
		responseMsg.append(data.at(i));
	}
	writeObj["data"] = responseMsg;
	writeObj["count"] = data.size();
	response.setHeader("Content-Type", "application/json; charset=UTF-8");
	QJsonDocument jsondoc;
	jsondoc.setObject(writeObj);
	response.write(jsondoc.toJson());
}
