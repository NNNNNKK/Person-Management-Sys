#include "getdepartcontroller.h"

GetDepartController::GetDepartController(QObject *parent) : HttpRequestHandler(parent)
{
}
void GetDepartController::service(HttpRequest &request, HttpResponse &response)
{
    Q_UNUSED(request)
	QJsonObject ob = DataStore::getOrgnizeData();
	QJsonArray writeArray;
	QJsonObject object;
	for (auto var : ob)
	{
		QJsonObject subObject;
		subObject["id"] = var.toObject()["orgIndexCode"].toString();
		subObject["name"] = var.toObject()["orgName"].toString();
		subObject["contractorNumber"] = var.toObject()["orgEmployeesNumber"];
		writeArray.append(subObject);
	}
    if(writeArray.count()>0)
        object["msg"]="get data suceess";
    else
        object["msg"]="no data";
    object["count"]= writeArray.count();
    object["data"]= writeArray;
	QJsonDocument jsondoc;
    jsondoc.setObject(object);
    response.setHeader("Content-Type", "application/json; charset=UTF-8");
    response.write(jsondoc.toJson());
}
