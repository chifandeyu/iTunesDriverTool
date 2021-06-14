#pragma once

#include <QObject>
#include <string>
#include <QVector>

typedef struct softInfo
{
    QString m_strSoftName; //软件名
    QString m_strSoftVersion; //软件版本号
    QString m_strInstallLocation;//软件安装目录
    QString m_strPublisher; //软件发布商
    QString m_strMainProPath; //主程序所在路径
    QString m_strUninstallPath; //卸载软件所在路径
    QString m_strInstallDate; //安装事件
}SOFTINFO;

enum AppleAppType
{
    APPLE_APP_SUPPORT32,
    APPLE_APP_SUPPORT64,
    APPLE_DEVICE_SUPPORT32,
    APPLE_DEVICE_SUPPORT64,
};
class SoftMgr : public QObject
{
    Q_OBJECT

public:
    SoftMgr(QObject *parent = nullptr);
    ~SoftMgr();
    bool GetSoftInfo();
    bool uninstallApp(const wchar_t *wszName);
signals:
    void uninstallAppErr(AppleAppType);

private:
    QVector<SOFTINFO> m_vecSoftInfo;
    QString m_strError;
};
