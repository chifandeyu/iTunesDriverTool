#ifndef TOOLWND_H
#define TOOLWND_H

#include <QMouseEvent>
#include <QApplication>
#include "itunesservice.h"

namespace Ui {
class ToolWnd;
}

#include "commonWidget.h"

class httpDownload;

class ToolWnd : public commonWidget
{
    Q_OBJECT

    typedef enum StackedWidget
    {
        EMMDownDrvPage = 0,		//驱动下载中
        EMMInstallingPage,		//驱动安装中
        EMMInstallSuccPage,		//驱动安装成功
        EMMInstallFailedPage,	//驱动安装失败
    }EStackedDrvWidgetPage;

public:
    explicit ToolWnd(QWidget *parent = nullptr);
    ~ToolWnd();

    void InitDownloadIosDrvFrame();
    void InitInstallIosDrvFrame();
    void InitInstallIosDrvSuccFrame();
    void InitInstallDrvFailedFrame(QString);
    void setInsatllFileName(QString name);
    void changeIOSGif();
    void setProgressValue( int val ,qint64 bytesReceived=0,qint64 bytesTotal=0, QString strSpeed="");
    void startDownloadDriver(const QString& fileUrl);

protected:
    void showDialog();

private slots:
    void on_closeBtn_clicked();

public slots:
    void slotProgress(qint64 bytesReceived, qint64 bytesTotal, const QString& strSpeed);
    void slotErorr(const QString& errStr);
    void slotStartDownload();
    void slotFinished();
    void slotCancel();

private:
    void connSigSlot();

private:
    Ui::ToolWnd *ui;
    QMovie* m_pIOSMovie;
    httpDownload* m_pDownloader = nullptr;
    QString m_fileUrl;
};

#endif // TOOLWND_H
