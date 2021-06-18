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
    /// 写入回调
    static size_t handleWrite(void* buffer, size_t size, size_t nmemb, void* userp);

    /// 进度回调
    static size_t handleProgress(void* buffer, double dltotal, double dlnow, double ultotal, double ulnow);

protected:
    /// 写入回调
    size_t onWrite(void* buffer, size_t size, size_t nmemb);

    /// 进度回调
    size_t onProgress(const double& dltotal, const double& dlnow);

    /// 下载回调
    void onDownload();
protected:
    /// 设置libcurl选项
    void setOption();

    /// 清除数据
    void clear();
private:
    QTime time;
    QUrl m_url;
    
    CURL* m_pCurl = nullptr;                ///< libcurl句柄
    FILE* m_pFile = nullptr;                ///< 文件指针
    bool            m_bRunning = false;     ///< 运行标志
    std::string     m_strDownloadUrl;       ///< 下载链接
    std::string     m_strLocalFilePath;     ///< 本地文件路径
};
