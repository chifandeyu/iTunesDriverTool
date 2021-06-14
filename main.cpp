#include "toolwnd.h"
#include <QApplication>
#include "itunesservice.h"
#include "RepairDriver.h"
#include "SoftMgr.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    iTunesServiceCheck sv;
    sv.InitEnvironment();
    bool appSupport = sv.checkApplicationSupport32();
    bool appSupport64 = sv.checkApplicationSupport64();
    bool mobileDeviceSupport = sv.checkMobileDeviceSupport32();
    bool mobileDeviceSupport64 = sv.checkMobileDeviceSupport64();

    RepairDriver rd;
    rd.show();
    rd.checkAllDriver();
    //SoftMgr sinfo;
    //const wchar_t *wszName = L"Apple 应用程序支持 (64 位)";
    //sinfo.uninstallApp(wszName);
    //wnd.startDownloadDriver("http://10.238.7.154/qt-opensource-windows-x86-5.12.9.exe");

    return a.exec();
}
