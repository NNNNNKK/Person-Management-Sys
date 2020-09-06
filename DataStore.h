#pragma once
#include <qjsondocument.h>
#include <QReadWriteLock> 
enum class DoorDirection
{
	Import,Export,Unused
};
class DataStore
{
public:
	static bool addTotalToday(QString personId);
	static int getTotalToday();
	static QJsonObject openJsonDocument(QString filename);
	static void writeJsonDocument(QString fileName, QJsonObject & writeObject);
	static bool checkDoorByName(QString doorName, DoorDirection & InOut);
	static QJsonObject& GetGlobalRealTimeObject() {
		return DataStore::GlobalRealTimeObject;
	}
	static QJsonObject getOrgnizeData();
	static void setOrgnizeData(QJsonObject data);

	static QJsonObject getRealTimeData();
	static void setRealTimeData(QJsonObject data);

	static void calcDepartPersonNum(QString orgIndex);

	static bool checkDirExist(QString path);
	static QString JsonObjectToJson(QJsonObject object);
	static QString JsonObjectToJson(QJsonArray object);
	/**delete old org data and rtime data*/
	static void deleteoldData(QStringList &newOrgIndex);
private:
	static QJsonObject GlobalRealTimeObject;
	static QReadWriteLock rtimefilelocak;
	static QReadWriteLock orgfilelocak;
	static QReadWriteLock totalfilelock;
};

