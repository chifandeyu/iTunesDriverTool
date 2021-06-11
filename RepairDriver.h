#pragma once

#include <QWidget>
#include "ui_RepairDriver.h"
#include "commonWidget.h"

//Ios驱动文件实体
typedef struct _iTunesDriverEntity
{
    QString strIndex;	    //驱动惟一标识Id
    QString strHashCode;	//驱动文件的MD5
    int nOSBITS;	        //系统位数
    QString strIosDriverName;    //驱动名称
    QString strIosUrl;		//驱动下载地址
    long nFileSize;         //文件大小
}iTunesDriverEntity, * PiTunesDriverEntity;

class RepairDriver : public commonWidget
{
    Q_OBJECT

public:
    RepairDriver(QWidget *parent = Q_NULLPTR);
    ~RepairDriver();

private:
    Ui::RepairDriver ui;
};
