#ifndef SYSTEMCONFIG_H
#define SYSTEMCONFIG_H
#include <QVariant>
#include <QSettings>

class SystemConfig
{
public:
    SystemConfig(QString qstrfilename = "");
    virtual ~SystemConfig(void);
    void Set(QString qstrnodename, QString qstrkeyname, QVariant qvarvalue);
    QVariant Get(QString qstrnodename, QString qstrkeyname);
private:
    QString m_qstrFileName;
    QSettings *m_psetting;

};

#endif // SYSTEMCONFIG_H
