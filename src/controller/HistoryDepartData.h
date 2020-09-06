#ifndef HISTORYDEPARTDATA_H
#define HISTORYDEPARTDATA_H
#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"
#include <QObject>

class HistoryDepartData : public HttpRequestHandler
{
    Q_OBJECT
public:
    explicit HistoryDepartData(QObject *parent = nullptr);
    void service(HttpRequest& request, HttpResponse& response);
signals:

public slots:
};

#endif 