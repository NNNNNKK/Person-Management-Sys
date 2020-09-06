#ifndef REALTIMEPRSNUMBERCONTROLLER_H
#define REALTIMEPRSNUMBERCONTROLLER_H
#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"
#include <QObject>
#include<QFile>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>
#include<QCoreApplication>
#include"datadeal/hikdatarequest.h"

class RealtimePrsNumberController : public HttpRequestHandler
{
    Q_OBJECT
public:
    explicit RealtimePrsNumberController(QObject *parent = nullptr);
    void service(HttpRequest& request, HttpResponse& response);

signals:

public slots:
};

#endif 
