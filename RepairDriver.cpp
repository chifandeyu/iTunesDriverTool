#include "RepairDriver.h"
#include <QGraphicsDropShadowEffect>
#include "itunesservice.h"
#include "httpDownload.h"
#include <QFile>
#include <QJsonParseError>
#include <QJsonObject>
#include "iTunesDriverInstall.h"
#include "iTunesDriverDlg.h"

RepairDriver::RepairDriver(QWidget *parent)
    : commonWidget(parent)
{
    m_iTunesChecker = new iTunesServiceCheck(this);
    ui.setupUi(this);
    QGraphicsDropShadowEffect* pGraphics = new QGraphicsDropShadowEffect(this);
    pGraphics->setOffset(0, 0);
    pGraphics->setColor(Qt::gray);
    pGraphics->setBlurRadius(10);
    ui.widget->setGraphicsEffect(pGraphics);

    ui.widget_appleUSBConnect->setVisible(false);
    connect(ui.pushButton, &QPushButton::clicked, this, &RepairDriver::slotDoRepair);
    connect(ui.closeBtn, &QPushButton::clicked, [this]() {
        close();
    });
    connect(ui.m_pUpdateBtn, &QPushButton::clicked, [this]() {
        checkAllDriver();
    });
    //m_pDwonloadDlg = new ToolWnd(this);
    //m_pDwonloadDlg->show();
    m_pInstaller = new iTunesDriverInstall();
    m_pInstaller->start();
    getConfig();
    m_iTunesDriverDlg = new iTunesDriverDlg(this);
    //m_iTunesDriverDlg->show();

    m_pDownloader = new httpDownload();
    connSigSlot();
}

RepairDriver::~RepairDriver()
{
    if (m_pDownloader) {
        m_pDownloader->deleteLater();
        m_pDownloader = nullptr;
    }
    if (m_pInstaller && m_pInstaller->isRunning()) {
        m_pInstaller->quit();
        m_pInstaller->wait();
    }
}

bool RepairDriver::checkAllDriver()
{
    bool bRet = true;
    bool is64 = iTunesServiceCheck::IsWow64();
    ui.widget_appleAppSupport64->setVisible(is64);
    ui.widget_appleMDSupoort64->setVisible(is64);
    ui.widget_appleMDSupoort32->setVisible(!is64);
    bool appSupport32 = m_iTunesChecker->checkAAS32();
    if (!appSupport32) {
        bRet = false;
    }
    ui.label_appsupport32_install->setVisible(appSupport32);
    ui.label_appsupport32_no->setVisible(!appSupport32);
    if (is64) {
        bool appSupport64 = m_iTunesChecker->checkAAS64();
        if (!appSupport64) {
            bRet = false;
        }
        ui.label_appsupport64_install->setVisible(appSupport64);
        ui.label_appsupport64_no->setVisible(!appSupport64);
        bool MDSupport64 = m_iTunesChecker->checkAMDS64();
        if (!MDSupport64) {
            bRet = false;
        }
        ui.label_amds64_install->setVisible(MDSupport64);
        ui.label_amds64_no->setVisible(!MDSupport64);
    }
    else {
        bool MDSupport32 = m_iTunesChecker->checkAMDS32();
        if (!MDSupport32) {
            bRet = false;
        }
        ui.label_amds_install->setVisible(MDSupport32);
        ui.label_amds_no->setVisible(!MDSupport32);
    }
    bool isRunning = m_iTunesChecker->servicIsRunning();
    if (!isRunning) {
        bRet = false;
    }
    ui.label_have_start->setVisible(isRunning);
    ui.label_no_start->setVisible(!isRunning);

    QString str = bRet ? QStringLiteral("重装驱动") : QStringLiteral("一键修复");
    ui.pushButton->setText(str);
    return bRet;
}

void RepairDriver::startDownloadDriver(const QString& fileUrl)
{
    if (m_pDownloader->isDownload())
        return;
    
    m_fileUrl = fileUrl;
    m_pDownloader->download(fileUrl, ".\\cache");
}

void RepairDriver::slotCancel()
{
    if (m_pDownloader && m_pDownloader->isDownload()) {
        m_pDownloader->cancel();
    }
}

void RepairDriver::slotProgress(qint64 bytesReceived, qint64 bytesTotal, const QString& strSpeed)
{
    //m_pDwonloadDlg->setProgressValue(bytesReceived, bytesTotal, strSpeed);
    if (bytesReceived == 0 && bytesReceived == 0)
        return;
    m_iTunesDriverDlg->setProgressValue(bytesReceived, bytesTotal, strSpeed);
}

void RepairDriver::slotDownloadErorr(const QString& errStr)
{
    m_bDownloadFailed = true;
    m_iTunesDriverDlg->onInstallDrvFailed(QStringLiteral("iTunes驱动下载失败！%1").arg(errStr));
}

void RepairDriver::slotStartDownload()
{
    m_iTunesDriverDlg->onStartDownload();
}

void RepairDriver::slotDownloadFinished()
{
    if (m_bDownloadFailed)
        return;
    doRepair();
}

void RepairDriver::slotInstallFinish()
{
    //check all
    bool isOk = checkAllDriver();
    if (isOk) {
        m_iTunesDriverDlg->onInstallDrvSucc();
    }
    else {
        m_iTunesDriverDlg->onInstallDrvFailed(QStringLiteral("iTunes 驱动安装失败！"));
    }
}

