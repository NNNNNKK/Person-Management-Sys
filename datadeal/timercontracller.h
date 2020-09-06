#ifndef TIMERCONTRACLLER_H
#define TIMERCONTRACLLER_H
#include"qobject.h"
#include"qtimer.h"
#include"datadeal/hikdatarequest.h"
#include"qfuture.h"
#include"qtconcurrent"
class Timercontracller:public QObject
{
	Q_OBJECT
public:
	Timercontracller(QObject* parent = nullptr);
	~Timercontracller();
	void startO_timer();//开启定时器
	void stopO_timer();//关闭定时器
	void startR_timer();//开启定时器
	void stopR_timer();//关闭定时器
private slots:
	void dealRealData();
	void dealOrgList();
private:
	QTimer *o_timer;//部门定时器
	QTimer *r_timer;//实时数据定时器
	HikdataRequest *h_rquest;
};

#endif