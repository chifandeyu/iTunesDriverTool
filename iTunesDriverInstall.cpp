#include "iTunesDriverInstall.h"
#include "itunesservice.h"
#include <QtGui\5.12.9\QtGui\private\qzipreader_p.h>
#include <QDir>
#include "SoftMgr.h"
#include <QProcess>
#include <QDebug>
#include <windows.h>
#include <shellapi.h>

iTunesDriverInstall::iTunesDriverInstall(QObject *parent)
    : QThread(parent)
{
    this->moveToThread(this);
    connect(this, &iTunesDriverInstall::sigInstall, this, &iTunesDriverInstall::slotInstall);
}

iTunesDriverInstall::~iTunesDriverInstall()
{
}

void iTunesDriverInstall::GetIosDriverPaths(
    QString& strAASPath, 
    QString& strAAS64Path, 
    QString& strAMDSPath,
    QString& strAMDS64Path)
{
    QString strCachePath = ".\\cache";
    strAASPath = QString(strCachePath + "\\AppleApplicationSupport.msi");
    QFileInfo aasInfo(strAASPath);
    if (aasInfo.exists()) {
        strAASPath = aasInfo.absoluteFilePath();
        strAASPath.replace("/", "\\");
    }
    strAAS64Path = QString(strCachePath + "\\AppleApplicationSupport64.msi");
    QFileInfo AAS64Info(strAAS64Path);
    if (AAS64Info.exists()) {
        strAAS64Path = AAS64Info.absoluteFilePath();
        strAAS64Path.replace("/", "\\");
    }
    strAMDSPath = QString(strCachePath + "\\AppleMobileDeviceSupport.msi");
    QFileInfo AMDSInfo(strAMDSPath);
    if (AMDSInfo.exists()) {
        strAMDSPath = AMDSInfo.absoluteFilePath();
        strAMDSPath.replace("/", "\\");
    }
    strAMDS64Path = QString(strCachePath + "\\AppleMobileDeviceSupport64.msi");
    QFileInfo AMDS64(strAMDS64Path);
    if (AMDS64.exists()) {
        strAMDS64Path = AMDS64.absoluteFilePath();
        strAMDS64Path.replace("/", "\\");
    }
}

bool iTunesDriverInstall::unZipPackage(const QString &filePath)
{
    bool bRet = true;
    QFileInfo info(filePath);
    QString dir = info.dir().absolutePath();
    QZipReader cZip(info.absoluteFilePath());
    foreach(QZipReader::FileInfo item, cZip.fileInfoList())
    {
        if (item.isDir)
        {
            QDir d(item.filePath);
            if (!d.exists())
                d.mkpath(dir + "\\" + item.filePath);
        }

        if (item.isFile)
        {
            QFile file(dir + "\\" + item.filePath);
            bRet = file.open(QIODevice::WriteOnly | QIODevice::Truncate);
            if (0 >= file.write(cZip.fileData(item.filePath))) {
                bRet = false;
            }
            file.close();
        }
    }

    cZip.close();
    return bRet;
}

void iTunesDriverInstall::unInstallDriver()
{
    SoftMgr sinfo;
    sinfo.GetSoftInfo();
    emit sigUninstallDriver(false);
    const wchar_t* wszASUName = L"Apple Software Update";
    sinfo.uninstallApp(wszASUName);
    const wchar_t* wszBonjour = L"Bonjour";
    sinfo.uninstallApp(wszBonjour);
    const wchar_t* wszAMDSName = L"Apple Mobile Device Support";
    sinfo.uninstallApp(wszAMDSName);
    const wchar_t *wszAASName = L"Apple 应用程序支持 (32 位)";
    sinfo.uninstallApp(wszAASName);
    const wchar_t* wszAAS64Name = L"Apple 应用程序支持 (64 位)";
    sinfo.uninstallApp(wszAAS64Name);
    emit sigUninstallDriver(true);
}

void iTunesDriverInstall::onInstallDriver(QString zipPackage)
{
    emit sigInstall(zipPackage);
}

void iTunesDriverInstall::slotInstall(QString zipPackage)
{
    m_isInstalling = true;

    //unzip package
    bool isOk = unZipPackage(zipPackage);
    if (isOk) {
        unInstallDriver();
    }

    bool is64 = iTunesServiceCheck::IsWow64();
    QString strAASPath;
    QString strAAS64Path;
    QString strAMDSPath;
    QString strAMDS64Path;
    GetIosDriverPaths(strAASPath, strAAS64Path, strAMDSPath, strAMDS64Path);
    installIosDriver(strAASPath, strAAS64Path, strAMDSPath, strAMDS64Path);
    m_isInstalling = false;
}

