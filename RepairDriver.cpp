#include "RepairDriver.h"
#include <QGraphicsDropShadowEffect>
#include "itunesservice.h"
#include "toolwnd.h"
#include "httpDownload.h"
#include <QFile>
#include <QJsonParseError>
#include <QJsonObject>

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
    connect(ui.pushButton, &QPushButton::clicked, this, &RepairDriver::doRepair);
    connect(ui.closeBtn, &QPushButton::clicked, [this]() {
        close();
    });
    m_pDwonloadDlg = new ToolWnd(this);
    getConfig();
}

RepairDriver::~RepairDriver()
{
    if (m_pDownloader) {
        m_pDownloader->deleteLater();
        m_pDownloader = nullptr;
    }
}

bool RepairDriver::checkAllDriver()
{
    bool is64 = iTunesServiceCheck::IsWow64();
    ui.widget_appleAppSupport64->setVisible(is64);
    ui.widget_appleMDSupoort64->setVisible(is64);
    ui.widget_appleMDSupoort32->setVisible(!is64);
    bool appSupport32 = m_iTunesChecker->checkApplicationSupport32();
    ui.label_appsupport32_install->setVisible(appSupport32);
    ui.label_appsupport32_no->setVisible(!appSupport32);
    if (is64) {
        bool appSupport64 = m_iTunesChecker->checkApplicationSupport64();
        ui.label_appsupport64_install->setVisible(appSupport64);
        ui.label_appsupport64_no->setVisible(!appSupport64);
        bool MDSupport64 = m_iTunesChecker->checkMobileDeviceSupport64();
        ui.label_amds64_install->setVisible(MDSupport64);
        ui.label_amds64_no->setVisible(!MDSupport64);
    }
    else {
        bool MDSupport32 = m_iTunesChecker->checkMobileDeviceSupport32();
        ui.label_amds_install->setVisible(MDSupport32);
        ui.label_amds_no->setVisible(!MDSupport32);
    }
    bool isRunning = m_iTunesChecker->servicIsRunning();
    ui.label_have_start->setVisible(isRunning);
    ui.label_no_start->setVisible(!isRunning);
    return true;
}

void RepairDriver::startDownloadDriver(const QString& fileUrl)
{
    m_fileUrl = fileUrl;
    if (m_pDownloader) {
        m_pDownloader->cancel();
        m_pDownloader->deleteLater();
        m_pDownloader = nullptr;
    }
    m_pDownloader = new httpDownload();
    connSigSlot();
    m_pDownloader->download(fileUrl);
}

void RepairDriver::slotCancel()
{
    if (m_pDownloader && m_pDownloader->isDownload()) {
        m_pDownloader->cancel();
    }
}

void RepairDriver::slotProgress(qint64 bytesReceived, qint64 bytesTotal, const QString& strSpeed)
{
    m_pDwonloadDlg->setProgressValue(bytesReceived, bytesTotal, strSpeed);
}

void RepairDriver::slotErorr(const QString& errStr)
{

}

void RepairDriver::slotStartDownload()
{
    m_pDwonloadDlg->InitDownloadIosDrvFrame();
}

void RepairDriver::slotFinished()
{

}

void RepairDriver::doRepair()
{

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
    QVariantList list = obj["files"].toVariant().toList();
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
        bool is64 = iTunesServiceCheck::IsWow64();
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
    for (int i = 0; i < list.count(); i++)
    {
        QVariantMap map = list[i].toMap();
        iTunesDriverEntity entity;
        entity.Index = map["Index"].toInt();
        qInfo() << "Index = " << entity.Index;
        entity.strHashCode = map["md5"].toString();
        qInfo() << "md5 = " << entity.strHashCode;
        entity.nOSBITS = map["OSBITS"].toInt();
        qInfo() << "OSBITS = " << entity.nOSBITS;
        entity.strIosDriverName = map["fileName"].toString();
        qInfo() << "fileName = " << entity.strIosDriverName;
        entity.strIosUrl = map["url"].toString();
        qInfo() << "url = " << entity.strIosUrl;
        entity.nFileSize = map["size"].toLongLong();
        qInfo() << "size = " << entity.nFileSize;
        m_entityMap.insert(entity.Index, entity);
    }
}

void RepairDriver::connSigSlot()
{
    connect(m_pDownloader, &httpDownload::sigErorr, this, &RepairDriver::slotErorr);
    connect(m_pDownloader, &httpDownload::sigStartDownload, this, &RepairDriver::slotStartDownload);
    connect(m_pDownloader, &httpDownload::sigProgress, this, &RepairDriver::slotProgress);
    connect(m_pDownloader, &httpDownload::sigFinished, this, &RepairDriver::slotFinished);
}
