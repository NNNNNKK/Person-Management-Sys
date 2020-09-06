#ifndef CONTRALLER_H
#define CONTRALLER_H
#include"RealTimeDataDealThread.h"
class Contraller
{
public:
    explicit Contraller();
    //DataBaseThread *thread;
     QJsonArray allReader;//所有读卡器列表
     QJsonArray lifeAreaCardReader_In;//生活区进门读卡器
     QJsonArray lifeAreaCardReader_Out;//生活区出门读卡器
     QJsonArray workAreaCardReader_In;//工作区进门读卡器
     QJsonArray workAreaCardReader_Out;//工作区出门读卡器
     QVariant var;
     QVariant GetEveryArea_CardReader();
     QString filePath;
     QJsonDocument readJsonDocument(QString filename);
     void initContractorNumber(int dept_id,QString filename,QString depart_name);
     void init();
     ~Contraller();
signals:
public slots:

};

#endif // CONTRALLER_H
