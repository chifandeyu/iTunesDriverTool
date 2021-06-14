#pragma once

#include <QThread>

class iTunesDriverInstall : public QThread
{
    Q_OBJECT

public:
    iTunesDriverInstall(QObject *parent);
    ~iTunesDriverInstall();
    
signals:
    void sigInstall();
public slots:
    void slotInstall();

protected:
    void run();

private:
    bool isInstalling = false;
};