void RepairDriver::slotInstalling(QString packageName)
{
    //m_pDwonloadDlg->setInsatllFileName(packageName);
    m_iTunesDriverDlg->setInsatllFileName(packageName);
}

void RepairDriver::slotStartInstall()
{
    m_iTunesDriverDlg->onStartInstallDrv();
}

void RepairDriver::slotUninstallDriver(bool bFinish)
{
    if (bFinish) {
        //m_iTunesDriverDlg->InitInstallIosDrvSuccFrame();
    }
    else {
        m_iTunesDriverDlg->onStartUninstallDrv();
    }
}

void RepairDriver::slotDoRepair()
{
    m_bDownloadFailed = false;
    doRepair();
    m_iTunesDriverDlg->exec();
}

void RepairDriver::doRepair()
{
    if (m_pInstaller->isInstalling()) {
        return;
    }
    bool has = false;
    //check zip package and install
    if (QFile::exists(m_zipPackage.strLocalPath)) {
        QString md5 = iTunesServiceCheck::fileMd5(m_zipPackage.strLocalPath);
        if (md5 == m_zipPackage.strHashCode) {
            m_pInstaller->onInstallDriver(m_zipPackage.strLocalPath);
            has = true;
        }
    }
    if (!has) {
        startDownloadDriver(m_zipPackage.strUrl);
    }
}

void RepairDriver::getConfig()
{
    QString strPath = "./cache/itunesdriver.json";
    QFile file(strPath);
    if (!file.exists())
    {
        std::string strPrintf = strPath.toLocal8Bit();
        qDebug() << strPrintf.c_str() << " not exist!";
        return;
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        std::string strPrintf = strPath.toLocal8Bit();
        qDebug() << "open file failed : " << strPrintf.c_str();
        return;
    }
    QString conf = file.readAll();
    QByteArray arr = conf.toUtf8();
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(arr, &error);
    if (error.error != QJsonParseError::NoError)
    {
        qInfo() << "parse json error" << error.errorString();
        return;
    }

    if (document.isNull() || document.isEmpty())
    {
        qInfo() << "parse json null or empty";
        return;
    }
    QJsonObject obj = document.object();
    //QVariantList list = obj["files"].toVariant().toList();
    QVariantList zipPackList = obj["packages"].toVariant().toList();
    for (QVariant qv : zipPackList) {
        QVariantMap map = qv.toMap();
        zipPackageInfo info;
        info.strHashCode = map["md5"].toString();
        qInfo() << "Package md5 = " << info.strHashCode;
        info.nOSBITS = map["OSBITS"].toInt();
        qInfo() << "Package OSBITS = " << info.nOSBITS;
        info.strUrl = map["url"].toString();
        qInfo() << "Package url = " << info.strUrl;
        info.nFileSize = map["size"].toLongLong();
        qInfo() << "Package size = " << info.nFileSize;
        info.strLocalPath = map["path"].toString();
        qInfo() << "Package path = " << info.strLocalPath;
        bool is64 = iTunesServiceCheck::IsWow64();
        QFileInfo fileInfo(info.strUrl);
        if (is64 && info.nOSBITS == 64) {
            m_zipPackage = info;
            break;
        }
        else if (!is64 && info.nOSBITS == 32) {
            m_zipPackage = info;
            break;
        }
        m_zipPackage = info;
    }
    //for (int i = 0; i < list.count(); i++)
    //{
    //    QVariantMap map = list[i].toMap();
    //    iTunesDriverEntity entity;
    //    entity.Index = map["Index"].toInt();
    //    qInfo() << "Index = " << entity.Index;
    //    entity.strHashCode = map["md5"].toString();
    //    qInfo() << "md5 = " << entity.strHashCode;
    //    entity.nOSBITS = map["OSBITS"].toInt();
    //    qInfo() << "OSBITS = " << entity.nOSBITS;
    //    entity.strIosDriverName = map["fileName"].toString();
    //    qInfo() << "fileName = " << entity.strIosDriverName;
    //    entity.strIosUrl = map["url"].toString();
    //    qInfo() << "url = " << entity.strIosUrl;
    //    entity.nFileSize = map["size"].toLongLong();
    //    qInfo() << "size = " << entity.nFileSize;
    //    m_entityMap.insert(entity.Index, entity);
    //}
}

void RepairDriver::connSigSlot()
{
    connect(m_pDownloader, &httpDownload::sigErorr, this, &RepairDriver::slotDownloadErorr);
    connect(m_pDownloader, &httpDownload::sigStartDownload, this, &RepairDriver::slotStartDownload);
    connect(m_pDownloader, &httpDownload::sigProgress, this, &RepairDriver::slotProgress);
    connect(m_pDownloader, &httpDownload::sigFinished, this, &RepairDriver::slotDownloadFinished);

    connect(m_pInstaller, &iTunesDriverInstall::sigStartInstall, this, &RepairDriver::slotStartInstall);
    connect(m_pInstaller, &iTunesDriverInstall::sigInstalling, this, &RepairDriver::slotInstalling);
    connect(m_pInstaller, &iTunesDriverInstall::sigInstallFinish, this, &RepairDriver::slotInstallFinish);
    connect(m_pInstaller, &iTunesDriverInstall::sigUninstallDriver, this, &RepairDriver::slotUninstallDriver);

    connect(m_iTunesDriverDlg, &iTunesDriverDlg::sigCancel, this, &RepairDriver::slotCancel);
}
