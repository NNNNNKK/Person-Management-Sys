#ifndef REALTIMEDEPAERTDATA_H
#define REALTIMEDEPAERTDATA_H
#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"
#include <QObject>
#include"datadeal/hikdatarequest.h"

class RealtimeDepartData : public HttpRequestHandler
{
    Q_OBJECT
public:
    explicit RealtimeDepartData(QObject *parent = nullptr);
    void service(HttpRequest& request, HttpResponse& response);
signals:

public slots:
};

#endif // REALTIMEDEPARTCONTROLLER_H
