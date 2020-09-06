#include "DataStore.h"
#include <QFile>
#include <QJsonObject>
#include <QDate>
#include <QApplication>
#include <QDebug>
#include <qjsonarray.h>
#include <QDir>

QReadWriteLock DataStore::rtimefilelocak;
QReadWriteLock DataStore::orgfilelocak;
QReadWriteLock DataStore::totalfilelock;

bool DataStore::addTotalToday(QString personId)
{
	QWriteLocker locker(&totalfilelock);
	QString path = QApplication::applicationDirPath();
	QString filename = path + "/Data/RealTime/total.data";
	QString tfilename= path + "/Data/RealTime/todayPersonId.data";//���������Ա��id
	auto item = DataStore::openJsonDocument(filename);
	auto idObj = DataStore::openJsonDocument(tfilename);//�򿪴����id�ļ�
	auto idArray = idObj["id"].toArray();

	//�ж��Ƿ�Ϊ��
	if (item.isEmpty())
	{
		item["totalInToday"] = 1;
		idArray = QJsonArray();
		idArray.append(personId);
		idObj["id"] = idArray;
		item["updateDay"] = QDate::currentDate().toString("yyyy-MM-dd");
	}
	else 
	{
		bool containflag = idArray.contains(personId);
		//�µ�һ��
		if (QDate::currentDate().toString("yyyy-MM-dd") != item["updateDay"].toString()) 
		{
			item["totalInToday"] = 1;
			item["updateDay"] = QDate::currentDate().toString("yyyy-MM-dd");
			idArray = QJsonArray();
			idArray.append(personId);
			idObj["id"] = idArray;
		}
		else 
		{
			if (!containflag)
			{
				item["totalInToday"] = item["totalInToday"].toInt() + 1;
				idArray.append(personId);
				idObj["id"] = idArray;
			}
		}
	}
	writeJsonDocument(tfilename, idObj);//дid
	writeJsonDocument(filename, item);//д����
	return true;
}

int DataStore::getTotalToday()
{ 
	QReadLocker locker(&totalfilelock);
	QString path = QApplication::applicationDirPath();
	QString filename = path + "/Data/RealTime/total.data";
	auto item = DataStore::openJsonDocument(filename);
	if (item.empty()) {
		return 0;
	}
	else {
		if (QDate::currentDate() != QDate::fromString(item["updateDay"].toString(),"yyyy-MM-dd")) {
			//item["totalInToday"] = 0;
			//item["updateDay"] = QDate::currentDate().toString("yyyy-MM-dd");
			//writeJsonDocument(filename, item);
			return 0;
		}else{
			return item["totalInToday"].toInt();
		}
		
	}
}

//��json�ĵ���������json����
QJsonObject DataStore::openJsonDocument(QString filename)
{
	QFile loadfile(filename);
	if (!loadfile.open(QIODevice::ReadOnly| QIODevice::Text))
	{
		qDebug("could't open %s", qPrintable(filename));
		return QJsonObject();
	}
	QByteArray allData = loadfile.readAll();
	QJsonParseError json_error;
	QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &json_error));
	if (json_error.error != QJsonParseError::NoError)
	{
		qDebug("json error:%s", qPrintable(json_error.errorString()));
		return QJsonObject();//���ؿյ�json����
	}
	if (jsonDoc.isObject())
		return jsonDoc.object();
	else
		return QJsonObject();
}
//дjson�ĵ�
void DataStore::writeJsonDocument(QString fileName, QJsonObject &writeObject)
{
	QFile loadfile(fileName);
	if (!loadfile.open(QIODevice::WriteOnly))
	{
		qDebug("error:could't open %s ", qPrintable(fileName));
	}
	QJsonDocument document;
	document.setObject(writeObject);
	loadfile.write(document.toJson());
	loadfile.flush();
	loadfile.close();
}

bool DataStore::checkDoorByName(QString doorName, DoorDirection & InOut)
{
	QString path = QApplication::applicationDirPath();
	QString filename = path + "/Data/Config/doors.data";
	qDebug() << doorName;
	auto item = DataStore::openJsonDocument(filename);
	if (item.isEmpty()) {
		qDebug() << "Get Doors Config File Failed!";
		return false;
	}
	else {

		if (item["doors"].toObject()["imports"].isArray()) {
			auto doors = item["doors"].toObject()["imports"].toArray();
		
			for (QJsonValue door : doors) {
				if (doorName == door["name"].toString()) {
					InOut = DoorDirection::Import;
					return true;
				}
			} 
		}
		else {
			qDebug() << "Get Doors Config File Imports Array Failed!";
			return false;
		}

		if (item["doors"].toObject()["exports"].isArray()) {
			auto doors = item["doors"].toObject()["exports"].toArray();
			for (QJsonValue door : doors) {
				if (doorName == door["name"].toString()) {
					InOut = DoorDirection::Export;
					return true;
				}
			}
		}
		else {
			qDebug() << "Get Doors Config File Exports Array Failed!";
			return false;
		}

		InOut = DoorDirection::Unused;
		return true;
	}

	qDebug() << "Get Doors Config File Access Fault State!";
	return false;
}

