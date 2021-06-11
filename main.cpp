#include "toolwnd.h"
#include <QApplication>
#include "itunesservice.h"
#include "RepairDriver.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ToolWnd wnd;
    wnd.show();
    wnd.InitDownloadIosDrvFrame();
    iTunesServiceCheck sv;
    sv.InitEnvironment();
    sv.checkAppleAppSupport();
    sv.checkAppleMDeviceSupport();

    RepairDriver rd;
    rd.show();

    wnd.startDownloadDriver("http://10.238.7.154/qt-opensource-windows-x86-5.12.9.exe");

    return a.exec();
}
