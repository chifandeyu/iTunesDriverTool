#pragma once

#include <QWidget>
#include "ui_RepairDriver.h"
#include "commonWidget.h"

typedef struct zipPackageInfo
{
    int nOSBITS;
    QString strUrl;
    QString strHashCode;
    long long nFileSize;         //文件大小
};

//Ios驱动文件实体
typedef struct _iTunesDriverEntity
{
    int Index;	            //驱动序号
    QString strHashCode;	//驱动文件的MD5
    int nOSBITS;	        //系统位数
    QString strIosDriverName; //驱动名称
    QString strIosUrl;		//驱动下载地址
    long long nFileSize;         //文件大小
}iTunesDriverEntity, *PiTunesDriverEntity;

class ToolWnd;
class httpDownload;
class iTunesServiceCheck;

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
    void slotFinished();

private slots:
    void doRepair();
private:
    void getConfig();
    void connSigSlot();
private:
    Ui::RepairDriver ui;
    QString m_fileUrl;
    QString m_configStr;
    ToolWnd *m_pDwonloadDlg = nullptr;
    httpDownload* m_pDownloader = nullptr;
    iTunesServiceCheck *m_iTunesChecker = nullptr;
    QMap<int, iTunesDriverEntity> m_entityMap;
    zipPackageInfo m_zipPackage;
};
