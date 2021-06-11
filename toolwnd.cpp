#include "toolwnd.h"
#include "ui_toolwnd.h"
#include <QGraphicsDropShadowEffect>
#include <QMovie>
#include <QDebug>
#include <QMessageBox>
#include "httpDownload.h"

ToolWnd::ToolWnd(QWidget *parent) :
    commonWidget(parent),
    ui(new Ui::ToolWnd)
{
    ui->setupUi(this);
    QGraphicsDropShadowEffect *pGraphics = new QGraphicsDropShadowEffect(this);
    pGraphics->setOffset(0, 0);
    pGraphics->setColor(Qt::gray);
    pGraphics->setBlurRadius(10);
    ui->widget->setGraphicsEffect(pGraphics);

    m_pIOSMovie = new QMovie(QString::fromUtf8(":/res/installIOSDrv_ing.gif"),"",this);
}

ToolWnd::~ToolWnd()
{
    if (m_pDownloader) {
        m_pDownloader->deleteLater();
        m_pDownloader = nullptr;
    }
    delete ui;
}

void ToolWnd::InitDownloadIosDrvFrame()
{
    ui->titleLab->setText(QStringLiteral("下载驱动"));
    ui->m_stackedWidgetDrv->setCurrentIndex(EMMDownDrvPage);
    changeIOSGif();
    showDialog();
}

void ToolWnd::InitInstallIosDrvFrame()
{
    ui->titleLab->setText(QStringLiteral("安装驱动"));
    ui->m_stackedWidgetDrv->setCurrentIndex(EMMInstallingPage);
}

void ToolWnd::InitInstallIosDrvSuccFrame()
{
    ui->titleLab->setText(QStringLiteral("驱动安装成功"));
    ui->m_stackedWidgetDrv->setCurrentIndex(EMMInstallSuccPage);
    showDialog();
}

void ToolWnd::InitInstallDrvFailedFrame(QString str)
{
    ui->titleLab->setText(QStringLiteral("安装失败"));
    ui->m_stackedWidgetDrv->setCurrentIndex(EMMInstallFailedPage);
    ui->MMInstallDrvMsg->setText(str);
}

void ToolWnd::setInsatllFileName(QString name)
{
    ui->MMInstallingDrvFile->setText(name);
}

void ToolWnd::changeIOSGif()
{
    bool result = m_pIOSMovie->isValid();
    if(result) {
        qDebug() <<"IOS Movie is valid";
        m_pIOSMovie->setScaledSize(ui->MMDownDrvIcon->size());
        ui->MMDownDrvIcon->setMovie(m_pIOSMovie);
        ui->MMInstallingDrvIcon->setMovie(m_pIOSMovie);
        m_pIOSMovie->start();
    }
    else
        qDebug() << "IOS Movie is invalid";
}

void ToolWnd::setProgressValue(int val, qint64 bytesReceived, qint64 bytesTotal, QString strSpeed)
{

}

void ToolWnd::startDownloadDriver(const QString& fileUrl)
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

void ToolWnd::showDialog()
{
    if(this->isHidden())
    {
//        setModal(true);
        show();
    }
}

void ToolWnd::on_closeBtn_clicked()
{
    if (m_pDownloader && m_pDownloader->isDownload()) {
        m_pDownloader->cancel();
    }
    close();
}

void ToolWnd::slotProgress(qint64 bytesReceived, qint64 bytesTotal, const QString& strSpeed)
{
    ui->m_pProgDriverDown->setMaximum(bytesTotal);
    ui->m_pProgDriverDown->setValue(bytesReceived);
    ui->MMDownDrvSpeed->setText(strSpeed);
}

void ToolWnd::slotErorr(const QString& errStr)
{
    QMessageBox::warning(this, QStringLiteral("Error"), QStringLiteral("file download failed: %1").arg(errStr), QMessageBox::Yes);
    close();
}

void ToolWnd::slotStartDownload()
{

}

void ToolWnd::slotFinished()
{
    InitInstallIosDrvFrame();
}

void ToolWnd::slotCancel()
{

}

void ToolWnd::connSigSlot()
{
    connect(m_pDownloader, &httpDownload::sigErorr, this, &ToolWnd::slotErorr);
    connect(m_pDownloader, &httpDownload::sigStartDownload, this, &ToolWnd::slotStartDownload);
    connect(m_pDownloader, &httpDownload::sigProgress, this, &ToolWnd::slotProgress);
    connect(m_pDownloader, &httpDownload::sigFinished, this, &ToolWnd::slotFinished);
}
