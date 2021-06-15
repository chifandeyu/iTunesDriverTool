#include "httpDownload.h"
#include <QFileInfo>
#include <QDir>

#define UNIT_KB (1024)           //KB
#define UNIT_MB (1024*1024)      //MB
#define UNIT_GB (1024*1024*1024) //GB

httpDownload::httpDownload(QObject *parent)
    : QObject(parent)
{
    m_pManager = new QNetworkAccessManager(this);
}

httpDownload::~httpDownload()
{
    cancel();
}

bool httpDownload::download(const QString& strUrl, const QString& dir)
{
    m_url = QUrl(strUrl);
    //将URL路径存入QFileInfo类
    QFileInfo info(m_url.path());

    //从info中得到文件名
    QString fileName(info.fileName());

    //如果没有获取到info
    if (fileName.isEmpty()) {
        //textEdit->setText(tr("<h2> 下载失败 请检查下载地址!!! </h2>"));
        emit sigErorr("download failed, please check adress!!!");
        qDebug() << "==== download failed, please check adress!!!" << strUrl;
        return false;
    }
    else {
        QDir qdir(dir);
        if (!qdir.exists()) {
            qdir.mkdir(dir);
        }
        fileName = dir + "\\" + fileName;
        //建立文件
        m_pFile = new QFile(fileName);
        
        //如果文件没有打开的情况
        if (!m_pFile->open((QIODevice::WriteOnly))) {
            delete m_pFile;
            m_pFile = nullptr;
            return false;
        }

        //执行网络请求函数
        startRequest(m_url);

        //开始下载
        emit sigStartDownload();
        qDebug() << "==== start download" << strUrl;
    }
    return true;
    return true;
}

void httpDownload::cancel()
{
    if (m_pReply) {
        m_pReply->deleteLater();
        m_pReply = nullptr;
    }
    if (m_pFile) {
        m_pFile->close();
        m_pFile->remove();
        m_pFile = nullptr;
    }
    m_isDownloading = false;
    emit sigCancel();
    qDebug() << "==== httpcancel";
}

void httpDownload::pause()
{

}

QString httpDownload::transformUnit(qint64 bytes, bool isSpeed)
{
    QString strUnit = "B";
    if (bytes <= 0)
        bytes = 0;
    else if (bytes < UNIT_KB)
    {
    }
    else if (bytes < UNIT_MB)
    {
        bytes /= UNIT_KB;
        strUnit = "KB";
    }
    else if (bytes < UNIT_GB)
    {
        bytes /= UNIT_MB;
        strUnit = "MB";
    }
    else if (bytes > UNIT_GB)
    {
        bytes /= UNIT_GB;
        strUnit = "GB";
    }

    if (isSpeed)
    {
        strUnit += "/S";
    }

    return QString("%1%2").arg(bytes).arg(strUnit);
}

void httpDownload::startRequest(QUrl url)
{
    m_isDownloading = true;
    //get发送一个网络请求 返回网络应答
    m_pReply = m_pManager->get(QNetworkRequest(url));

    //每当有新的数据要读取时 发射信号
    connect(m_pReply, &QNetworkReply::readyRead, this, &httpDownload::httpReadyRead);

    //每当有下载进度更新时 发射信号 更新进度条和文本窗
    connect(m_pReply, &QNetworkReply::downloadProgress, this, &httpDownload::updateProgress);

    //应答处理结束时会发射finished信号
    connect(m_pReply, &QNetworkReply::finished, this, &httpDownload::httpFinished);

    connect(m_pReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(httpError(QNetworkReply::NetworkError)));
    //时间计时开始
    time.start();
}

void httpDownload::httpReadyRead()
{
    if (m_pFile) {
        m_pFile->write(m_pReply->readAll());
    }
}

void httpDownload::httpFinished()
{
    qDebug() << "==== httpFinished";
    if (m_pFile) {
        m_pFile->close();
        delete m_pFile;
        m_pFile = nullptr;
    }

    m_pReply->deleteLater();
    m_pReply = nullptr;
    emit sigFinished();
    m_isDownloading = false;
}

void httpDownload::updateProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    // calculate the download speed
    double speed = bytesReceived * 1000.0 / time.elapsed();
    QString unit;
    if (speed < 1024) {
        unit = "bytes/sec";
    }
    else if (speed < 1024 * 1024) {
        speed /= 1024;
        unit = "kB/s";
    }
    else {
        speed /= 1024 * 1024;
        unit = "MB/s";
    }

    QString strSpeed = QString::fromLatin1("%1 %2").arg(speed, 3, 'f', 1).arg(unit);

    emit sigProgress(bytesReceived, bytesTotal, strSpeed);
}

void httpDownload::httpError(QNetworkReply::NetworkError)
{
    m_isDownloading = false;
    QString errStr = m_pReply->errorString();
    emit sigErorr(errStr);
    qDebug() << "==== httpError" << errStr;
}
