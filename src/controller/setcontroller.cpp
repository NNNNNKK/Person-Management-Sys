#include "setcontroller.h"
#include<QFile>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>
#include<QHash>
#include<QDate>
#include<QDir>
#include<QCoreApplication>

SetController::SetController(QObject *parent) : HttpRequestHandler(parent)
{
}
void SetController::service(HttpRequest &request, HttpResponse &response)
{
    QJsonObject object=QJsonObject();
    int number=request.getParameter("rs").toInt();
    QString departid=request.getParameter("gsname");
    QString password=QString(request.getParameter("pw"));
    QString inputusername=QString(request.getParameter("un"));
	QString islocal = QString(request.getParameter("local"));
    if(password==userpassword && username==inputusername)
    {
       if( request.getParameter("gsname").isEmpty())
           object["msg"]="gsname is empty";
       else
       { 
		   QJsonObject ob = DataStore::getOrgnizeData();
		   QJsonObject son = ob[departid].toObject();
		   son["orgEmployeesNumber"]= number;
		   if (islocal == "on")
			   son["islocal"] = true;
		   else
			   son["islocal"] = false;
		   ob.remove(departid);
		   ob.insert(departid, son);
		   DataStore::setOrgnizeData(ob); 
          }
	   object["msg"] = "modefied suceess";
    }
    else
        object["msg"]="password or username wrong";
    object["code"]=0;
    QJsonDocument jsonDoc;
    jsonDoc.setObject(object);
    response.setHeader("Content-Type", "application/json; charset=UTF-8");
    response.write(jsonDoc.toJson());
}
