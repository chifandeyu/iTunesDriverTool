#include "iTunesDriverInstall.h"

iTunesDriverInstall::iTunesDriverInstall(QObject *parent)
    : QThread(parent)
{
    this->moveToThread(this);
    connect(this, &iTunesDriverInstall::sigInstall, this, &iTunesDriverInstall::slotInstall);
}

iTunesDriverInstall::~iTunesDriverInstall()
{
}

void iTunesDriverInstall::slotInstall()
{
    isInstalling = true;

    isInstalling = false;
}

void iTunesDriverInstall::run()
{
    int ret = exec();
}
