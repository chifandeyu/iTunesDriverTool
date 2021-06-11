#pragma once

#include <QObject>
#include <QSettings>

class Setting : public QObject
{
    Q_OBJECT

public:
    Setting(QObject *parent);
    ~Setting();
};
