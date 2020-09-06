#include"hikdatarequest.h"
#include"qfuture.h"
#include"qtconcurrent"
#include"GloableLocker.h"
#include<qtextcodec.h>
Unit_query_t* HikdataRequest::hikQuery = nullptr;

HikdataRequest::HikdataRequest( QString path, QObject *parent):appPath(path)
{
}
//�ַ���תΪjson
QJsonObject HikdataRequest::string2json(string response)
{
	QString qszResponse = QString::fromUtf8(response.c_str());
	QJsonDocument jsonDocument = QJsonDocument::fromJson(qszResponse.toUtf8());
	if (jsonDocument.isEmpty())
	{
		qDebug(" hik https  response string is empty and response html is :%s",qPrintable(qszResponse.toLocal8Bit().data()));
		return QJsonObject();
	}
	else
		return jsonDocument.object();
}
//��ȡIPV4��ַ
QString HikdataRequest::getIpv4()
{
	QString strIpAddress;
	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	// ��ȡ��һ����������IPv4��ַ
	int nListSize = ipAddressesList.size();
	for (int i = 0; i < nListSize; ++i)
	{
		if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
			ipAddressesList.at(i).toIPv4Address()) {
			strIpAddress = ipAddressesList.at(i).toString();
			break;
		}
	}
	// ���û���ҵ������Ա���IP��ַΪIP
	if (strIpAddress.isEmpty())
		strIpAddress = QHostAddress(QHostAddress::LocalHost).toString();
	return strIpAddress;
}
//д��ʷ��������
void HikdataRequest::writeOrgHistoryJson(QString & filename, QJsonObject & object, QString & date,const QString &orgIndex)
{      
	  QReadWriteLock lock;
	  lock.lockForWrite();
	  QString dirPath = appPath + "/Data/HistoryData/" + date;
	  QDir dir(dirPath);
		if (!dir.exists())
			dir.mkpath(dirPath);
		QString fileName = dirPath + "/" + filename;//�ļ�·�����������ļ���
		QJsonObject root;
		root[orgIndex] = object;
		DataStore::writeJsonDocument(fileName, root);
		lock.unlock();
}
//��hikQuery��ֵ
void HikdataRequest::sethikQueryPointer(Unit_query_t * pointer)
{
	if (pointer != nullptr)
		hikQuery = pointer;
	else
		qDebug("pointer is nullptr");
}
//��ͬ������
int HikdataRequest::contractHours(QString startdate)
{
	QString currentDate = QDate::currentDate().toString("yyyyMM");
	QDate beginDate = QDate::fromString(startdate + "01", "yyyyMMdd");
	QDate endDate;
	//�ж���ʼ�·��Ƿ�Ϊ��ǰ�·�
	if (currentDate == startdate)
		endDate = QDate::currentDate();
	else
		endDate = QDate::fromString(startdate + "01", "yyyyMMdd").addMonths(1);
	//�жϿ�ʼʱ���Ƿ�С�ڽ���ʱ��
	int weekDays = 0;
	while (beginDate.operator<(endDate))
	{
		int weeknumber = beginDate.dayOfWeek();
		if (weeknumber != 6 && weeknumber != 7)
			weekDays++;
		beginDate = beginDate.addDays(1);
	}
	return weekDays;
}
//��������
HikdataRequest::~HikdataRequest()
{
	//delete hikQuery;
}
//��ȡ���в����б�
QJsonArray HikdataRequest::getTotalOrg()
{   
	string Interface_address = "/artemis/api/resource/v1/org/orgList";
	QJsonObject requestobject;
	QJsonArray orgArray;//Ҫ���صĲ����б���Ŀ
	requestobject["pageNo"] = 1;
	requestobject["pageSize"] = 1000;
	QJsonObject responseJson=httpRequest(hikQuery, Interface_address, requestobject);

	QString path = QCoreApplication::applicationDirPath();
	QString filename = path + "/Data/Config/orgblacklist.data";
	QJsonArray balckList = DataStore::openJsonDocument("filename")["black"].toArray();//��ȡ����֯������
	if (responseJson.contains("code") && responseJson.value("code").toInt() == 0)
	{ 
		QJsonObject dataObj = responseJson["data"].toObject();
		 QJsonArray pre_org = dataObj.value("list").toArray();
		 for (auto var : pre_org)
		 {
			 if (!balckList.contains(var.toObject()["parentOrgName"].toString()))
				 orgArray.append(var.toObject());
		 }
	}
	else
          qDebug("get source error:get orgList failed!");
	return orgArray;
}
//д��֯����Ա��
void  HikdataRequest::writeOrganization()
{
	qDebug("process depart Information."); 
	QJsonObject   oldOrglist = DataStore::getOrgnizeData();//��ȡ�ɵĲ�����Ϣ
	QStringList oldIndexCodes=oldOrglist.keys();//��ȡ�ɲ��ŵ�Ψһ��ʶ��
	QStringList newIndexCodes;//�����ݲ��ŵ�λ�Ʊ�ʶ��
	QJsonArray orgArray = getTotalOrg();//��ȡisc�е����е���֯�б�
	for (int i = 0; i < orgArray.size(); i++)
	{
		QJsonObject getedOrg = orgArray.at(i).toObject();
		getedOrg.remove("updateTime");
		getedOrg.remove("parentOrgIndexCode");
		QString orgIndexCode = getedOrg.value("orgIndexCode").toString();//��֯Ψһ��ʶ��
		newIndexCodes.append(orgIndexCode);
		//��ȡÿ����֯�µ���Ա��Ϣ
		QJsonObject request;
		request["orgIndexCode"] = orgIndexCode;
		request["pageNo"] = 1;
		request["pageSize"] = 1000;
		string api = "/artemis/api/resource/v2/person/orgIndexCode/personList";
		QJsonObject backData=httpRequest(hikQuery, api, request);
		//���ݻ�ȡ�ɹ���ִ�еĲ���
		if (backData.contains("code") && backData["code"].toInt() == 0)
		{
			QJsonArray employeelist;
			QJsonArray backlist = backData["data"].toObject()["list"].toArray();
			for (auto var : backlist)
			{
				if (var.isObject())
				{
					QJsonObject ob = var.toObject();
					ob.remove("personPhoto");//ɾ��������Ƭ��Ϣ
					ob.remove("createTime");//ɾ��������ʱ��
					ob.remove("updateTime");//ɾ������ʱ��
					ob.remove("orgPath");
					ob.remove("orgPathName");
					ob["orgName"] = getedOrg["orgName"];//��������
					employeelist.append(ob);
				}
			}
			auto ite = oldOrglist.find(orgIndexCode);
			if (ite == oldOrglist.end())//δ�ҵ�����֯
			{
				getedOrg["orgEmployeesNumber"] = 50;
				getedOrg["islocal"] = false;
				getedOrg["isModify"] = false;
				getedOrg["employeelist"] = employeelist;
				oldOrglist[orgIndexCode] = getedOrg;
				qDebug() << "failed to find exsting depart, we will create one";
			}
			else
			{
				//qDebug() << (*ite);
				 (*ite).toObject()["employeelist"] = employeelist;
				// qDebug() <<endl<<"------------------------------------------------------------------------------"<<endl<< (*ite);
			}
			DataStore::setOrgnizeData(oldOrglist); 
		}
		else
			qDebug("error:get %s personList faied",qPrintable(orgIndexCode));
	}
	DataStore::deleteoldData(newIndexCodes);//ɾ�����ź�ʵʱ�������Ѿ������ڲ��ŵ�����
	qDebug("update depart Information finished.");
}
 
