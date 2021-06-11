#ifndef ITUNESSERVICE_H
#define ITUNESSERVICE_H

#include <QObject>

class iTunesServiceCheck : public QObject
{
    Q_OBJECT
public:
    explicit iTunesServiceCheck(QObject *parent = nullptr);
    int InitEnvironment();
    bool servicIsRunning();
    bool checkAppleAppSupport();
    bool checkAppleMDeviceSupport();
private:
    bool checkReg(const QString& strKey, const QString& strName, wchar_t* InstallDir);
signals:

};

#endif // ITUNESSERVICE_H