void iTunesDriverInstall::run()
{
    int ret = exec();
}

void iTunesDriverInstall::installIosDriver(
    const QString& strAASPath, 
    const QString& strAAS64Path,
    const QString& strAMDSPath,
    const QString& strAMDS64Path)
{
    //////////////////////////////////////////////////////////////////////////
    qDebug() << "install ios driver entry...";
    emit sigStartInstall();
    /////////////////////////////////AAS32/////////////////////////////////////////
    BOOL result = false;
    SHELLEXECUTEINFO ShExecInfo = { 0 };
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = NULL;
    ShExecInfo.lpFile = (LPCWSTR)reinterpret_cast<const wchar_t*>(strAASPath.utf16());
    std::wstring paramWStr = std::wstring(L"  /qn /norestart");
    ShExecInfo.lpParameters = paramWStr.c_str();
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = SW_SHOWNORMAL;
    ShExecInfo.hInstApp = NULL;
    emit sigInstalling("AppleApplicationSupport");
    result = ShellExecuteEx(&ShExecInfo);
    DWORD ret = WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
    if (result == TRUE && ret == 0)
    {
        //strcpy_s(pLocalInstallAASConnMsgWparam->err_msg, "AppleApplicationSupport.msi安装成功");
        qDebug() << "install AppleApplicationSupport.msi success";
    }
    else
    {
        //strcpy_s(pLocalInstallAASConnMsgWparam->err_msg, "AppleApplicationSupport.msi安装失败");
        qDebug() << QString("install AppleApplicationSupport.msi failed, result = %1 ret = %2")
            .arg(result).arg(ret);
    }

    ////////////////////////////////////AAS64//////////////////////////////////////
    if (iTunesServiceCheck::IsWow64())
    {
        ShExecInfo.lpFile = (LPCWSTR)reinterpret_cast<const wchar_t*>(strAAS64Path.utf16());
        emit sigInstalling("AppleApplicationSupport64");
        result = ShellExecuteEx(&ShExecInfo);
        ret = WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
        if (result == TRUE && ret == 0)
        {
            //strcpy_s(pLocalInstallAAS64ConnMsgWparam->err_msg, "AppleApplicationSupport64.msi安装成功");
            qDebug() << "install AppleApplicationSupport64.msi success";
        }
        else
        {
            //strcpy_s(pLocalInstallAAS64ConnMsgWparam->err_msg, "AppleApplicationSupport64.msi安装失败");
            qDebug() << QString("install AppleApplicationSupport64.msi failed, result = %1 ret = %2")
                .arg(result).arg(ret);
        }
        /////////////////AMDS64/////////////////
        ShExecInfo.lpFile = (LPCWSTR)reinterpret_cast<const wchar_t*>(strAMDS64Path.utf16());
        emit sigInstalling("AppleMobileDeviceSupport64");
        result = ShellExecuteEx(&ShExecInfo);
        ret = WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
        if (ret == 0)
        {
            //strcpy_s(pLocalInstallAMDSConnMsgWparam->err_msg, "AppleMobileDeviceSupport64.msi安装成功");
            qDebug() << "install AppleMobileDeviceSupport64.msi success";
        }
        else
        {
            //strcpy_s(pLocalInstallAMDSConnMsgWparam->err_msg, "AppleMobileDeviceSupport64.msi安装失败");
            qDebug() << QString("install AppleMobileDeviceSupport64.msi failed, result = %1 ret = %2")
                .arg(result).arg(ret);
        }
    }
    else
    {
        /////////////////AMDS/////////////////
        ShExecInfo.lpFile = (LPCWSTR)reinterpret_cast<const wchar_t*>(strAMDSPath.utf16());
        emit sigInstalling("AppleMobileDeviceSupport");
        result = ShellExecuteEx(&ShExecInfo);
        ret = WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
        if (ret == 0)
        {
            //strcpy_s(pLocalInstallAMDSConnMsgWparam->err_msg, "AppleMobileDeviceSupport.msi安装成功");
            qDebug() << "install AppleMobileDeviceSupport.msi success";
        }
        else
        {
            //strcpy_s(pLocalInstallAMDSConnMsgWparam->err_msg, "AppleMobileDeviceSupport.msi安装失败");
            qDebug() << QString("install AppleMobileDeviceSupport.msi failed, result = %1 ret = %2")
                .arg(result).arg(ret);
        }
    }
    emit sigInstallFinish();
    qDebug() << "install ios driver leave...";
}
