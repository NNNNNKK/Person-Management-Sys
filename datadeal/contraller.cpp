#include "contraller.h"
#include<QJsonObject>
#include<QFile>
#include"departset.h"
#include"systemconfig.h"

//读取各个区域的读卡器
QVariant Contraller::GetEveryArea_CardReader()
{
    QList<QJsonArray> alllist;//依次存入所有读卡器、工作区进出读卡器、生活区进出读卡器
    QString filename=QCoreApplication::applicationDirPath()+"/Profile/AreaReader.json";//区域读卡器路径
    QJsonDocument jsonDoc=readJsonDocument(filename);//读取配置文件
    if(jsonDoc.isEmpty())
    {
        qDebug()<<filename+"文件中读卡器配置为空!";
        return QVariant();
    }
    QJsonObject rootObj = jsonDoc.object();
    QJsonArray Card_reader=rootObj.find("Card_reader").value().toArray();
    QJsonArray Living_area_In=rootObj.find("Living_area").value().toObject().value("In").toArray();//生活区进门读卡器
    QJsonArray Living_area_Out=rootObj.find("Living_area").value().toObject().value("Out").toArray();//生活区进门读卡器
    QJsonArray Working_area_In=rootObj.find("Working_area").value().toObject().value("In").toArray();//工作区进门读卡器
    QJsonArray Working_area_Out=rootObj.find("Working_area").value().toObject().value("Out").toArray();//工作区出门读卡器
    alllist.append(Card_reader);
    alllist.append(Living_area_In);
    alllist.append(Living_area_Out);
    alllist.append(Working_area_In);
    alllist.append(Working_area_In);
    QVariant var;
    var.setValue<QList<QJsonArray>>(alllist);
    return var;
}
Contraller::Contraller()
{
    init();//初始化各个区域读卡器
}
 Contraller::~Contraller()
{

}
 //读取Json配置文件(把文件转化为QJsonDocument)
 QJsonDocument Contraller::readJsonDocument(QString filename)
 {
     QFile loadfile(filename);
     if(!loadfile.open(QIODevice::ReadOnly))
         {
             qDebug()<<"could't open projects json";
             return QJsonDocument();
         }
     QByteArray allData = loadfile.readAll();
     loadfile.close();
     QJsonParseError json_error;
     QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &json_error));
         if(json_error.error != QJsonParseError::NoError)
         {
             qDebug() << "json error!"<<endl;
             return QJsonDocument();
         }
     return jsonDoc;
 }
 void Contraller::initContractorNumber(int dept_id,QString filename,QString depart_name)
 {
     departset config;
     QJsonArray jsonArray=config.openJsonDocument(filename);
     if(jsonArray.size()==0)
     {
         QJsonObject object;
         object["Id"]=dept_id;
         object["DepartName"]=depart_name;
         object["ContractorNumber"]=50;
         jsonArray.append(object);
     }
     else
     {
         QList<int> listId;
         for(auto var:jsonArray)
             if(var.isObject())
                listId.append(var.toObject()["Id"].toInt());
         if(!listId.contains(dept_id))
         {
             QJsonObject object;
             object["Id"]=dept_id;
             object["DepartName"]=depart_name;
             object["ContractorNumber"]=50;
             jsonArray.append(object);
         }
     }
     QFile loadfile(filename);
     if(!loadfile.open(QIODevice::WriteOnly))
         {
             qDebug()<<"could't open json projects";
         }
     QJsonDocument jsonDoc;
     jsonDoc.setArray(jsonArray);
     loadfile.write(jsonDoc.toJson());
     loadfile.close();
 }
 //初始化
void Contraller::init()
{
    filePath=QCoreApplication::applicationDirPath();//路径
    var=GetEveryArea_CardReader();
    allReader=var.value<QList<QJsonArray>>().at(0);//所有读卡器列表
    lifeAreaCardReader_In=var.value<QList<QJsonArray>>().at(1);//生活区进入读卡器
    lifeAreaCardReader_Out=var.value<QList<QJsonArray>>().at(2);//生活区出门读卡器
    workAreaCardReader_In=var.value<QList<QJsonArray>>().at(3);//工作区进门读卡器
    workAreaCardReader_Out=var.value<QList<QJsonArray>>().at(4);//工作区出门读卡器
}
