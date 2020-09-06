#include <QCoreApplication>
#include <QDir>
#include "httplistener.h"
#include "templatecache.h"
#include "httpsessionstore.h"
#include "staticfilecontroller.h"
#include "filelogger.h"
#include "requestmapper.h"
#include<QProcess>
#include<QSettings>
#include <iostream>
#include <sstream> 
#include "datadeal/systemconfig.h"
#include"qtimer.h"
#include"datadeal/hikdatarequest.h"
#include"qpointer.h"
#include"datadeal/timercontracller.h"
#include <iostream>

constexpr auto AUTO_RUN = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
using namespace std;
/** Cache for template files */
TemplateCache* templateCache;
/** Storage for session cookies */
HttpSessionStore *sessionStore;
/** Controller for static files */
StaticFileController* staticFileController;
/** Redirects log messages to a file */
FileLogger* logger;
//自启
void setAutoStart(bool is_auto_start)
{
	QString application_name = QCoreApplication::applicationName();
	QSettings *settings = new QSettings(AUTO_RUN, QSettings::NativeFormat);
	if (is_auto_start)
	{
		QString application_path = QCoreApplication::applicationFilePath();
		settings->setValue(application_name, application_path.replace("/", "\\"));
	}
	else
	{
		settings->remove(application_name);
	}
	delete settings;
}
/**存储调试日志*/
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	// 加锁
	static QMutex mutex;
	mutex.lock();
	QByteArray localMsg = msg.toLocal8Bit();
	QString strMsg("");
	switch (type)
	{
	case QtDebugMsg:
		strMsg = QString("Debug");
		break;
	case QtWarningMsg:
		strMsg = QString("Warning");
		break;
	case QtCriticalMsg:
		strMsg = QString("Critical");
		break;
	case QtFatalMsg:
		strMsg = QString("Fatal");
		break;
	}
	// 设置输出信息格式
	QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	QString strMessage = QString("Time:%1 Type:%2  Messge:%3  File:%4  Line:%5")
		.arg(strDateTime).arg(strMsg).arg(localMsg.constData()).arg(context.file).arg(context.line);
	// 输出信息至文件中（读写、追加形式）
	QFile file(QString("./logs/log-%1.txt").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd")));
	file.open(QIODevice::ReadWrite | QIODevice::Append);
	QTextStream stream(&file);
	stream << strMessage.toLocal8Bit() << "\r\n";
	//std::cout << strMessage.tolo << std::endl;
	file.flush();
	file.close();
	// 解锁
	mutex.unlock();
}
//订阅hik事件服务
bool subscribeHikevent()
{
	QJsonObject requestObject;//请求body
	QJsonArray eventTypes;//事件类型
	string apiAddress = "/artemis/api/eventService/v1/eventSubscriptionByEventTypes";//接口api
	eventTypes.append(196893);
	eventTypes.append(198914);
	requestObject["eventTypes"] = eventTypes;
	QString eventDest = "http://" + HikdataRequest::getIpv4() + ":9080" + "/CallBack";
	requestObject["eventDest"] = eventDest;
	requestObject["subType"] = 0;
	QJsonObject backMsg = HikdataRequest::httpRequest(HikdataRequest::gethikQuery(), apiAddress, requestObject);
	qDebug() << backMsg;
	if (backMsg.contains("code") && backMsg["code"].toInt() == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}void cancleHikevent()
{
	QJsonObject requestObject;//请求body
	QJsonArray eventTypes;//事件类型
	string apiAddress = "/artemis/api/eventService/v1/eventUnSubscriptionByEventTypes";//接口api
	eventTypes.append(198914);
	eventTypes.append(196893);
	requestObject["eventTypes"] = eventTypes;
	QJsonObject backMsg = HikdataRequest::httpRequest(HikdataRequest::gethikQuery(), apiAddress, requestObject);
	if (backMsg.contains("code") && backMsg["code"].toInt() != 0)
	{
		qDebug("cancle falied!");
	}
	qDebug() << backMsg;
}

//主函数
QReadWriteLock* rtimefilelocak;
int main(int argc, char *argv[])
{

	// 安装消息处理程序  
	rtimefilelocak = new QReadWriteLock();
	QCoreApplication app(argc, argv);
	app.setApplicationName("HttpServer");
	app.setOrganizationName("Butterfly");
	QCommandLineParser parser;
	// A boolean option with a single name (-p)
	QCommandLineOption showProgressOption(
		"l",
		QCoreApplication::translate("main", "Save log to file."));
	parser.addOption(showProgressOption);
	parser.process(app);
	bool saveLog = parser.isSet(showProgressOption);
	if (saveLog) {
		qInstallMessageHandler(myMessageOutput);
	}

	QString AppPath = QCoreApplication::applicationDirPath();
	QString configFilePath = AppPath + "/Profile/config.ini";//配置文件路径
	SystemConfig appconfig(configFilePath);
	//设置IP地址、端口号、APPKey和APPSecret
	Unit_query_t query;
	query.appKey = appconfig.Get("AppConfig", "appkey").toString().toStdString();                // 合作方APPKey
	query.appSecret = appconfig.Get("AppConfig", "appSecret").toString().toStdString();         // 合作方APPSecret
	query.artemisIp = appconfig.Get("AppConfig", "artemisIp").toString().toStdString();         // 平台IP地址
	query.artemisPort = appconfig.Get("AppConfig", "artemisPort").toInt();  // HTTP或HTTPS协议端口，
	//需要根据实际使用HTTP还是HTTPS协议来填充，此处使用HTTPS协议
	HikdataRequest::sethikQueryPointer(&query);
	HikdataRequest dataRequest(AppPath);//初始化海康数据请求类
	dataRequest.writeOrganization();//获取部门
	QFuture<void> f = QtConcurrent::run(HikdataRequest::realtimeDataDeal);//实时数据处理

	Timercontracller timercontracller;
	timercontracller.startO_timer();//定时器开启，定时刷新部门列表及其人数
	timercontracller.startR_timer();
	QString configFileName = AppPath + "/etc/systemset.ini";
	// Configure template loader and cache
	QSettings* templateSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
	templateSettings->beginGroup("templates");
	templateCache = new TemplateCache(templateSettings, &app);
	// Configure session store
	QSettings* sessionSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
	sessionSettings->beginGroup("sessions");
	sessionStore = new HttpSessionStore(sessionSettings, &app);
	// Configure logging into a file
	QSettings* logSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
	logSettings->beginGroup("logging");
	FileLogger* logger = new FileLogger(logSettings, 10000, &app);
	//logger->installMsgHandler();
	// Configure static file controller
	QSettings* fileSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
	fileSettings->beginGroup("docroot");
	staticFileController = new StaticFileController(fileSettings, &app);
	// Configure and start the TCP listener
	QSettings* listenerSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
	listenerSettings->beginGroup("listener");
	new HttpListener(listenerSettings, new RequestMapper(&app), &app);

	cancleHikevent();
	//订阅门禁事件
	if (!subscribeHikevent())
		qDebug("errr:subscribe hik event failed");
	qWarning("Application has started");
	app.exec();
	qWarning("Application has stopped");
	logger->clear();
}
