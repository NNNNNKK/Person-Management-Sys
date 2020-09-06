#include "RealtimeDepartData.h"
#include<QFile>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>
#include<QCoreApplication>
#include"GloableLocker.h"
RealtimeDepartData::RealtimeDepartData(QObject *parent) : HttpRequestHandler(parent)
{
}
QJsonDocument backtype2Data()
{
	QJsonObject rtObj = DataStore::getRealTimeData();//打开json文档
	QJsonObject orgObj = DataStore::getOrgnizeData();
	QJsonArray selectOrg;
	int total = 0, local = 0, contractor = 0;
	for (auto orgindex : rtObj.keys())
	{
		QJsonObject son = rtObj[orgindex].toObject();
		QJsonObject org = orgObj[orgindex].toObject();
		bool islocal = org["islocal"].toBool();
		int number = son["realnumber"].toInt();

		QJsonObject element;
		element["orgName"] = org["orgName"];
		element["realnumber"] = number;
		element["islocal"] = islocal;
		total += number;
		if (islocal)
			local += number;
		else
			contractor += number;
		selectOrg.append(element);
	}
	QJsonObject res;
	res["code"] = 0;
	res["count"] = selectOrg.count();
	res["data"] = selectOrg;
	res["total"] = total;
	res["local"] = local;
	res["contractor"] = contractor;
	QJsonDocument doc;
	doc.setObject(res);
	return doc;
}
void RealtimeDepartData::service(HttpRequest &request, HttpResponse &response)
{
	int page = request.getParameter("page").toInt();
	int limitnumber = request.getParameter("limit").toInt();
	int orgType = request.getParameter("islocal").toInt();
	if (orgType == 2)
	{
		QJsonDocument doc = backtype2Data();
		response.setHeader("Content-Type", "application/json; charset=UTF-8");
		response.write(doc.toJson());

	}
	else
	{
		QJsonObject object; QJsonArray mainArray;
		int  count = 0;

		QJsonObject robj = DataStore::getRealTimeData();//获取缓存的实时数据
		QJsonObject org = DataStore::getOrgnizeData(); //获取已有部门信息

		QJsonArray dealedArray;//处理后的数组
		for (auto var : robj.keys())
		{
			if (robj[var].isObject())
			{
				QJsonObject subObj = robj[var].toObject();
				//auto subobjstr = DataStore::JsonObjectToJson(subObj);
				QString orgIndexCode = var;
				QJsonObject obj = org[orgIndexCode].toObject();
				QJsonObject subobject;
				int islocal = obj["islocal"].toBool();
				//判断与请求的的部门是否一致
				if (orgType == islocal && (subObj["realnumber"].toInt() != 0))
				{
					auto index = subObj["orgIndexCode"].toString();
					subobject["orgIndexCode"] = subObj["orgIndexCode"];
					auto name = obj["orgName"].toString();
					subobject["orgName"] = obj["orgName"];
					subobject["contractor"] = obj["orgEmployeesNumber"];
					subobject["count"] = subObj["realnumber"].toInt();
					subobject["attendanceratio"] = QString::number(100 * subObj["realnumber"].toInt() / obj["orgEmployeesNumber"].toDouble(), 'f', 1) + "%";
					dealedArray.append(subobject);
				}
			}
		}
		for (int i = (page - 1)*limitnumber; i < (page*limitnumber < dealedArray.count() ? page * limitnumber : dealedArray.count()); i++)
			mainArray.append(dealedArray.at(i).toObject());
		QJsonDocument jsondoc;
		response.setHeader("Content-Type", "application/json; charset=UTF-8");
		object["code"] = 0;
		object["count"] = dealedArray.count();
		object["data"] = mainArray;
		if (count > 0)
			object["msg"] = "get data ok";
		else
			object["msg"] = "no data";
		jsondoc.setObject(object);
		response.write(jsondoc.toJson());
	}

}
