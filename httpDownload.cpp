#include "httpDownload.h"
#include <QFileInfo>
#include <QDir>

#define UNIT_KB (1024)           //KB
#define UNIT_MB (1024*1024)      //MB

httpDownload::httpDownload(QObject *parent)
    : QThread(parent)
{
}

httpDownload::~httpDownload()
{
    cancel();
}

void httpDownload::run()
{
    onDownload();
}

bool httpDownload::download(const QString& strUrl, const QString& dir)
{
    m_url = QUrl(strUrl);
    //��URL·������QFileInfo��
    QFileInfo info(m_url.path());

    //��info�еõ��ļ���
    QString fileName(info.fileName());

    //���û�л�ȡ��info
    if (fileName.isEmpty()) {
        //textEdit->setText(tr("<h2> ����ʧ�� �������ص�ַ!!! </h2>"));
        emit sigErorr("download failed, please check adress!!!");
        qDebug() << "==== download failed, please check adress!!!" << strUrl;
        return false;
    }

    if (m_bRunning == true) return true;
    m_cancel = false;
    clear();

    QDir qdir(dir);
    if (!qdir.exists()) {
        qdir.mkdir(dir);
    }
    fileName = dir + "\\" + fileName;

    m_strDownloadUrl = strUrl.toStdString();
    m_strLocalFilePath = fileName.toLocal8Bit().toStdString();

    // ��ʼ��libcurl
    m_pCurl = curl_easy_init();
    if (m_pCurl == NULL)
    {
        return false;
    }

    // ����libcurl��ѡ��
    setOption();

    // �����ļ�
    m_pFile = fopen(m_strLocalFilePath.c_str(), "wb");
    if (m_pFile == NULL)
    {
        return false;
    }

    start();
    time.start();
    return true;
}

bool httpDownload::cancel()
{
    m_cancel = true;
    m_bRunning = false;
    quit();
    wait();
    return true;
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

size_t httpDownload::handleWrite(void* buffer, size_t size, size_t nmemb, void* userp)
{
    httpDownload* pDownloader = (httpDownload*)userp;
    if (pDownloader)
    {
        return pDownloader->onWrite(buffer, size, nmemb);
    }
    return 0;
}

size_t httpDownload::handleProgress(void* buffer, double dltotal, double dlnow, double ultotal, double ulnow)
{
    httpDownload* pDownloader = (httpDownload*)buffer;
    if (pDownloader)
    {
        if (pDownloader->m_cancel)
        {
            return -1;
        }
        //pDownloader->onProgress(dltotal, dlnow);
        pDownloader->updateProgress(dlnow, dltotal);
    }
    return 0;
}

size_t httpDownload::onWrite(void* buffer, size_t size, size_t nmemb)
{
    size_t return_size = fwrite(buffer, size, nmemb, m_pFile);
    //qDebug() << (char*)buffer;
    return return_size;
}

size_t httpDownload::onProgress(const double& dltotal, const double& dlnow)
{
    qDebug()<<"dlnow:"<< dlnow<<"dltotal:"<<dltotal<<"dlnow:"<< (dlnow * 100.0 / dltotal);
    return 0;
}

void httpDownload::onDownload()
{
    m_bRunning = true;
    emit sigStartDownload();
    qDebug() << "==== start download";
    // ִ������
    CURLcode return_code = CURLE_OK;
    return_code = curl_easy_perform(m_pCurl);
    bool isSuccess = true;
    // �ر��ļ�
    if (m_pFile)
    {
        fclose(m_pFile);
        m_pFile = NULL;
    }
    if (CURLE_ABORTED_BY_CALLBACK == return_code) {
        emit sigCancel();
        qDebug() << "==== httpcancel";
        isSuccess = false;
    }
    QString errStr;
    // ����ʧ��
    if (return_code != CURLE_OK)
    {
        errStr = curl_easy_strerror(return_code);
        isSuccess = false;
    }

    // ��ȡ״̬��
    int response_code = 0;
    curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &response_code);
    if (response_code != 200)
    {
        qDebug() << "http code: " << response_code;
        if(errStr.isEmpty())
            errStr = QString("http error code: %1").arg(response_code);
        isSuccess = false;
    }

    {
        curl_easy_cleanup(m_pCurl);
        m_pCurl = NULL;
        curl_global_cleanup();
    }
    m_bRunning = false;
    if (isSuccess)
    {
        emit sigFinished();
        qDebug() << "==== httpFinished";
    }
    else
    {
        emit sigErorr(errStr);
    }
}

void httpDownload::setOption()
{
    // Զ��URL��֧�� http, https, ftp
    curl_easy_setopt(m_pCurl, CURLOPT_URL, m_strDownloadUrl.c_str());

    // ����User-Agent
    std::string useragent = ("Mozilla/5.0 (Windows NT 6.1; WOW64; rv:13.0) Gecko/20100101 Firefox/13.0.1");
    curl_easy_setopt(m_pCurl, CURLOPT_USERAGENT, useragent.c_str());

    // �����ض����������
    curl_easy_setopt(m_pCurl, CURLOPT_MAXREDIRS, 5);

    // ����301��302��ת����location
    curl_easy_setopt(m_pCurl, CURLOPT_FOLLOWLOCATION, 1);

    curl_easy_setopt(m_pCurl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(m_pCurl, CURLOPT_POST, false);

    // �������ݻص�����
    curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, handleWrite);
    curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, this);

    // ���Ȼص�����
    curl_easy_setopt(m_pCurl, CURLOPT_NOPROGRESS, 0);
    curl_easy_setopt(m_pCurl, CURLOPT_PROGRESSDATA, this);
    curl_easy_setopt(m_pCurl, CURLOPT_PROGRESSFUNCTION, handleProgress);

    // ����������SSL��֤����ʹ��CA֤��
    curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, 0L);

    // ��֤�������˷��͵�֤�飬Ĭ���� 2(��)��1���У���0�����ã�
    curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, 0L);
}

void httpDownload::clear()
{
    if (m_pFile)
    {
        fclose(m_pFile);
        m_pFile = NULL;
    }

    if (m_pCurl)
    {
        curl_easy_cleanup(m_pCurl);
        m_pCurl = NULL;
        curl_global_cleanup();
    }

    m_strDownloadUrl.clear();
    m_strLocalFilePath.clear();
}
