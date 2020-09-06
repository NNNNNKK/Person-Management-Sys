#include"src/controller/httpcallBackController.h"
#include"datadeal/hikdatarequest.h"
#include"GloableLocker.h"
#include<string.h>
QMutex processLock;

httpcallBackController::httpcallBackController(QObject *parent) : HttpRequestHandler(parent)
{

}
void httpcallBackController::service(HttpRequest& request, HttpResponse& response)
{
	QJsonObject backData = byteArray2json(request.getBody());//把返回的内容解析成json对象
	/*QFile file("backData.json");
	file.open(QIODevice::Append);
	file.write(request.getBody());
	file.close();*/
     response.setHeader("Content-Type", "application/json; charset=UTF-8");
	// qDebug() << "---------------------------------"<<backData;
	 response.write(QString("HTTP/1.1 200 OK").toUtf8());
	 QFuture<void> f = QtConcurrent::run(dealBackdata, backData);//临时线程
}

QJsonObject byteArray2json(const QByteArray& byteArray)
{
	QJsonParseError json_error;
	QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArray, &json_error);
	//解析出错判断
	if (jsonDocument.isNull() || json_error.error != QJsonParseError::NoError)
	{
		qDebug("json error:%s",qPrintable(json_error.errorString())); 
	}
	return jsonDocument.object();
}

void dealBackdata(QJsonObject & ob)
{
	qDebug() << "Processing Callback..." << endl;
	QJsonObject event = ob["params"].toObject()["events"].toArray().first().toObject();//事件
	QString eventId = event["eventId"].toString();
	qDebug() << "Processing Callback EventID:"<< event["eventId"].toString(); 
	QString virtualCard = event["data"].toObject()["ExtEventCardNo"].toString();
	QString srcIndex = event["srcIndex"].toString();
	QString happenTimeStr = ob["params"].toObject()["events"].toArray().first().toObject()["happenTime"].toString();//
	qDebug() << QString("**Callback Info** virtualCard:%1, srcIndex:%2, happenTime:%3").arg(virtualCard).arg(srcIndex).arg(happenTimeStr);
	QJsonObject hikrequest;
	//for deal hik system bug
	QDateTime happentime = QDateTime::fromString(happenTimeStr, "yyyy-MM-ddThh:mm:ss.zzz+08:00");
	if (happentime.date() != QDate::currentDate()) {
		qDebug() << "hik events system error occured!!!";
		return;
	}
	qDebug() << "hik events system time check passed!";
	//qDebug() << DataStore::JsonObjectToJson(event);
	//end
	hikrequest["endTime"] = QDateTime::fromString(happenTimeStr, "yyyy-MM-ddThh:mm:ss.zzz+08:00").addSecs(+1).toString("yyyy-MM-ddThh:mm:ss+08:00");
	hikrequest["startTime"]= QDateTime::fromString(happenTimeStr, "yyyy-MM-ddThh:mm:ss.zzz+08:00").addSecs(-1).toString("yyyy-MM-ddThh:mm:ss+08:00");
	hikrequest["pageNo"] = 1;
	hikrequest["pageSize"] = 1000;
	string api = "/artemis/api/acs/v1/door/events";
	bool flag;
	for (int i = 0; i < 10; i++)
	{
           QThread::msleep(1000);
		   //Test
		  // virtualCard = ob["param1"].toString();
		   //
		   flag = queryDetailInfo(api, hikrequest, virtualCard);
		   if (flag)
			   break;
	}
	if (!flag) {
		qDebug() << "Failed to get detail info from hik server after 10 times trials,Writing log to /Data/Log/";
		QString filename = QCoreApplication::applicationDirPath() + "/Data/Log/"+QDate::currentDate().toString("yyyyMMdd")+".txt";
		QFile file(filename);
		if (file.open(QIODevice::Append))
		{
			QJsonDocument doc;
			doc.setObject(ob);
			file.write(doc.toJson()+"\n");
		}
		file.close();
	}
	qDebug() << "Callback Finished!" << "--is got detail ? "<<flag<<endl;
}

