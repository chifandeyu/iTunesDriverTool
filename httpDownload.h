#pragma once

#include <QThread>
#include "curl.h"
#include <QTime>
#include <QUrl>
#include <QDebug>

class httpDownload : public QThread
{
    Q_OBJECT

public:
    httpDownload(QObject *parent = nullptr);
    ~httpDownload();
    virtual void run();
    virtual bool download(const QString& strUrl, const QString& dir);

    virtual bool cancel();

    bool isDownload() const {
        return m_bRunning;
    }
    bool m_cancel = false;
signals:
    void sigErorr(const QString& errStr);
    void sigStartDownload();
    void sigProgress(qint64 bytesReceived, qint64 bytesTotal, const QString& strSpeed);
    void sigFinished();
    void sigCancel();

protected:
    void updateProgress(qint64 bytesReceived, qint64 bytesTotal);
    /// д��ص�
    static size_t handleWrite(void* buffer, size_t size, size_t nmemb, void* userp);

    /// ���Ȼص�
    static size_t handleProgress(void* buffer, double dltotal, double dlnow, double ultotal, double ulnow);

protected:
    /// д��ص�
    size_t onWrite(void* buffer, size_t size, size_t nmemb);

    /// ���Ȼص�
    size_t onProgress(const double& dltotal, const double& dlnow);

    /// ���ػص�
    void onDownload();
protected:
    /// ����libcurlѡ��
    void setOption();

    /// �������
    void clear();
private:
    QTime time;
    QUrl m_url;
    
    CURL* m_pCurl = nullptr;                ///< libcurl���
    FILE* m_pFile = nullptr;                ///< �ļ�ָ��
    bool            m_bRunning = false;     ///< ���б�־
    std::string     m_strDownloadUrl;       ///< ��������
    std::string     m_strLocalFilePath;     ///< �����ļ�·��
};
