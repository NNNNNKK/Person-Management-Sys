#ifndef HISTORYDEPARTDETAILDATA_H
#define HISTORYDEPARTDETAILDATA_H
#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"
#include <QObject>

class HistoryDepartDetailData : public HttpRequestHandler
{
    Q_OBJECT
public:
    explicit HistoryDepartDetailData(QObject *parent = nullptr);
    void service(HttpRequest& request, HttpResponse& response);
	QJsonDocument backData(int&page,int&limit, QString &dept_id, QString &date);
	QJsonDocument backtyData(int&page, int&limit, QString &dept_id,QString &personid, QString &date);
signals:
	
private:

public slots:
};

#endif // SUMMARIZEDATACONTROLLER_H
