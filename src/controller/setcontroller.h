#ifndef SETCONTROLLER_H
#define SETCONTROLLER_H

#include <QObject>
#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"
#include"datadeal/hikdatarequest.h"
class SetController : public HttpRequestHandler
{
    Q_OBJECT
public:
    explicit SetController(QObject *parent = nullptr);
    void service(HttpRequest& request, HttpResponse& response);
signals:
private:
    QString userpassword="ruigan8888";
    QString username="ruigan";
public slots:
};
#endif // SETCONTROLLER_H
