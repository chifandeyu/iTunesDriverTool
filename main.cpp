
#include <QApplication>
#include "itunesservice.h"
#include "RepairDriver.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RepairDriver rd;
    //auto check all driver
    bool isOk = rd.checkAllDriver();
    if (!isOk) {
        rd.slotDoRepair();
    }
    rd.show();
    return a.exec();
}
