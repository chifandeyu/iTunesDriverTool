#include "iTunesDriverDlg.h"
#include "ui_iTunesDriverDlg.h"
#include <QGraphicsDropShadowEffect>
#include <QMovie>
#include <QDebug>
#include <QMessageBox>
#include <QFile>

iTunesDriverDlg::iTunesDriverDlg(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::WindowType::WindowMinMaxButtonsHint | Qt::WindowType::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    QGraphicsDropShadowEffect* pGraphics = new QGraphicsDropShadowEffect(this);
    pGraphics->setOffset(0, 0);
    pGraphics->setColor(Qt::gray);
    pGraphics->setBlurRadius(10);
    ui.widget->setGraphicsEffect(pGraphics);

    m_pIOSMovie = new QMovie(QString::fromUtf8(":/res/installIOSDrv_ing.gif"), "", this);
    connSigSlot();
}

iTunesDriverDlg::~iTunesDriverDlg()
{
}

void iTunesDriverDlg::InitDownloadDrvFrame()
{
    ui.titleLab->setText(QStringLiteral("下载驱动"));
    ui.m_stackedWidgetDrv->setCurrentIndex(EMMDownDrvPage);
    changeIOSGif();
}

void iTunesDriverDlg::InitInstallDrvFrame()
{
    ui.titleLab->setText(QStringLiteral("安装驱动"));
    ui.m_stackedWidgetDrv->setCurrentIndex(EMMInstallingPage);
    changeIOSGif();
}

void iTunesDriverDlg::onStartUninstallDrv()
{
    InitUninstallDrvFrame();
}

void iTunesDriverDlg::onInstallDrvFailed(QString err)
{
    InitInstallDrvFailedFrame(err);
}

void iTunesDriverDlg::onInstallDrvSucc()
{
    InitInstallDrvSuccFrame();
}

void iTunesDriverDlg::InitInstallDrvSuccFrame()
{
    ui.titleLab->setText(QStringLiteral("驱动安装成功"));
    ui.m_stackedWidgetDrv->setCurrentIndex(EMMInstallSuccPage);
}

void iTunesDriverDlg::InitInstallDrvFailedFrame(QString str)
{
    ui.titleLab->setText(QStringLiteral("安装失败"));
    ui.m_stackedWidgetDrv->setCurrentIndex(EMMInstallFailedPage);
    ui.MMInstallDrvMsg->setText(str);
}

void iTunesDriverDlg::InitUninstallDrvFrame()
{
    ui.titleLab->setText(QStringLiteral("卸载旧驱动"));
    ui.m_stackedWidgetDrv->setCurrentIndex(EUnInstallPage);
}

void iTunesDriverDlg::setInsatllFileName(QString name)
{
    ui.MMInstallingDrvFile->setText(name);
}

void iTunesDriverDlg::changeIOSGif()
{
    bool result = m_pIOSMovie->isValid();
    if (result) {
        qDebug() << "IOS Movie is valid";
        m_pIOSMovie->setScaledSize(ui.MMDownDrvIcon->size());
        ui.MMDownDrvIcon->setMovie(m_pIOSMovie);
        ui.MMInstallingDrvIcon->setMovie(m_pIOSMovie);
        m_pIOSMovie->start();
    }
    else
        qDebug() << "IOS Movie is invalid";
}

void iTunesDriverDlg::setProgressValue(qint64 bytesReceived, qint64 bytesTotal, const QString& strSpeed)
{
    ui.m_pProgDriverDown->setMaximum(bytesTotal);
    ui.m_pProgDriverDown->setValue(bytesReceived);
    ui.MMDownDrvSpeed->setText(strSpeed);
}

void iTunesDriverDlg::onErorr(const QString& errStr)
{
    QMessageBox::warning(this, QStringLiteral("Error"), QStringLiteral("file download failed: %1").arg(errStr), QMessageBox::Yes);
    close();
}

void iTunesDriverDlg::onStartDownload()
{
    InitDownloadDrvFrame();
}

void iTunesDriverDlg::onStartInstallDrv()
{
    InitInstallDrvFrame();
}

void iTunesDriverDlg::on_closeBtn_clicked()
{
    emit sigCancel();
    close();
}

void iTunesDriverDlg::connSigSlot()
{
    connect(ui.m_CancelBtn, &QPushButton::clicked, this, &iTunesDriverDlg::on_closeBtn_clicked);
}
