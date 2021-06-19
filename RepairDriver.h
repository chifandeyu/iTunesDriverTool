#pragma once

#include <QWidget>
#include "ui_RepairDriver.h"
#include "commonWidget.h"

struct zipPackageInfo
{
    int nOSBITS;
    QString strLocalPath;
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
class iTunesDriverDlg;
class httpDownload;
class iTunesServiceCheck;
class iTunesDriverInstall;

class RepairDriver : public commonWidget
{
    Q_OBJECT
    typedef enum StackedWidget
    {
        EMMNonePage = 0,
        EMMDownDrvPage,		    //驱动下载中
        EMMInstallingPage,		//驱动安装中
        EMMInstallSuccPage,		//驱动安装成功
        EMMInstallFailedPage,	//驱动安装失败
        EUnInstallPage,	//驱动安装失败
    }EStackedDrvWidgetPage;
public:
    RepairDriver(QWidget *parent = Q_NULLPTR);
    ~RepairDriver();
    bool checkAllDriver();
    void startDownloadDriver(const QString& fileUrl);
public slots:
    void slotCancel();
    void slotProgress(qint64 bytesReceived, qint64 bytesTotal, const QString& strSpeed);
    void slotDownloadErorr(const QString& errStr);
    void slotStartDownload();
    void slotDownloadFinished();
    void slotInstallFinish();
    void slotInstalling(QString packageName);
    void slotStartInstall();
    void slotUninstallDriver(bool bFinish);
    void slotDoRepair();
protected:
    bool nativeEvent(const QByteArray& eventType, void* message, long* result);

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
    //QMap<int, iTunesDriverEntity> m_entityMap;
    zipPackageInfo m_zipPackage;
    bool m_bDownloadFailed = false;
    bool m_bProc = false;
};
