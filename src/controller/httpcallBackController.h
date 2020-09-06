#ifndef HTTPCALLBACKCONTROLLER_H
#define HTTPCALLBACKCONTROLLER_H
#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"
#include <QObject>
#include<QDate>
#include<QFile>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>
#include<QCoreApplication>
#include <QtConcurrent>
#include <QFuture> 
#include <DataStore.h>
#include <qqueue.h>

class httpcallBackController : public HttpRequestHandler
{
	Q_OBJECT
public:
	explicit httpcallBackController(QObject *parent = nullptr);
	void service(HttpRequest& request, HttpResponse& response);
	
signals:

public slots:
private :
};
/**字节数组转为json*/
QJsonObject byteArray2json(const QByteArray & byteArray);
/**处理返回的数据*/
void  dealBackdata(QJsonObject &ob);
bool queryDetailInfo(std::string &api, QJsonObject &hikrequest, QString &virtualCard);
/**写json*/
void writeJson(const QString &orgIndex,const QString &orgName,const QString &pId, const DoorDirection type,const QJsonObject& msg,bool islocal, int contractor);
#endif 
