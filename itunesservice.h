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
    bool checkAAS32();
    bool checkAAS64();
    bool checkAMDS32();
    bool checkAMDS64();

private:
    bool checkReg(const QString& strKey, const QString& strName, wchar_t* InstallDir, bool check32_Bit = false);
signals:

};

#endif // ITUNESSERVICE_H