//����ÿ���˵�����

QJsonObject producePersonHistory(QJsonValue ob)
{
	QJsonObject pMs = ob.toObject();
	QJsonObject request = pMs["request"].toObject();
	QJsonArray  pCodes;
	pCodes.append(pMs["personId"].toString());
	request["personIds"] = pCodes;
	request["pageNo"] = 1;
	request["pageSize"] = 100;
	string Interface_address = "/artemis/api/acs/v1/door/events";
	//qDebug() << request << endl << "---------------------------------" << endl;
	QJsonObject personevent = HikdataRequest::httpRequest(HikdataRequest::gethikQuery(), Interface_address, request);
	//qDebug() << personevent << endl << "---------------------------------" << endl;
	QJsonArray backData; QJsonObject pDetail; QDateTime entertime;
	pDetail["personName"] = pMs["personName"].toString();//��Ա����
	pDetail["cardNumber"] = pMs["jobNo"].toString();//��Ա�Ŀ���
	pDetail["attendenceHours"] = double(0);//��Ա�ۼƹ���ʱ��
	pDetail["personId"] = pMs["personId"].toString();//��Աid
	pDetail["orgindex"] = pMs["orgIndexCode"];
	pDetail["orgName"] = pMs["getedOrg"];
	pDetail["detail"]=QJsonArray();
	if (personevent.contains("code")&&personevent["code"].toString() == QString::number(0))
	{
		backData = personevent["data"].toObject()["list"].toArray();
		int totalPage = personevent["data"].toObject()["totalPage"].toInt();//�ܹ���ҳ��
		for (int i = 2; i <= totalPage; i++)
		{
			request["pageNo"] = i;
			QJsonArray subData = HikdataRequest::httpRequest(HikdataRequest::gethikQuery(), Interface_address, request)["data"].toObject()["list"].toArray();
			for (auto var : subData)
				backData.append(var.toObject());
		}
		bool enterflag = false;//�����־λ
		double workSecs=0;//�ò��Ÿ���ʵ�ʹ���ʱ��
		QJsonArray detailMsg;//����������Ϣ
		for (auto var : backData)
		{ 
			//qDebug() << var << endl << "---------------------------------" << endl;
			QJsonObject hisMsg,varOb= var.toObject();
			hisMsg["personName"] = varOb["personName"];hisMsg["doorName"] = varOb["doorName"];hisMsg["eventTime"] = varOb["eventTime"];
			detailMsg.append(hisMsg);//����jsonarray
			QString doorName = hisMsg["doorName"].toString();//�����ǳ��ı�־
			int backEnterType = doorName.mid(doorName.size() - 1,1).toInt();
			QString httpbacktime = hisMsg["eventTime"].toString();//http���ص��¼���ʱ��
			QString time = httpbacktime.mid(0, 10) + " " + httpbacktime.mid(11, 8);
			/*�жϽ����־�Ƿ�Ϊ��*/
			if (enterflag)
			{   //�Ѿ����룬�жϸ��¼��Ƿ�Ϊ��
				if (backEnterType == 2){
					QDateTime leaveTime = QDateTime::fromString(httpbacktime, "yyyy-MM-ddThh:mm:ss+08:00");
					workSecs += entertime.secsTo(leaveTime);//�����ڽ�����֮���ʱ���,���ۼ�����
					enterflag = false;//������־λΪ��
				}continue;//�¼�����Ϊ�����򲻴���
			}
			//�����־Ϊfalse���ж����¼��Ƿ�Ϊ��
			else
			{
				if (backEnterType == 1){
					enterflag = true; entertime = QDateTime::fromString(httpbacktime, "yyyy-MM-ddThh:mm:ss+08:00");
				} continue;//�¼�����һֱΪ����������
			}
		}
		pDetail["detail"] = detailMsg;
		pDetail["attendenceHours"]= QString::number(workSecs/float(3600),'f',2);//������¸ò���������Ա�Ĺ���ʱ��,����2λ��Ч����
	}
	return pDetail;
}
//ʵʱ��Ա������Ϣ����
//static QReadWriteLock rlock;
void producePersonRealTime(QJsonObject ob, QJsonObject request, QString orgIndex, QString orgName, int contractor, bool islocal)
{
	QJsonArray  pCodes;
	pCodes.append(ob["personId"].toString());
	request["personIds"] = pCodes;
	string Interface_address = "/artemis/api/acs/v1/door/events";
	QJsonObject personevent = HikdataRequest::httpRequest(HikdataRequest::gethikQuery(), Interface_address, request);
	QJsonObject personMsg;//���˽�����Ϣ
	DoorDirection status;
	DoorDirection doorType;
	QString doorName;
	if (personevent.contains("code") && personevent["code"].toInt() == 0)
	{
		QJsonArray list = personevent["data"].toObject()["list"].toArray();//��ȡ����list
		QJsonObject detail = list.first().toObject();//���¼�¼
		if (detail.isEmpty())
		{
			personMsg["personName"] = ob["personName"].toString();//����
			personMsg["eventTime"] = QString(QString::fromLocal8Bit("no record").toUtf8());
			personMsg["eventPlace"] = QString(QString::fromLocal8Bit("no record").toUtf8());
			doorType = DoorDirection::Export;
		}
		else
		{
			doorName = detail["doorName"].toString();
			bool ret = DataStore::checkDoorByName(doorName, doorType);
			if (ret) {
			}
			else {
				//if we failed to get door direction, maybe door config file is error!
				qDebug() << "failed to get door direction, maybe door the config file is error!";
				return;
			} 
			personMsg["personName"] = ob["personName"].toString();//����
			QString eventTime = detail["eventTime"].toString();//����ʱ��
			personMsg["eventTime"] = eventTime.mid(0,10)+" "+ eventTime.mid(11, 8);
			personMsg["eventPlace"] = detail["doorName"].toString();//�򿨵���
		}
		if (doorType == DoorDirection::Import)
		{
			status = DoorDirection::Import;
			personMsg["iostate"] = "in";//�򿨵���
		}
		else if(doorType == DoorDirection::Export || doorType == DoorDirection::Unused){
			status = DoorDirection::Export;
			personMsg["iostate"] = "out";//�򿨵���
		}	
	}
	else {
		//api error
		personMsg["personName"] = ob["personName"].toString();//����
		personMsg["eventTime"] = QString(QString::fromLocal8Bit("select Failed").toUtf8());
		personMsg["eventPlace"] = QString(QString::fromLocal8Bit("select Failed").toUtf8());
		personMsg["iostate"] = "out";//�򿨵���
		status = DoorDirection::Export;
	}
	QString dirPath = QCoreApplication::applicationDirPath() + "/Data/RealTime/";
	QDir path(dirPath);
	if (!path.exists())
		path.mkpath(dirPath);
	QString dir = "";//д���ļ�·�� 
	writePersonRealtimemsg(dir, orgIndex, personMsg, status,orgName, contractor,islocal, ob["personId"].toString()); 
}
//ʵʱд�˵��ļ�
void writePersonRealtimemsg(QString &filename,QString  orgCode,QJsonObject &personMsg, DoorDirection status,QString orgName,int contractor,bool islocal,QString pId)
{
	QJsonObject  && ob = DataStore::getRealTimeData();//��ԭ������
	QJsonObject orgObj = ob[orgCode].toObject();
	if (orgObj.isEmpty())
	{
		QJsonObject sonObj;
		sonObj["contractor"] = contractor;
		sonObj[pId] = personMsg;
		sonObj["islocal"] = islocal;
		sonObj["orgIndexCode"] = orgCode;
		sonObj["orgName"] = orgName;
		if (status == DoorDirection::Import) {
			sonObj["realnumber"] = 1;
		}
		else {
			sonObj["realnumber"] = 0;
		}
		ob[orgCode] = sonObj;
	}
	else
	{  
		/*if (status == DoorDirection::Import)
			orgObj["realnumber"] = orgObj["realnumber"].toInt() + 1;*/
		//else
		//	orgObj["realnumber"] = orgObj["realnumber"].toInt() -1;
		orgObj[pId] = personMsg;
		ob[orgCode] = orgObj;
	}
	DataStore::setRealTimeData(ob); 
}

