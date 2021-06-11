#pragma once

#include <QWidget>
#include "ui_RepairDriver.h"
#include "commonWidget.h"

//Ios�����ļ�ʵ��
typedef struct _iTunesDriverEntity
{
    QString strIndex;	    //����Ωһ��ʶId
    QString strHashCode;	//�����ļ���MD5
    int nOSBITS;	        //ϵͳλ��
    QString strIosDriverName;    //��������
    QString strIosUrl;		//�������ص�ַ
    long nFileSize;         //�ļ���С
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
