#include"hikdatarequest.h"
#include"qfuture.h"
#include"qtconcurrent"
#include"GloableLocker.h"
#include<qtextcodec.h>
Unit_query_t* HikdataRequest::hikQuery = nullptr;

HikdataRequest::HikdataRequest( QString path, QObject *parent):appPath(path)
{
}
//字符串转为json
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
//获取IPV4地址
QString HikdataRequest::getIpv4()
{
	QString strIpAddress;
	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	// 获取第一个本主机的IPv4地址
	int nListSize = ipAddressesList.size();
	for (int i = 0; i < nListSize; ++i)
	{
		if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
			ipAddressesList.at(i).toIPv4Address()) {
			strIpAddress = ipAddressesList.at(i).toString();
			break;
		}
	}
	// 如果没有找到，则以本地IP地址为IP
	if (strIpAddress.isEmpty())
		strIpAddress = QHostAddress(QHostAddress::LocalHost).toString();
	return strIpAddress;
}
//写历史部门数据
void HikdataRequest::writeOrgHistoryJson(QString & filename, QJsonObject & object, QString & date,const QString &orgIndex)
{      
	  QReadWriteLock lock;
	  lock.lockForWrite();
	  QString dirPath = appPath + "/Data/HistoryData/" + date;
	  QDir dir(dirPath);
		if (!dir.exists())
			dir.mkpath(dirPath);
		QString fileName = dirPath + "/" + filename;//文件路径名，包含文件名
		QJsonObject root;
		root[orgIndex] = object;
		DataStore::writeJsonDocument(fileName, root);
		lock.unlock();
}
//给hikQuery设值
void HikdataRequest::sethikQueryPointer(Unit_query_t * pointer)
{
	if (pointer != nullptr)
		hikQuery = pointer;
	else
		qDebug("pointer is nullptr");
}
//合同工作日
int HikdataRequest::contractHours(QString startdate)
{
	QString currentDate = QDate::currentDate().toString("yyyyMM");
	QDate beginDate = QDate::fromString(startdate + "01", "yyyyMMdd");
	QDate endDate;
	//判断起始月份是否为当前月份
	if (currentDate == startdate)
		endDate = QDate::currentDate();
	else
		endDate = QDate::fromString(startdate + "01", "yyyyMMdd").addMonths(1);
	//判断开始时间是否小于结束时间
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
//析构函数
HikdataRequest::~HikdataRequest()
{
	//delete hikQuery;
}
//获取所有部门列表
QJsonArray HikdataRequest::getTotalOrg()
{   
	string Interface_address = "/artemis/api/resource/v1/org/orgList";
	QJsonObject requestobject;
	QJsonArray orgArray;//要返回的部门列表数目
	requestobject["pageNo"] = 1;
	requestobject["pageSize"] = 1000;
	QJsonObject responseJson=httpRequest(hikQuery, Interface_address, requestobject);

	QString path = QCoreApplication::applicationDirPath();
	QString filename = path + "/Data/Config/orgblacklist.data";
	QJsonArray balckList = DataStore::openJsonDocument("filename")["black"].toArray();//获取父组织黑名单
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
//写组织及其员工
void  HikdataRequest::writeOrganization()
{
	qDebug("process depart Information."); 
	QJsonObject   oldOrglist = DataStore::getOrgnizeData();//获取旧的部门信息
	QStringList oldIndexCodes=oldOrglist.keys();//获取旧部门的唯一标识码
	QStringList newIndexCodes;//新数据部门的位移标识码
	QJsonArray orgArray = getTotalOrg();//获取isc中的所有的组织列表
	for (int i = 0; i < orgArray.size(); i++)
	{
		QJsonObject getedOrg = orgArray.at(i).toObject();
		getedOrg.remove("updateTime");
		getedOrg.remove("parentOrgIndexCode");
		QString orgIndexCode = getedOrg.value("orgIndexCode").toString();//组织唯一标识码
		newIndexCodes.append(orgIndexCode);
		//获取每个组织下的人员信息
		QJsonObject request;
		request["orgIndexCode"] = orgIndexCode;
		request["pageNo"] = 1;
		request["pageSize"] = 1000;
		string api = "/artemis/api/resource/v2/person/orgIndexCode/personList";
		QJsonObject backData=httpRequest(hikQuery, api, request);
		//数据获取成功后执行的操作
		if (backData.contains("code") && backData["code"].toInt() == 0)
		{
			QJsonArray employeelist;
			QJsonArray backlist = backData["data"].toObject()["list"].toArray();
			for (auto var : backlist)
			{
				if (var.isObject())
				{
					QJsonObject ob = var.toObject();
					ob.remove("personPhoto");//删除个人照片信息
					ob.remove("createTime");//删除创建的时间
					ob.remove("updateTime");//删除更新时间
					ob.remove("orgPath");
					ob.remove("orgPathName");
					ob["orgName"] = getedOrg["orgName"];//部门名字
					employeelist.append(ob);
				}
			}
			auto ite = oldOrglist.find(orgIndexCode);
			if (ite == oldOrglist.end())//未找到该组织
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
	DataStore::deleteoldData(newIndexCodes);//删除部门和实时数据中已经不存在部门的数据
	qDebug("update depart Information finished.");
}
 
//生成每个人的数据

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
	pDetail["personName"] = pMs["personName"].toString();//人员姓名
	pDetail["cardNumber"] = pMs["jobNo"].toString();//人员的卡号
	pDetail["attendenceHours"] = double(0);//人员累计工作时间
	pDetail["personId"] = pMs["personId"].toString();//人员id
	pDetail["orgindex"] = pMs["orgIndexCode"];
	pDetail["orgName"] = pMs["getedOrg"];
	pDetail["detail"]=QJsonArray();
	if (personevent.contains("code")&&personevent["code"].toString() == QString::number(0))
	{
		backData = personevent["data"].toObject()["list"].toArray();
		int totalPage = personevent["data"].toObject()["totalPage"].toInt();//总共的页数
		for (int i = 2; i <= totalPage; i++)
		{
			request["pageNo"] = i;
			QJsonArray subData = HikdataRequest::httpRequest(HikdataRequest::gethikQuery(), Interface_address, request)["data"].toObject()["list"].toArray();
			for (auto var : subData)
				backData.append(var.toObject());
		}
		bool enterflag = false;//进入标志位
		double workSecs=0;//该部门该月实际工作时间
		QJsonArray detailMsg;//进入具体的信息
		for (auto var : backData)
		{ 
			//qDebug() << var << endl << "---------------------------------" << endl;
			QJsonObject hisMsg,varOb= var.toObject();
			hisMsg["personName"] = varOb["personName"];hisMsg["doorName"] = varOb["doorName"];hisMsg["eventTime"] = varOb["eventTime"];
			detailMsg.append(hisMsg);//放入jsonarray
			QString doorName = hisMsg["doorName"].toString();//进还是出的标志
			int backEnterType = doorName.mid(doorName.size() - 1,1).toInt();
			QString httpbacktime = hisMsg["eventTime"].toString();//http返回的事件的时间
			QString time = httpbacktime.mid(0, 10) + " " + httpbacktime.mid(11, 8);
			/*判断进入标志是否为真*/
			if (enterflag)
			{   //已经进入，判断该事件是否为出
				if (backEnterType == 2){
					QDateTime leaveTime = QDateTime::fromString(httpbacktime, "yyyy-MM-ddThh:mm:ss+08:00");
					workSecs += entertime.secsTo(leaveTime);//求相邻进出打卡之间的时间差,并累加起来
					enterflag = false;//令进入标志位为假
				}continue;//事件连续为进，则不处理
			}
			//进入标志为false，判断是事件是否为进
			else
			{
				if (backEnterType == 1){
					enterflag = true; entertime = QDateTime::fromString(httpbacktime, "yyyy-MM-ddThh:mm:ss+08:00");
				} continue;//事件连续一直为出，不处理
			}
		}
		pDetail["detail"] = detailMsg;
		pDetail["attendenceHours"]= QString::number(workSecs/float(3600),'f',2);//计算该月该部门所有人员的工作时长,保留2位有效数字
	}
	return pDetail;
}
//实时人员进入信息处理
//static QReadWriteLock rlock;
void producePersonRealTime(QJsonObject ob, QJsonObject request, QString orgIndex, QString orgName, int contractor, bool islocal)
{
	QJsonArray  pCodes;
	pCodes.append(ob["personId"].toString());
	request["personIds"] = pCodes;
	string Interface_address = "/artemis/api/acs/v1/door/events";
	QJsonObject personevent = HikdataRequest::httpRequest(HikdataRequest::gethikQuery(), Interface_address, request);
	QJsonObject personMsg;//个人进入信息
	DoorDirection status;
	DoorDirection doorType;
	QString doorName;
	if (personevent.contains("code") && personevent["code"].toInt() == 0)
	{
		QJsonArray list = personevent["data"].toObject()["list"].toArray();//获取数据list
		QJsonObject detail = list.first().toObject();//最新记录
		if (detail.isEmpty())
		{
			personMsg["personName"] = ob["personName"].toString();//姓名
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
			personMsg["personName"] = ob["personName"].toString();//姓名
			QString eventTime = detail["eventTime"].toString();//进入时间
			personMsg["eventTime"] = eventTime.mid(0,10)+" "+ eventTime.mid(11, 8);
			personMsg["eventPlace"] = detail["doorName"].toString();//打卡的门
		}
		if (doorType == DoorDirection::Import)
		{
			status = DoorDirection::Import;
			personMsg["iostate"] = "in";//打卡的门
		}
		else if(doorType == DoorDirection::Export || doorType == DoorDirection::Unused){
			status = DoorDirection::Export;
			personMsg["iostate"] = "out";//打卡的门
		}	
	}
	else {
		//api error
		personMsg["personName"] = ob["personName"].toString();//姓名
		personMsg["eventTime"] = QString(QString::fromLocal8Bit("select Failed").toUtf8());
		personMsg["eventPlace"] = QString(QString::fromLocal8Bit("select Failed").toUtf8());
		personMsg["iostate"] = "out";//打卡的门
		status = DoorDirection::Export;
	}
	QString dirPath = QCoreApplication::applicationDirPath() + "/Data/RealTime/";
	QDir path(dirPath);
	if (!path.exists())
		path.mkpath(dirPath);
	QString dir = "";//写的文件路径 
	writePersonRealtimemsg(dir, orgIndex, personMsg, status,orgName, contractor,islocal, ob["personId"].toString()); 
}
//实时写人的文件
void writePersonRealtimemsg(QString &filename,QString  orgCode,QJsonObject &personMsg, DoorDirection status,QString orgName,int contractor,bool islocal,QString pId)
{
	QJsonObject  && ob = DataStore::getRealTimeData();//打开原来数据
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

//按部门进行历史数据处理函数
void HikdataRequest::historyDataDeal(QString startDate, QString orgCode)
{
	//获取一个月的工作日的天数
	int weekDays = contractHours(startDate);
	QJsonObject writeObeject;
	QString selectDate = startDate.mid(0, 4) + "-" + startDate.mid(4, 2) + "-01T00:00:00+08:00";//查询时间初始化
	QJsonObject orgObject = DataStore::getOrgnizeData();//读取部门信息
	QJsonObject request; request["eventType"] = 196893;//事件类型,即人脸认证通过
	request["startTime"] = selectDate;//查询起始时间 
	request["endTime"] = QDateTime::fromString(selectDate, "yyyy-MM-ddThh:mm:ss+08:00").addMonths(1).addDays(-1).toString("yyyy-MM-ddThh:mm:ss+08:00") + "";//查询结束时间
	request["sort"] = "eventTime"; request["order"] = "asc";
	QJsonObject orgMtMsg;//该月该部门整体的数据
	orgMtMsg["orgName"] = NULL;//部门名字
	orgMtMsg["contractWrH"] = NULL;//合同工作时
	orgMtMsg["contractPsN"] = NULL;//合同人数
	orgMtMsg["attendanceRatio"] = NULL;//出勤率
	orgMtMsg["turnoverRate"] = NULL;//流转率
	orgMtMsg["actualWkT"] = NULL;//实际工作时间
	orgMtMsg["data"] = NULL;//实际数据
	orgMtMsg["count"] = NULL;//数据数量
	orgMtMsg["departId"] = orgCode;//部门的ID
	double actulWkh=0, contractWkh=0;//实际工作时与合同工作时
	int actualPsN, contractPsN;//合同人数与实际人数
	QJsonArray dataDetail;
	for (auto var : orgObject)
	{
		if (var.isObject())
		{
			QJsonObject &&ob = var.toObject();
			if (ob["orgIndexCode"] == orgCode)
			{
				orgMtMsg["orgName"] =ob["orgName"];//部门名字
				contractPsN = ob["orgEmployeesNumber"].toInt();//该部门的合同人数
				QJsonArray &&personList = ob["employeelist"].toArray();//每个部门的人员信息列表
				actualPsN = personList.size();//实际人数
				for (auto var= personList.begin();var!=personList.end();var++)
				{
					//qDebug() << *var << endl << "--------------------------------------------------" << endl;
					auto temp = (*var).toObject();
					temp["request"] = request;
					*var= temp;//把request请求放入每个人的body中
					//qDebug() << *var << endl << "--------------------------------------------------" << endl;
				}
				QFuture<QJsonObject> f = QtConcurrent::mapped(personList, producePersonHistory);
				f.waitForFinished();
				QList<QJsonObject> returnData=f.results();//线程运行完后的数据
				qDebug()<< "---------------f"<< returnData.first();
				for (auto var : returnData)
				{
					double val=var["attendenceHours"].toString().toDouble();
					actulWkh += val;
					dataDetail.append(var);
				}
				returnData.clear();//释放内存
				break;
			}
		}
	}
	contractWkh = weekDays * 8 * contractPsN;
	orgMtMsg["contractWrH"] = contractWkh;
	orgMtMsg["contractPsN"] = contractPsN;
	orgMtMsg["attendanceRatio"] = QString::number((actulWkh / contractWkh)*100, 'f', 2)+"%";//出勤率
	orgMtMsg["turnoverRate"] = QString::number((1 - double(actualPsN / contractPsN)) * 100, 'f', 2) + "%";//流转率
	orgMtMsg["actualWkT"] = QString::number(actulWkh,'f',1);//实际工作时长
	orgMtMsg["data"] = dataDetail;
	orgMtMsg["count"] = dataDetail.size();
	orgMtMsg["updatetime"] = QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss");
	//fileName = appPath + "/Data/History/" + startDate + ".data";
	QString fileName = orgCode + ".data";
	writeOrgHistoryJson(fileName, orgMtMsg, startDate,orgCode);//把数据写入json文档中
}
//实时数据处理
void HikdataRequest::realtimeDataDeal()
{
	qDebug() << "*******************************Processing RealTime Data********************************";
	QJsonObject request, OrgDoc, writeObeject, orgRealObject; 
	OrgDoc = DataStore::getOrgnizeData();//获取部门
	QJsonArray writeArray, writeOrgReal;
	//request["startTime"] = QDateTime::currentDateTime().addDays(-2).toString("yyyy-MM-ddThh:mm:ss+08:00");//查询起始时间
	request["startTime"] = QDate::currentDate().toString("yyyy-MM-dd")+QString("T00:00:00+08:00");//查询起始时间
	//qDebug() << request["startTime"].toString();
	request["endTime"] = QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss+08:00"); //查询结束时间
	//qDebug() << request["endTime"].toString();
	//request["eventType"] = 196893;//事件类型
	request["sort"] = "eventTime";//按时间顺序排序
	request["order"] = "desc";//降序
	request["pageNo"] = 1;//第一页
	request["pageSize"] = 1;//最新的一条记录
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
			request["eventType"] = 196893;//事件类型刷脸
			producePersonRealTime((*var).toObject(),request, orgIndex, orgName, orgEmployeesNumber, islocal);
			request["eventType"] = 198914;//事件类型刷卡
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
//请求接口访问
QJsonObject HikdataRequest::httpRequest(Unit_query_t * query, string Interface_address, QJsonObject requestbody)
{
	std::stringstream ss;
	ss << "https://" << query->artemisIp << ":" << query->artemisPort << Interface_address;//请求szUrl
	std::string szUrl = ss.str();
	//根据期望的Response内容类型组装请求头
	map<string, string> headers;
	headers.insert(std::make_pair(HttpHeader::HTTP_HEADER_ACCEPT, "application/json"));
	headers.insert(std::make_pair(HttpHeader::HTTP_HEADER_CONTENT_TYPE, "application/json;charset=UTF-8"));
	//请求超时时间与自定义参与签名参数列表
	int iTimeOut = 10;
	list<string> signHeaderPrefixList;
	//组装body(此处直接组装字符串，也可以使用json来组装，报文较复杂时，建议使用json来组装)
	QJsonDocument document;
	document.setObject(requestbody);
	string szResponse = HttpPost(szUrl, headers, document.toJson().toStdString(),query->appKey, query->appSecret, iTimeOut, signHeaderPrefixList);
	if (szResponse.size() > 0)
		return string2json(szResponse);//返回请求的json数据对象,转为QJsonObject
	else
		return QJsonObject();
}

