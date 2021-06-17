#pragma once

#include <QThread>
#include <QFileInfo>
#include <QThread>

const QString strCachePath = ".\\cache";

class iTunesDriverInstall : public QThread
{
    Q_OBJECT

public:
    iTunesDriverInstall(QObject *parent = nullptr);
    ~iTunesDriverInstall();
    static void GetIosDriverPaths(QString& strAASPath, QString& strAAS64Path,
        QString& strAMDSPath, QString& strAMDS64Path, 
        QString& BonjourPath, QString& Bonjour64Path);
    bool unZipPackage(const QString& filePath);
    void unInstallDriver();
    void onInstallDriver(QString zipPackage);
    bool isInstalling() {
        return m_isInstalling;
    }
signals:
    void sigInstall(QString zipPackage);
    void sigStartInstall();
    void sigInstalling(QString packageName);
    void sigInstallFinish();
    void sigUninstallDriver(bool bFinish);

public slots:
    void slotInstall(QString zipPackage);

protected:
    void run();
private:
    /**
    * @brief 安装已经下载好的驱动
    * @[in] ios_driver_path
    */
    void installIosDriver(
        const QString& strAASPath,
        const QString& strAAS64Path,
        const QString& strAMDSPath,
        const QString& strAMDS64Path,
        const QString& BonjourPath, 
        const QString& Bonjour64Path);
private:
    bool m_isInstalling = false;
};
