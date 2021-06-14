#pragma once

#include <QObject>
#include <string>
#include <QVector>

typedef struct softInfo
{
    QString m_strSoftName; //�����
    QString m_strSoftVersion; //����汾��
    QString m_strInstallLocation;//�����װĿ¼
    QString m_strPublisher; //���������
    QString m_strMainProPath; //����������·��
    QString m_strUninstallPath; //ж���������·��
    QString m_strInstallDate; //��װ�¼�
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
