#pragma once
#include"HttpUtillib.h"
#include "HttpHeader.h"
#include "qlist.h"
#include"qjsonobject.h"
#include"qstring.h"
#include"qjsonarray.h"
#include<sstream>
#include<qjsonobject.h>
#include"qjsondocument.h"
#include"qdebug.h"
#include"qfile.h"
#include"qjsonarray.h"
#include<iostream>
#include"qcoreapplication.h"
#include"systemconfig.h"
#include"qdir.h"
#include"HttpUtillib.h"
#include "HttpHeader.h"
#include"qdatetime.h"
#include"qhostaddress.h"
#include"qnetworkinterface.h"
#include"qcoreapplication.h"
#include"qobject.h"
#include"qtimer.h"
#include"qreadwritelock.h"
#include"qreadlocker";
#include"qmutex.h"
#include<iostream> 
#include <DataStore.h>
class HikdataRequest:public QObject
{
	Q_OBJECT
public:
	 explicit HikdataRequest(QString path=QCoreApplication::applicationDirPath(), QObject *parent = nullptr);
	 ~HikdataRequest();
     /**获取得到的所有部门编号和名称*/
	 QJsonArray getTotalOrg();
	 /**历史数据处理*/
	 void historyDataDeal(QString startDate,QString orgCode);
		/**写部门的历史数据*/
	void writeOrgHistoryJson(QString &filename,QJsonObject&object,QString &date, const QString &orgIndex);
	 /**数据请求接口,返回的是Json对象*/
    static  QJsonObject httpRequest(Unit_query_t * query, string Interface_address, QJsonObject requestbody);
	/**把字符串转为json对象*/
    static  QJsonObject string2json(string response);
	/**获取本机ipv4地址*/
	static QString getIpv4();
	/**获取hikQuery*/
	static  Unit_query_t * const gethikQuery() { return hikQuery; }
	/**设置hikQuery的值*/
	static void sethikQueryPointer(Unit_query_t * pointer);
	static int contractHours(QString startdate);
/**实时数据处理接口*/
	static void realtimeDataDeal();
	//static void calcDepartPersonNum(QString filename, QString orgIndex);
public slots:
	
	/**写组织及其组织下的人员*/
	void writeOrganization();

private:
		QString appPath;
	    static  Unit_query_t * hikQuery;
protected:
};
/**生成要查询人员的信息*/
	QJsonObject  producePersonHistory(QJsonValue ob);
	void producePersonRealTime(QJsonObject ob, QJsonObject request,QString orgIndex,QString orgName,int contractor,bool islocal);
	void writePersonRealtimemsg(QString &filename, QString  orgCode, QJsonObject &personMsg, DoorDirection status, QString orgName, int contractor, bool islocal, QString pId);
	



