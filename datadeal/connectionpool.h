#ifndef CONNECTIONPOOL_H
#define CONNECTIONPOOL_H

#include <QtSql>
#include <QQueue>
#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include<QAtomicPointer>
class ConnectionPool {
public:
    static void release(); // �ر����е����ݿ�����
    static QSqlDatabase openConnection();                 // ��ȡ���ݿ�����
    static void closeConnection(QSqlDatabase connection); // �ͷ����ݿ����ӻ����ӳ�
    ~ConnectionPool();
    static ConnectionPool& getInstance();//������ʼ��
public:

    QSqlDatabase createConnection(const QString &connectionName); // �������ݿ�����
    QQueue<QString> usedConnectionNames;   // ��ʹ�õ����ݿ�������
    QQueue<QString> unusedConnectionNames; // δʹ�õ����ݿ�������
    // ���ݿ���Ϣ
    QString hostName;
    QString databaseName;
    QString username;
    QString password;
    QString databaseType;
    int port;
    bool    testOnBorrow;    // ȡ�����ӵ�ʱ����֤�����Ƿ���Ч
    QString testOnBorrowSql; // ���Է������ݿ�� SQL
    int maxWaitTime;  // ��ȡ�������ȴ�ʱ��
    int waitInterval; // ���Ի�ȡ����ʱ�ȴ����ʱ��
    int maxConnectionCount; // ���������
private:
    ConnectionPool();
    ConnectionPool(const ConnectionPool &other);
    ConnectionPool& operator=(const ConnectionPool &other);
    static QMutex mutex;
    static QWaitCondition waitConnection;
    static QAtomicPointer<ConnectionPool> m_instance;

};

#endif // CONNECTIONPOOL_H
