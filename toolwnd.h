#ifndef TOOLWND_H
#define TOOLWND_H

#include <QMouseEvent>
#include <QApplication>
#include "itunesservice.h"

namespace Ui {
class ToolWnd;
}

#include "commonWidget.h"

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
    void setProgressValue(qint64 bytesReceived, qint64 bytesTotal, const QString& strSpeed);
    void onErorr(const QString& errStr);
    void onStartDownload();
    void onDownloadFinished();

protected:
    void showDialog();
signals:
    void sigCancel();

private slots:
    void on_closeBtn_clicked();

public slots:
    
    void slotCancel();

private:
    void connSigSlot();

private:
    Ui::ToolWnd *ui;
    QMovie* m_pIOSMovie;
};

#endif // TOOLWND_H
