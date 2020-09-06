#ifndef GETDEPARTCONTROLLER_H
#define GETDEPARTCONTROLLER_H
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
#include"datadeal/hikdatarequest.h"
class GetDepartController : public HttpRequestHandler
{
    Q_OBJECT
public:
    explicit GetDepartController(QObject *parent = nullptr);
    void service(HttpRequest& request, HttpResponse& response);
signals:

public slots:
};

#endif // GETDEPARTCONTROLLER_H
