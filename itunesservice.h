#ifndef ITUNESSERVICE_H
#define ITUNESSERVICE_H

#include <QObject>

class iTunesServiceCheck : public QObject
{
    Q_OBJECT
public:
    explicit iTunesServiceCheck(QObject *parent = nullptr);
    static bool IsWow64();
    static QString fileMd5(const QString &filePath);
    int InitEnvironment();
    bool servicIsRunning();
    bool checkApplicationSupport32();
    bool checkApplicationSupport64();
    bool checkMobileDeviceSupport32();
    bool checkMobileDeviceSupport64();

private:
    bool checkReg(const QString& strKey, const QString& strName, wchar_t* InstallDir, bool check32_Bit = false);
signals:

};

#endif // ITUNESSERVICE_H