//�����Ž�����ʷ���ݴ�����
void HikdataRequest::historyDataDeal(QString startDate, QString orgCode)
{
	//��ȡһ���µĹ����յ�����
	int weekDays = contractHours(startDate);
	QJsonObject writeObeject;
	QString selectDate = startDate.mid(0, 4) + "-" + startDate.mid(4, 2) + "-01T00:00:00+08:00";//��ѯʱ���ʼ��
	QJsonObject orgObject = DataStore::getOrgnizeData();//��ȡ������Ϣ
	QJsonObject request; request["eventType"] = 196893;//�¼�����,��������֤ͨ��
	request["startTime"] = selectDate;//��ѯ��ʼʱ�� 
	request["endTime"] = QDateTime::fromString(selectDate, "yyyy-MM-ddThh:mm:ss+08:00").addMonths(1).addDays(-1).toString("yyyy-MM-ddThh:mm:ss+08:00") + "";//��ѯ����ʱ��
	request["sort"] = "eventTime"; request["order"] = "asc";
	QJsonObject orgMtMsg;//���¸ò������������
	orgMtMsg["orgName"] = NULL;//��������
	orgMtMsg["contractWrH"] = NULL;//��ͬ����ʱ
	orgMtMsg["contractPsN"] = NULL;//��ͬ����
	orgMtMsg["attendanceRatio"] = NULL;//������
	orgMtMsg["turnoverRate"] = NULL;//��ת��
	orgMtMsg["actualWkT"] = NULL;//ʵ�ʹ���ʱ��
	orgMtMsg["data"] = NULL;//ʵ������
	orgMtMsg["count"] = NULL;//��������
	orgMtMsg["departId"] = orgCode;//���ŵ�ID
	double actulWkh=0, contractWkh=0;//ʵ�ʹ���ʱ���ͬ����ʱ
	int actualPsN, contractPsN;//��ͬ������ʵ������
	QJsonArray dataDetail;
	for (auto var : orgObject)
	{
		if (var.isObject())
		{
			QJsonObject &&ob = var.toObject();
			if (ob["orgIndexCode"] == orgCode)
			{
				orgMtMsg["orgName"] =ob["orgName"];//��������
				contractPsN = ob["orgEmployeesNumber"].toInt();//�ò��ŵĺ�ͬ����
				QJsonArray &&personList = ob["employeelist"].toArray();//ÿ�����ŵ���Ա��Ϣ�б�
				actualPsN = personList.size();//ʵ������
				for (auto var= personList.begin();var!=personList.end();var++)
				{
					//qDebug() << *var << endl << "--------------------------------------------------" << endl;
					auto temp = (*var).toObject();
					temp["request"] = request;
					*var= temp;//��request�������ÿ���˵�body��
					//qDebug() << *var << endl << "--------------------------------------------------" << endl;
				}
				QFuture<QJsonObject> f = QtConcurrent::mapped(personList, producePersonHistory);
				f.waitForFinished();
				QList<QJsonObject> returnData=f.results();//�߳�������������
				qDebug()<< "---------------f"<< returnData.first();
				for (auto var : returnData)
				{
					double val=var["attendenceHours"].toString().toDouble();
					actulWkh += val;
					dataDetail.append(var);
				}
				returnData.clear();//�ͷ��ڴ�
				break;
			}
		}
	}
	contractWkh = weekDays * 8 * contractPsN;
	orgMtMsg["contractWrH"] = contractWkh;
	orgMtMsg["contractPsN"] = contractPsN;
	orgMtMsg["attendanceRatio"] = QString::number((actulWkh / contractWkh)*100, 'f', 2)+"%";//������
	orgMtMsg["turnoverRate"] = QString::number((1 - double(actualPsN / contractPsN)) * 100, 'f', 2) + "%";//��ת��
	orgMtMsg["actualWkT"] = QString::number(actulWkh,'f',1);//ʵ�ʹ���ʱ��
	orgMtMsg["data"] = dataDetail;
	orgMtMsg["count"] = dataDetail.size();
	orgMtMsg["updatetime"] = QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss");
	//fileName = appPath + "/Data/History/" + startDate + ".data";
	QString fileName = orgCode + ".data";
	writeOrgHistoryJson(fileName, orgMtMsg, startDate,orgCode);//������д��json�ĵ���
}
//ʵʱ���ݴ���
void HikdataRequest::realtimeDataDeal()
{
	qDebug() << "*******************************Processing RealTime Data********************************";
	QJsonObject request, OrgDoc, writeObeject, orgRealObject; 
	OrgDoc = DataStore::getOrgnizeData();//��ȡ����
	QJsonArray writeArray, writeOrgReal;
	//request["startTime"] = QDateTime::currentDateTime().addDays(-2).toString("yyyy-MM-ddThh:mm:ss+08:00");//��ѯ��ʼʱ��
	request["startTime"] = QDate::currentDate().toString("yyyy-MM-dd")+QString("T00:00:00+08:00");//��ѯ��ʼʱ��
	//qDebug() << request["startTime"].toString();
	request["endTime"] = QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss+08:00"); //��ѯ����ʱ��
	//qDebug() << request["endTime"].toString();
	//request["eventType"] = 196893;//�¼�����
	request["sort"] = "eventTime";//��ʱ��˳������
	request["order"] = "desc";//����
	request["pageNo"] = 1;//��һҳ
	request["pageSize"] = 1;//���µ�һ����¼
	for (auto orgIndex : OrgDoc.keys())//for departs
	{
		auto orginfo = OrgDoc[orgIndex].toObject();
		QJsonArray personList = orginfo["employeelist"].toArray();
		auto orgEmployeesNumber = orginfo["orgEmployeesNumber"].toInt();
		auto islocal = orginfo["islocal"].toBool();
		auto orgName = orginfo["orgName"].toString();
		qDebug() << "*****************Deal"<<orgName <<" RealTime Data*************"<<endl;
		for (auto var = personList.begin(); var != personList.end(); var++)//for person
		{
			request["eventType"] = 196893;//�¼�����ˢ��
			producePersonRealTime((*var).toObject(),request, orgIndex, orgName, orgEmployeesNumber, islocal);
			request["eventType"] = 198914;//�¼�����ˢ��
			producePersonRealTime((*var).toObject(), request, orgIndex, orgName, orgEmployeesNumber, islocal);
			DataStore::calcDepartPersonNum(orgIndex);
		}  
		//if(personList.size()!=0)
			
	}
	qDebug() << "**************************************************************************" << endl;
	qDebug() << "***************************Realtime Update Finished!**********************" << endl;
	qDebug() << "**************************************************************************" << endl;
}
//void HikdataRequest::calcDepartPersonNum(QString filename, QString orgIndex)
//{  
//	rtimefilelocak->lockForRead();
//	QJsonObject OrgDoc = openJsonDocument(filename);
//	rtimefilelocak->unlock();
//	QJsonObject orginfo = OrgDoc[orgIndex].toObject();
//	int realnum = 0;
//	for (auto personIndexs : orginfo.keys())
//	{
//		if (orginfo[personIndexs].isObject()) {
//			if (orginfo[personIndexs].toObject()["iostate"] == "in") {
//				realnum++;
//			}
//		}
//	}
//	orginfo["realnumber"] = realnum;
//	OrgDoc[orgIndex] = orginfo;
//	rtimefilelocak->lockForWrite();
//	writeJsonDocument(filename, OrgDoc);
//	rtimefilelocak->unlock();
//}
//����ӿڷ���
QJsonObject HikdataRequest::httpRequest(Unit_query_t * query, string Interface_address, QJsonObject requestbody)
{
	std::stringstream ss;
	ss << "https://" << query->artemisIp << ":" << query->artemisPort << Interface_address;//����szUrl
	std::string szUrl = ss.str();
	//����������Response����������װ����ͷ
	map<string, string> headers;
	headers.insert(std::make_pair(HttpHeader::HTTP_HEADER_ACCEPT, "application/json"));
	headers.insert(std::make_pair(HttpHeader::HTTP_HEADER_CONTENT_TYPE, "application/json;charset=UTF-8"));
	//����ʱʱ�����Զ������ǩ�������б�
	int iTimeOut = 10;
	list<string> signHeaderPrefixList;
	//��װbody(�˴�ֱ����װ�ַ�����Ҳ����ʹ��json����װ�����Ľϸ���ʱ������ʹ��json����װ)
	QJsonDocument document;
	document.setObject(requestbody);
	string szResponse = HttpPost(szUrl, headers, document.toJson().toStdString(),query->appKey, query->appSecret, iTimeOut, signHeaderPrefixList);
	if (szResponse.size() > 0)
		return string2json(szResponse);//���������json���ݶ���,תΪQJsonObject
	else
		return QJsonObject();
}

