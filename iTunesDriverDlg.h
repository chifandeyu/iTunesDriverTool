#pragma once

#include <QMouseEvent>
#include <QApplication>
#include "itunesservice.h"
#include <QDialog>
#include "ui_iTunesDriverDlg.h"

class iTunesDriverDlg : public QDialog
{
    Q_OBJECT
    typedef enum StackedWidget
    {
        EMMDownDrvPage = 0,		//驱动下载中
        EMMInstallingPage,		//驱动安装中
        EMMInstallSuccPage,		//驱动安装成功
        EMMInstallFailedPage,	//驱动安装失败
        EUnInstallPage,	//驱动安装失败
    }EStackedDrvWidgetPage;
public:
    iTunesDriverDlg(QWidget *parent = Q_NULLPTR);
    ~iTunesDriverDlg();
    
    void setInsatllFileName(QString name);
    void changeIOSGif();
    void setProgressValue(qint64 bytesReceived, qint64 bytesTotal, const QString& strSpeed);
    void onErorr(const QString& errStr);
    void onStartDownload();
    void onStartInstallDrv();
    void onStartUninstallDrv();
    void onInstallDrvSucc();
    void onInstallDrvFailed(QString err);

protected:
    void mousePressEvent(QMouseEvent* event) {
        mouse_is_press = true;
        mouse_move_len = event->globalPos() - this->pos();
    }
    void mouseMoveEvent(QMouseEvent* event) {
        if (mouse_is_press && (event->buttons() && Qt::LeftButton)
            && (event->globalPos() - mouse_move_len).manhattanLength() > QApplication::startDragDistance())
        {
            move(event->globalPos() - mouse_move_len);
            mouse_move_len = event->globalPos() - pos();
        }
    }
    void mouseReleaseEvent(QMouseEvent* event) {
        mouse_is_press = false;
    }
signals:
    void sigCancel();

private slots:
    void on_closeBtn_clicked();

private:
    void connSigSlot();
    void InitDownloadDrvFrame();
    void InitInstallDrvFrame();
    void InitInstallDrvSuccFrame();
    void InitInstallDrvFailedFrame(QString);
    void InitUninstallDrvFrame();

private:
    Ui::iTunesDriverDlg ui;
    QMovie* m_pIOSMovie;
    bool mouse_is_press;
    QPoint  mouse_move_len;
};
