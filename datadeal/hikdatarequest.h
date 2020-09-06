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
     /**��ȡ�õ������в��ű�ź�����*/
	 QJsonArray getTotalOrg();
	 /**��ʷ���ݴ���*/
	 void historyDataDeal(QString startDate,QString orgCode);
		/**д���ŵ���ʷ����*/
	void writeOrgHistoryJson(QString &filename,QJsonObject&object,QString &date, const QString &orgIndex);
	 /**��������ӿ�,���ص���Json����*/
    static  QJsonObject httpRequest(Unit_query_t * query, string Interface_address, QJsonObject requestbody);
	/**���ַ���תΪjson����*/
    static  QJsonObject string2json(string response);
	/**��ȡ����ipv4��ַ*/
	static QString getIpv4();
	/**��ȡhikQuery*/
	static  Unit_query_t * const gethikQuery() { return hikQuery; }
	/**����hikQuery��ֵ*/
	static void sethikQueryPointer(Unit_query_t * pointer);
	static int contractHours(QString startdate);
/**ʵʱ���ݴ���ӿ�*/
	static void realtimeDataDeal();
	//static void calcDepartPersonNum(QString filename, QString orgIndex);
public slots:
	
	/**д��֯������֯�µ���Ա*/
	void writeOrganization();

private:
		QString appPath;
	    static  Unit_query_t * hikQuery;
protected:
};
/**����Ҫ��ѯ��Ա����Ϣ*/
	QJsonObject  producePersonHistory(QJsonValue ob);
	void producePersonRealTime(QJsonObject ob, QJsonObject request,QString orgIndex,QString orgName,int contractor,bool islocal);
	void writePersonRealtimemsg(QString &filename, QString  orgCode, QJsonObject &personMsg, DoorDirection status, QString orgName, int contractor, bool islocal, QString pId);
	