bool queryDetailInfo(std::string &api, QJsonObject &hikrequest, QString &virtualCard)
{
	QJsonObject  personMsg = HikdataRequest::httpRequest(HikdataRequest::gethikQuery(), api, hikrequest);//查询门禁点的事件
	//Test
	//personMsg["code"] = 0;
	//
	if (personMsg.contains("code") && personMsg["code"].toInt() == 0)
	{
		QJsonArray eventlist = personMsg["data"].toObject()["list"].toArray();//
		//Test
		//eventlist.append("djg");
		//
		for (auto var : eventlist)
		{
			QJsonObject subObj = var.toObject();
			//Test
			//subObj["cardNo"]= virtualCard;
			//
			if (subObj["cardNo"].toString() == virtualCard)
			{
				QString orgIndex = subObj["orgIndexCode"].toString();
				QString orgName = subObj["orgName"].toString();
				QString pId = subObj["personId"].toString();
				QString doorName = subObj["doorName"].toString();
				//Test
				//doorName = virtualCard;
				//
				qDebug() << QString("Processing Person ID:%1, orgIndex: %2, doorName:%3").arg(pId).arg(orgIndex).arg(QString::fromUtf8(doorName.toStdString().data()));
				DoorDirection enterType;
				QMutexLocker locker(&processLock);
				bool ret = DataStore::checkDoorByName(doorName, enterType);
				if (ret) {
					//if we got door direction,then set state of the person
					QJsonObject openJson = DataStore::getOrgnizeData()[orgIndex].toObject();
					bool islocal = openJson["islocal"].toBool();//返回该部门的类型
					int contractor = openJson["orgEmployeesNumber"].toInt();
					QJsonObject ps;
					ps["personName"] = subObj["personName"].toString();
					static QString lastEnterPersonName;
					//Test
					//lastEnterPersonName = "gjj";
					//
					ps["eventTime"] = subObj["eventTime"].toString();
					ps["eventPlace"] = doorName;
					ps["iostate"] = enterType == DoorDirection::Import ? "in" : "out";
					if ((enterType == DoorDirection::Import)&&!islocal) {
						qDebug() << "**********************Person Enter****************************************Name:"<< qPrintable(ps["personName"].toString())<<"Id:"<< pId;
						if (lastEnterPersonName != subObj["personName"].toString()) {
							DataStore::addTotalToday(pId);
						}
					}
					else if(enterType == DoorDirection::Export){
						qDebug() << "**********************Person Out  ****************************************Name:" << qPrintable(ps["personName"].toString()) << "Id:" << pId;
					}
					else {
						qDebug() << "**********************Ignore Unused Door****************************************Name:" << qPrintable( ps["personName"].toString()) << "Id:" << pId;
					}
					lastEnterPersonName = subObj["personName"].toString();
					qDebug()<<"Processing EventTime:"  << subObj["eventName"].toString();
					//here we write person state to file
					writeJson(orgIndex, orgName, pId, enterType, ps, islocal, contractor);//写json
					//here we recalculate depart state
					DataStore::calcDepartPersonNum(orgIndex);
					qDebug() << "**********************Write Record Over ***************************************Name:" << ps["personName"].toString() << "Id:" << pId;
					return true; 
				}
				else
				{
					//if we failed to get door direction, maybe door config file is error!
					qDebug() << "failed to get door direction, maybe door the config file is error!";
					return false;
				}

				//unreached here
				
			}
		}
	}
	return false;
}

void writeJson(const QString & orgIndex, const QString & orgName, const QString &pId, const DoorDirection type, const QJsonObject & msg, bool islocal, int contractor)
{
	if (type == DoorDirection::Unused)
		return;
	QJsonObject priObj = DataStore::getRealTimeData();//最外层大的obj
	auto ite= priObj.find(orgIndex);
	if (ite == priObj.end())
	{
		QJsonObject psdetail;
		psdetail[pId] = msg;//人员最新记录
		psdetail["orgIndexCode"] = orgIndex;//组织唯一标识码
		psdetail["orgName"] = orgName;//组织名字
		if (type == DoorDirection::Import)
			psdetail["realnumber"] = int(1);
		else  if (type == DoorDirection::Export)
			psdetail["realnumber"] = int(0);
		priObj[orgIndex] = psdetail;
	}
	else
	{
		QJsonObject ob = (*ite).toObject();
		ob["islocal"] = islocal;
		ob["contractor"] = contractor;
		if (type == DoorDirection::Import)
			ob["realnumber"]= ob["realnumber"].toInt()+1;
		else  if (type == DoorDirection::Export)
			ob["realnumber"] = ob["realnumber"].toInt() - 1;
		auto subite = ob.find(pId);
		if (subite == ob.end())
			ob.insert(pId, msg);
		else
			(*subite)=msg;
		(*ite) = ob;
	}
	DataStore::setRealTimeData(priObj);
}
