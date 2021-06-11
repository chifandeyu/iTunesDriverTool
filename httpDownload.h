#pragma once

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QFile>

class httpDownload : public QObject
{
    Q_OBJECT

public:
    httpDownload(QObject *parent = nullptr);
    virtual ~httpDownload();

    bool download(const QString& strUrl);
    void cancel();
    void pause();
    static QString transformUnit(qint64 bytes, bool isSpeed);//大小转成字符串
    bool isDownload() const {
        return m_isDownloading;
    }

private:
    void startRequest(QUrl url);

signals:
    void sigErorr(const QString& errStr);
    void sigStartDownload();
    void sigProgress(qint64 bytesReceived, qint64 bytesTotal, const QString& strSpeed);
    void sigFinished();
    void sigCancel();

private slots:
    void httpReadyRead();
    void httpFinished();
    void updateProgress(qint64 bytesReceived, qint64 bytesTotal);
    void httpError(QNetworkReply::NetworkError);

private:
    QNetworkAccessManager* m_pManager = nullptr;
    QNetworkReply* m_pReply = nullptr;
    QUrl m_url;
    QFile* m_pFile = nullptr;
    QTime time;
    bool m_isDownloading = false;
    int m_currentDownload;
    int m_intevalDownload;// m_intervalDownload 为下次计算速度之前的下载字节数;
};
