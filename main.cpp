
#include <QApplication>
#include "itunesservice.h"
#include "RepairDriver.h"
#include "SoftMgr.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //iTunesServiceCheck sv;
    //sv.InitEnvironment();
    //bool appSupport = sv.checkAAS32();
    //bool appSupport64 = sv.checkAAS64();
    //bool mobileDeviceSupport = sv.checkAMDS32();
    //bool mobileDeviceSupport64 = sv.checkAMDS64();

    RepairDriver rd;
    rd.show();
    bool isOk = rd.checkAllDriver();
    if (!isOk) {
        rd.slotDoRepair();
    }
    return a.exec();
}
