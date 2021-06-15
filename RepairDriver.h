#pragma once

#include <QWidget>
#include "ui_RepairDriver.h"
#include "commonWidget.h"

typedef struct zipPackageInfo
{
    int nOSBITS;
    QString strLocalPath;
    QString strUrl;
    QString strHashCode;
    long long nFileSize;         //�ļ���С
};

//Ios�����ļ�ʵ��
typedef struct _iTunesDriverEntity
{
    int Index;	            //�������
    QString strHashCode;	//�����ļ���MD5
    int nOSBITS;	        //ϵͳλ��
    QString strIosDriverName; //��������
    QString strIosUrl;		//�������ص�ַ
    long long nFileSize;         //�ļ���С
}iTunesDriverEntity, *PiTunesDriverEntity;

class ToolWnd;
class iTunesDriverDlg;
class httpDownload;
class iTunesServiceCheck;
class iTunesDriverInstall;

class RepairDriver : public commonWidget
{
    Q_OBJECT

public:
    RepairDriver(QWidget *parent = Q_NULLPTR);
    ~RepairDriver();
    bool checkAllDriver();
    void startDownloadDriver(const QString& fileUrl);
public slots:
    void slotCancel();
    void slotProgress(qint64 bytesReceived, qint64 bytesTotal, const QString& strSpeed);
    void slotErorr(const QString& errStr);
    void slotStartDownload();
    void slotDownloadFinished();
    void slotInstallFinish();
    void slotInstalling(QString packageName);
    void slotStartInstall();
    void slotUninstallDriver(bool bFinish);
    void slotDoRepair();

private:
    void doRepair();
    void getConfig();
    void connSigSlot();
private:
    Ui::RepairDriver ui;
    QString m_fileUrl;
    QString m_configStr;
    //ToolWnd *m_pDwonloadDlg = nullptr;
    iTunesDriverDlg* m_iTunesDriverDlg = nullptr;
    httpDownload* m_pDownloader = nullptr;
    iTunesDriverInstall* m_pInstaller = nullptr;
    iTunesServiceCheck *m_iTunesChecker = nullptr;
    QMap<int, iTunesDriverEntity> m_entityMap;
    zipPackageInfo m_zipPackage;
};