QJsonObject DataStore::getOrgnizeData()
{
	QReadLocker locker(&orgfilelocak);
	QString appPath = QCoreApplication::applicationDirPath();
	//�ж��ļ����Ƿ���ڣ����������򴴽���·���µ��ļ���
	QString  fileDir = appPath + "/Data/Config";
	if (checkDirExist(fileDir)) {
		QString fileName = fileDir + "/org.data";
		//�����ļ��Ƿ���ڶ�Ҫ��json�ĵ�
		QJsonObject   Orglist = openJsonDocument(fileName);
		return Orglist;
	}
	else {
		return QJsonObject();
	}
}

void DataStore::setOrgnizeData(QJsonObject data)
{
	QWriteLocker locker(&orgfilelocak);
	QString appPath = QCoreApplication::applicationDirPath();
	//�ж��ļ����Ƿ���ڣ����������򴴽���·���µ��ļ���
	QString  fileDir = appPath + "/Data/Config";
	if (checkDirExist(fileDir)) {
		QString fileName = fileDir + "/org.data";
		writeJsonDocument(fileName, data);
	}
	
}

QJsonObject DataStore::getRealTimeData()
{
	QReadLocker locker(&rtimefilelocak);

	QString appPath = QCoreApplication::applicationDirPath();
	//�ж��ļ����Ƿ���ڣ����������򴴽���·���µ��ļ���
	QString  fileDir = appPath + "/Data/RealTime";
	if (checkDirExist(fileDir)) {
		QString fileName = fileDir + "/rtime.data";
		//�����ļ��Ƿ���ڶ�Ҫ��json�ĵ�
		QJsonObject   Orglist = openJsonDocument(fileName);
		return Orglist;
	}
	else {
		return QJsonObject();
	} 
}

void DataStore::setRealTimeData(QJsonObject data)
{
	QWriteLocker locker(&rtimefilelocak);
	QString appPath = QCoreApplication::applicationDirPath();
	//�ж��ļ����Ƿ���ڣ����������򴴽���·���µ��ļ���
	QString  fileDir = appPath + "/Data/RealTime";
	if (checkDirExist(fileDir)) {
		QString fileName = fileDir + "/rtime.data";
		writeJsonDocument(fileName, data);
	}
}

void DataStore::calcDepartPersonNum(QString orgIndex)
{ 
	QJsonObject OrgDoc = DataStore::getRealTimeData();
	QJsonObject orginfo = OrgDoc[orgIndex].toObject();
	int realnum = 0;
	for (auto personIndexs : orginfo.keys())
	{
		if (orginfo[personIndexs].isObject()) {
			if (orginfo[personIndexs].toObject()["iostate"] == "in") {
				realnum++;
			}
		}
	}
	orginfo["realnumber"] = realnum;
	OrgDoc[orgIndex] = orginfo; 
	setRealTimeData(OrgDoc); 
}

bool DataStore::checkDirExist(QString path)
{ 
	QDir dir;
	if (!dir.exists(path))
	{
		if (!dir.mkdir(path)) {
			qDebug("create %s path failed.", qPrintable(path));
			return false;
		} 
	}
	return true;
}


QString DataStore::JsonObjectToJson(QJsonObject object) {
	QJsonDocument doc;
	doc.setObject(object);
	return doc.toJson();
}
QString DataStore::JsonObjectToJson(QJsonArray object) {
	QJsonDocument doc;
	doc.setArray(object);
	return doc.toJson();
}

void DataStore::deleteoldData(QStringList &newOrgIndex)
{
	////���û�л�ȡ��ISC�����򷵻�
	//if(newOrgIndex.count()==0)
	//    return false;
	//else
	//{
		//����org.data
		QJsonObject orgList=DataStore::getOrgnizeData();
		if(newOrgIndex.count()>0)
		     for (auto var : orgList.keys())
		    {
			    //δ�鵽�ò���
			   if (!newOrgIndex.contains(var))
			  {
				orgList.remove(var);
			  }
		    }
		DataStore::setOrgnizeData(orgList);
		//����rtime.data
		QJsonObject rtimeObj = DataStore::getRealTimeData();
		QStringList orgIndexCodes = orgList.keys();
		for (auto var1 : rtimeObj.keys())
		{
			//δ�鵽�ò���
			if (!orgIndexCodes.contains(var1))
			{
				rtimeObj.remove(var1);
			}
		}
		DataStore::setRealTimeData(rtimeObj);
	//}
}
