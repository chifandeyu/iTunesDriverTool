#pragma once

#include <QObject>

class iTunesDriverInstall : public QObject
{
    Q_OBJECT

public:
    iTunesDriverInstall(QObject *parent);
    ~iTunesDriverInstall();
};
