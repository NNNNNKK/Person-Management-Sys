#include "connectionpool.h"
#include <QDebug>

QMutex ConnectionPool::mutex;
QAtomicPointer<ConnectionPool> ConnectionPool::m_instance=nullptr;
QWaitCondition ConnectionPool::waitConnection;

ConnectionPool::ConnectionPool() {
    // �������ݿ����ӵ���Щ��Ϣ��ʵ�ʿ�����ʱ����Ҫͨ����ȡ�����ļ��õ���
    // ����Ϊ����ʾ��������д�����˴����
  //  QString str=QDir::currentPath()+"/Profile/config.ini";
    //QSettings setting(str, QSettings::IniFormat);//�������ļ� ��д���·����һ��������·����
   // setting.beginGroup("DataBaseServer");

    //QString IP = setting.value("IP").toString();
   // QString DataBase = setting.value("DataBase").toString();
    //QString UserName = setting.value("UserName").toString();
    //QString PassWord = setting.value("PassWord").toString();
  //  int     Port=setting.value("Port").toInt();
  //  hostName     = IP;
    //port=Port;
    databaseName = "tset.db";
    //username     = UserName;
   // password     = PassWord;
    databaseType = "QSQLITE";
    testOnBorrow = true;
    testOnBorrowSql = "SELECT 1";
    maxWaitTime  = 1000;
    waitInterval = 200;
    maxConnectionCount  = 5;
}
ConnectionPool::~ConnectionPool() {
    // �������ӳص�ʱ��ɾ�����е�����
    foreach(QString connectionName, usedConnectionNames) {
        QSqlDatabase::removeDatabase(connectionName);
    }

    foreach(QString connectionName, unusedConnectionNames) {
        QSqlDatabase::removeDatabase(connectionName);
    }
}

ConnectionPool& ConnectionPool::getInstance()
{
//    if (nullptr == instance)
//    {
//        QMutexLocker locker(&mutex);
//        if (nullptr == instance)
//        {
//            instance = new ConnectionPool();
//        }
//    }
//    return *instance;
#ifndef Q_ATOMIC_POINTER_TEST_AND_SET_IS_ALWAYS_NATIVE
     if(!QAtomicPointer<ConnectionPool>::isTestAndSetNative())
         qDebug()<<"Error: TestAndSetNative not supported!";
#endif
     {
         QMutexLocker locker(&mutex);
         if(m_instance.testAndSetOrdered(nullptr,nullptr))
         {
             m_instance.testAndSetOrdered(nullptr,new ConnectionPool);
         }
         return *m_instance.load();
     }
}

void ConnectionPool::release() {
    QMutexLocker locker(&mutex);
    delete m_instance;
   m_instance = nullptr;
}

QSqlDatabase ConnectionPool::openConnection() {
    ConnectionPool& pool = ConnectionPool::getInstance();
    QString connectionName;

    QMutexLocker locker(&mutex);

    // �Ѵ���������
    int connectionCount = pool.unusedConnectionNames.size() + pool.usedConnectionNames.size();

    // ��������Ѿ����꣬�ȴ� waitInterval ���뿴���Ƿ��п������ӣ���ȴ� maxWaitTime ����
    for (int i = 0;
         i < pool.maxWaitTime
         && pool.unusedConnectionNames.size() == 0 && connectionCount == pool.maxConnectionCount;
         i += pool.waitInterval) {
        waitConnection.wait(&mutex, pool.waitInterval);

        // ���¼����Ѵ���������
        connectionCount = pool.unusedConnectionNames.size() + pool.usedConnectionNames.size();
    }

    if (pool.unusedConnectionNames.size() > 0)
    {
        // ���Ѿ����յ����ӣ���������
        connectionName = pool.unusedConnectionNames.dequeue();
    } else if (connectionCount < pool.maxConnectionCount) {
        // û���Ѿ����յ����ӣ�����û�дﵽ������������򴴽��µ�����
        connectionName = QString("Connection-%1").arg(connectionCount + 1);
    } else {
        // �Ѿ��ﵽ���������
        //qDebug() << "Cannot create more connections.";
        return QSqlDatabase();
    }

    // ��������
    QSqlDatabase db = pool.createConnection(connectionName);

    // ��Ч�����Ӳŷ��� usedConnectionNames
    if (db.isOpen()) {
        pool.usedConnectionNames.enqueue(connectionName);
    }
    return db;
}

void ConnectionPool::closeConnection(QSqlDatabase connection) {
    ConnectionPool& pool = ConnectionPool::getInstance();
    QString connectionName = connection.connectionName();

    // ��������Ǵ��������ӣ��� used ��ɾ�������� unused ��
    if (pool.usedConnectionNames.contains(connectionName)) {
        QMutexLocker locker(&mutex);
        pool.usedConnectionNames.removeOne(connectionName);
        pool.unusedConnectionNames.enqueue(connectionName);
        waitConnection.wakeOne();
    }
}

QSqlDatabase ConnectionPool::createConnection(const QString &connectionName)
{
    // �����Ѿ��������ˣ������������������´���
    if (QSqlDatabase::contains(connectionName))
    {
        QSqlDatabase db1 = QSqlDatabase::database(connectionName);

        if (testOnBorrow) {
            // ��������ǰ�������ݿ⣬������ӶϿ������½�������
            //qDebug() << "Test connection on borrow, execute:" << testOnBorrowSql << ", for" << connectionName;
            QSqlQuery query(testOnBorrowSql, db1);

            if (query.lastError().type() != QSqlError::NoError && !db1.open()) {
               // qDebug() << "Open datatabase error:" << db1.lastError().text();
                return QSqlDatabase();
            }
        }

        return db1;
    }

    // ����һ���µ�����
    QSqlDatabase db = QSqlDatabase::addDatabase(databaseType, connectionName);
  //  db.setHostName(hostName);
    db.setDatabaseName(databaseName);
  //  db.setUserName(username);
   // db.setPassword(password);
    //db.setPort(port);
    if (!db.open()) {
        qDebug() << "Open datatabase error:" << db.lastError().text();
        return QSqlDatabase();
    }

    return db;
}
