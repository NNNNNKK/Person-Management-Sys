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
	QString tfilename= path + "/Data/RealTime/todayPersonId.data";//当天进入人员的id
	auto item = DataStore::openJsonDocument(filename);
	auto idObj = DataStore::openJsonDocument(tfilename);//打开存入的id文件
	auto idArray = idObj["id"].toArray();

	//判断是否为空
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
		//新的一天
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
	writeJsonDocument(tfilename, idObj);//写id
	writeJsonDocument(filename, item);//写人数
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

//打开json文档，并返回json对象
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
		return QJsonObject();//返回空的json对象
	}
	if (jsonDoc.isObject())
		return jsonDoc.object();
	else
		return QJsonObject();
}
//写json文档
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
	//判断文件夹是否存在，若不存在则创建该路径下的文件夹
	QString  fileDir = appPath + "/Data/Config";
	if (checkDirExist(fileDir)) {
		QString fileName = fileDir + "/org.data";
		//无论文件是否存在都要打开json文档
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
	//判断文件夹是否存在，若不存在则创建该路径下的文件夹
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
	//判断文件夹是否存在，若不存在则创建该路径下的文件夹
	QString  fileDir = appPath + "/Data/RealTime";
	if (checkDirExist(fileDir)) {
		QString fileName = fileDir + "/rtime.data";
		//无论文件是否存在都要打开json文档
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
	//判断文件夹是否存在，若不存在则创建该路径下的文件夹
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
	////如果没有获取到ISC数据则返回
	//if(newOrgIndex.count()==0)
	//    return false;
	//else
	//{
		//处理org.data
		QJsonObject orgList=DataStore::getOrgnizeData();
		if(newOrgIndex.count()>0)
		     for (auto var : orgList.keys())
		    {
			    //未查到该部门
			   if (!newOrgIndex.contains(var))
			  {
				orgList.remove(var);
			  }
		    }
		DataStore::setOrgnizeData(orgList);
		//处理rtime.data
		QJsonObject rtimeObj = DataStore::getRealTimeData();
		QStringList orgIndexCodes = orgList.keys();
		for (auto var1 : rtimeObj.keys())
		{
			//未查到该部门
			if (!orgIndexCodes.contains(var1))
			{
				rtimeObj.remove(var1);
			}
		}
		DataStore::setRealTimeData(rtimeObj);
	//}
}
