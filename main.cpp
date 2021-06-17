
#include <QApplication>
#include "itunesservice.h"
#include "RepairDriver.h"
#include <algorithm>
#include <QSharedMemory>
#include <QSystemSemaphore>
#include <QDebug>
#ifdef Q_OS_WIN32
#include <Windows.h>
#endif

const std::string autoCheck = "auto";
const char* activeWnd = "activeWnd";
LPCWSTR mainwnd = L"RepairDriver";

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/res/itunes_tool.ico"));
#pragma region singleApp
    //�ź��������壬�Ѳ��������ڴ�Ĵ�����ס����Ϊ�п���ͬʱ���2��APP, ��ֹ����
    QSystemSemaphore sema("LJ_iTunesDriver_tool key", 1, QSystemSemaphore::Open);
    sema.acquire();

#ifdef Q_OS_LINUX
    /* Windowsƽ̨�ϲ�����Ӧ�ó�������󣬹����ڴ�λ����ڵ����
     * LINUXӦ�ó��������,�����ڴ�β����Զ�����,��ó����ٴ����л������
     * ���Գ�������ʱ��ȥ����Ƿ��г�������󻹴����Ĺ����ڴ�Σ�����У�������,�ٴ���
     */
    QSharedMemory mem("SingleApp");
    // ���Խ����̸��ӵ������ڴ��
    if (mem.attach()) {
        // �������ڴ��������̷���, ����˽����Ǹ��ӵ�����洢���ε����һ�����̣���ϵͳ�ͷŹ���洢���Σ�����������
        mem.detach();
    }
#endif
    /*
     * ÿ��App�򿪵�ʱ�򣬻�ȡһ�ι����ڴ档
     * �����ȡʧ�ܣ�˵���ǵ�һ��������APP��ֱ�Ӵ��������ڴ�ͺ��ˡ�
     * �����ȡ�ɹ���˵�����ǵ�һ����ֱ���˳��ͺ��ˡ�
     * ��֤App��ϵͳ��ֻ�ܴ�һ����
     */
    QSharedMemory unimem("LJ_iTunesDriver_tool");
    bool isRunning = false;
    if (unimem.attach()) {
        isRunning = true;
        HWND receiveWindow = ::FindWindow(NULL, mainwnd);
        if (receiveWindow)
        {
            COPYDATASTRUCT copyData = { 0 };
            copyData.lpData = (PVOID)activeWnd;
            copyData.cbData = strlen(activeWnd) + 1;
            ::SendMessage(receiveWindow, WM_COPYDATA, NULL, (LPARAM)&copyData);
        }
    }
    else {
        unimem.create(1);
        isRunning = false;
    }

    sema.release();

    if (isRunning) {
        qWarning() << QStringLiteral("�Ѿ���һ��LJ_iTunesDriver_tool�����У������˳�");
        exit(0);
    }
#pragma endregion singleApp
    
    RepairDriver rd;
    //auto check all driver
    bool isOk = rd.checkAllDriver();
    
    //start from ljtools
    bool isAutoCheck = false;
    if (argc > 1) {
        std::string param = argv[1];
        std::transform(param.begin(), param.end(), param.begin(), tolower);
        if (!param.empty() && autoCheck == param) {
            isAutoCheck = true;
            if (isOk) {
                return 0;
            }
        }
    }
    rd.show();
    if (!isOk && isAutoCheck) {
        rd.slotDoRepair();
    }
    return a.exec();
}
