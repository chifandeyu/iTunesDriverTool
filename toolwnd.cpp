#include "toolwnd.h"
#include "ui_toolwnd.h"
#include <QGraphicsDropShadowEffect>
#include <QMovie>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QJsonParseError>

//获取Json文件信息
QString ReadJsonFile(const QString& strPath)
{
    qDebug() << " entry";
    QFile file(strPath);

    if (!file.exists(strPath))
    {
        std::string strPrintf = strPath.toLocal8Bit();
        qDebug() << strPrintf.c_str() << " not exist!";
        return QString("");
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        std::string strPrintf = strPath.toLocal8Bit();
        qDebug() << "open file failed : " << strPrintf.c_str();
        return QString("");
    }

    QTextStream in(&file);
    QString strRet;
    in >> strRet;

    file.close();

    qDebug() << " leave";
    return strRet;
}

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

void ToolWnd::setProgressValue(qint64 bytesReceived, qint64 bytesTotal, const QString& strSpeed)
{
    ui->m_pProgDriverDown->setMaximum(bytesTotal);
    ui->m_pProgDriverDown->setValue(bytesReceived);
    ui->MMDownDrvSpeed->setText(strSpeed);
}

void ToolWnd::onErorr(const QString & errStr)
{
    QMessageBox::warning(this, QStringLiteral("Error"), QStringLiteral("file download failed: %1").arg(errStr), QMessageBox::Yes);
    close();
}

void ToolWnd::onStartDownload()
{
    InitDownloadIosDrvFrame();
}

void ToolWnd::onDownloadFinished()
{
    InitInstallIosDrvFrame();
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
    emit sigCancel();
    close();
}

void ToolWnd::slotCancel()
{

}
