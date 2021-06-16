
#include <QApplication>
#include "itunesservice.h"
#include "RepairDriver.h"
#include <algorithm>
#include <QSharedMemory>
#include <QSystemSemaphore>
#include <QDebug>

const std::string fromLJtools = "ljtools";

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);

#pragma region singleApp
    //信号量的意义，把操作共享内存的代码锁住。因为有可能同时点击2次APP, 防止并发
    QSystemSemaphore sema("LJ_iTunesDriver_tool key", 1, QSystemSemaphore::Open);
    sema.acquire();

#ifdef Q_OS_LINUX
    /* Windows平台上不存在应用程序崩溃后，共享内存段还存在的情况
     * LINUX应用程序崩溃后,共享内存段不会自动销毁,则该程序再次运行会出问题
     * 所以程序启动时先去检查是否有程序崩溃后还存留的共享内存段，如果有，先销毁,再创建
     */
    QSharedMemory mem("SingleApp");
    // 尝试将进程附加到共享内存段
    if (mem.attach()) {
        // 将共享内存与主进程分离, 如果此进程是附加到共享存储器段的最后一个进程，则系统释放共享存储器段，即销毁内容
        mem.detach();
    }
#endif
    /*
     * 每个App打开的时候，获取一次共享内存。
     * 如果获取失败，说明是第一个启动的APP，直接创建共享内存就好了。
     * 如果获取成功，说明不是第一个，直接退出就好了。
     * 保证App在系统里只能打开一个。
     */
    QSharedMemory unimem("LJ_iTunesDriver_tool");
    bool isRunning = false;
    if (unimem.attach()) {
        isRunning = true;
    }
    else {
        unimem.create(1);
        isRunning = false;
    }

    sema.release();

    if (isRunning) {
        qWarning() << QStringLiteral("已经有一个LJ_iTunesDriver_tool在运行，即将退出");
        exit(0);
    }
#pragma endregion singleApp

    RepairDriver rd;
    //auto check all driver
    bool isOk = rd.checkAllDriver();
    
    //start from ljtools
    bool isFromLJTools = false;
    if (argc > 1) {
        std::string param = argv[1];
        std::transform(param.begin(), param.end(), param.begin(), tolower);
        if (!param.empty() && fromLJtools == param) {
            isFromLJTools = true;
            if (isOk) {
                return 0;
            }
        }
    }
    rd.show();
    if (!isOk && isFromLJTools) {
        rd.slotDoRepair();
    }
    return a.exec();
}
