#include "itunesservice.h"
#include <windows.h>
#include <QDebug>
#include <QFile>
#include <QCryptographicHash>

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

bool IsServiceRunning(LPCTSTR pszName)
{
    bool			ret;
    BOOL			bRet;
    SC_HANDLE		hManager;
    SC_HANDLE		hService;
    SERVICE_STATUS	sStatus;

    ret = false;
    hManager = NULL;
    hService = NULL;
    while (1)
    {
        hManager = ::OpenSCManager(NULL, NULL, GENERIC_EXECUTE);
        if (hManager == NULL)
            break;

        hService = ::OpenService(hManager, pszName, SERVICE_QUERY_STATUS);
        if (hService == NULL)
            break;

        ::ZeroMemory(&sStatus, sizeof(SERVICE_STATUS));
        bRet = ::QueryServiceStatus(hService, &sStatus);
        if (bRet == FALSE)
            break;

        if (sStatus.dwCurrentState == SERVICE_RUNNING)
            ret = true;

        break;
    }

    if (hService)
        ::CloseServiceHandle(hService);
    if (hManager)
        ::CloseServiceHandle(hManager);

    return	ret;
}

bool StartSevice(LPCTSTR pszName)
{
    bool			ret = false;
    BOOL			bRet = FALSE;
    SC_HANDLE		hManager = NULL;
    SC_HANDLE		hService = NULL;
    SERVICE_STATUS	sStatus;

    hManager = ::OpenSCManager(NULL, NULL, GENERIC_EXECUTE);
    if (hManager == NULL)
    {
        return false;
    }

    hService = ::OpenService(hManager, pszName, SERVICE_START | SERVICE_QUERY_STATUS);
    if (hService == NULL)
    {
        if (hManager) { ::CloseServiceHandle(hManager); }
        return false;
    }

    ::ZeroMemory(&sStatus, sizeof(SERVICE_STATUS));
    bRet = ::QueryServiceStatus(hService, &sStatus);
    if (bRet == FALSE)
    {
        if (hService) { ::CloseServiceHandle(hService); }
        if (hManager) { ::CloseServiceHandle(hManager); }
        return false;
    }

    if (sStatus.dwCurrentState == SERVICE_RUNNING)
    {
        if (hService) { ::CloseServiceHandle(hService); }
        if (hManager) { ::CloseServiceHandle(hManager); }
        return true;
    }

    bRet = ::StartService(hService, NULL, NULL);

    return bRet;
}

iTunesServiceCheck::iTunesServiceCheck(QObject *parent) : QObject(parent)
{

}

//IsWow64返回TRUE则是64位系统，否则为32位系统。
bool iTunesServiceCheck::IsWow64()
{
    BOOL bIsWow64 = FALSE;
    LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;
    fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
        GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

    if (NULL != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
        {
            return FALSE;
        }
    }
    return (bIsWow64 == TRUE);
}

QString iTunesServiceCheck::fileMd5(const QString & filePath)
{
    QString sMd5;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray bArray = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5);
        sMd5 = QString(bArray.toHex()).toUpper();
    }
    file.close();
    return sMd5;
}

int iTunesServiceCheck::InitEnvironment()
{
    /*
        首先得验证所有exe是否有安装和服务是否运行
        如果没有安装和运行 则无法使用ios模块
        此处先不做这些处理
    */
    int iReturn =  0;
    HKEY hKey = NULL;
    DWORD dwType, dwPathSize = MAX_PATH;
    LSTATUS lStatus = ERROR_SUCCESS;
    wchar_t wszAppleASPath[MAX_PATH] = L"\0";
    wchar_t wszMobileDPath[MAX_PATH] = L"\0";

    LPWSTR lpwStrPath = NULL;
    DWORD dwResult, dwSize;

    //Apple Application Support
    lStatus = RegOpenKeyW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Apple Inc.\\Apple Application Support", &hKey);
    if (ERROR_SUCCESS != lStatus)
    {
        qDebug() << QString("RegOpenKey Apple Application Support Failed: %d").arg(lStatus);
        iReturn = -1;
        goto __ReturnClean;
    }

    lStatus = RegQueryValueExW(
        hKey, L"InstallDir", NULL, &dwType, (LPBYTE)wszAppleASPath, &dwPathSize);
    if (ERROR_SUCCESS != lStatus || '\0' == wszAppleASPath[0])
    {
        qDebug() << QString("RegQueryValue Apple Application Support Path Failed: %d").arg(lStatus);
        iReturn = -2;
        goto __ReturnClean;
    }

    //Mobile Device Support
    {
        int iLen = wcslen(wszAppleASPath);
        if (L'\\' == wszAppleASPath[iLen - 1])
            wszAppleASPath[iLen - 1] = L'\0';

        wcscpy(wszMobileDPath, wszAppleASPath);
        wchar_t* pwszTmp = wcsrchr(wszMobileDPath, L'\\');
        if (NULL == pwszTmp)
        {
            qDebug() << QString("Apple Application Support Path Is Error!");
            iReturn = -3;
            goto __ReturnClean;
        }

        pwszTmp[1] = L'\0';
        wcscat(wszMobileDPath, L"Mobile Device Support");
    }

    dwResult = GetEnvironmentVariableW(L"Path", lpwStrPath, 0);
    dwSize = (dwResult + 2 * MAX_PATH) * sizeof(wchar_t);
    lpwStrPath = (LPWSTR)malloc(dwSize);
    memset(lpwStrPath, 0, dwSize);
    dwResult = GetEnvironmentVariableW(L"Path", lpwStrPath, dwSize);

    wcscat(lpwStrPath, L";");
    wcscat(lpwStrPath, wszAppleASPath);
    wcscat(lpwStrPath, L";");
    wcscat(lpwStrPath, wszMobileDPath);
    wcscat(lpwStrPath, L";");
    if (!SetEnvironmentVariableW(L"Path", lpwStrPath))
    {
        qDebug() << QString("SetEnvironmentVariable Failed:%d").arg(GetLastError());
        iReturn = -4;
        goto __ReturnClean;
    }

__ReturnClean:
    if (NULL != hKey) RegCloseKey(hKey);
    return iReturn;
}

bool iTunesServiceCheck::servicIsRunning()
{
    return (IsServiceRunning(L"Apple Mobile Device Service") 
        || IsServiceRunning(L"Apple Mobile Device"));
}

bool iTunesServiceCheck::checkAAS32()
{
    wchar_t InstallDir[MAX_PATH];
    bool hasInstall = checkReg("SOFTWARE\\Apple Inc.\\Apple Application Support", "InstallDir", InstallDir, true);
    QString ret = QString::fromWCharArray(InstallDir);
    return hasInstall;
}

bool iTunesServiceCheck::checkAAS64()
{
    wchar_t InstallDir[MAX_PATH];
    bool hasInstall = checkReg("SOFTWARE\\Apple Inc.\\Apple Application Support", "InstallDir", InstallDir);
    QString ret = QString::fromWCharArray(InstallDir);
    return hasInstall;
}

bool iTunesServiceCheck::checkAMDS32()
{
    wchar_t InstallDir[MAX_PATH];
    bool hasInstall = checkReg("SOFTWARE\\Apple Inc.\\Apple Mobile Device Support", "InstallDir", InstallDir, true);
    QString ret = QString::fromWCharArray(InstallDir);
    return hasInstall;
}

bool iTunesServiceCheck::checkAMDS64()
{
    wchar_t InstallDir[MAX_PATH];
    bool hasInstall = checkReg("SOFTWARE\\Apple Inc.\\Apple Mobile Device Support", "InstallDir", InstallDir);
    QString ret = QString::fromWCharArray(InstallDir);
    return hasInstall;
}

bool iTunesServiceCheck::checkReg(const QString& strKey, const QString& strName, wchar_t* pWszValue, bool check32_Bit)
{
    LSTATUS lStatus = ERROR_SUCCESS;
    HKEY hKey = NULL;
    DWORD dwType, dwPathSize = MAX_PATH;
    wchar_t wszKey[MAX_PATH] = { 0 };
    int len = strKey.toWCharArray(wszKey);
    wchar_t wszName[MAX_PATH] = { 0 };
    int nameLen = strName.toWCharArray(wszName);
    //Apple Application Support
    bool is64Bit = IsWow64();
    if (is64Bit) {
        if (check32_Bit) {
            lStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, wszKey/*L"SOFTWARE\\Apple Inc.\\Apple Application Support"*/, 0, KEY_READ | KEY_WOW64_32KEY, &hKey);
        }
        else {
            lStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, wszKey/*L"SOFTWARE\\Apple Inc.\\Apple Application Support"*/, 0, KEY_READ | KEY_WOW64_64KEY, &hKey);
        }
    }
    else {
        lStatus = RegOpenKeyW(HKEY_LOCAL_MACHINE, wszKey/*L"SOFTWARE\\Apple Inc.\\Apple Application Support"*/, &hKey);
    }

    if (ERROR_SUCCESS != lStatus || !hKey)
    {
        qDebug() << QString("RegOpenKey %1 Failed: %2").arg(strKey).arg(lStatus);
        return false;
    }

    lStatus = RegQueryValueExW(
        hKey, wszName, NULL, &dwType, (LPBYTE)pWszValue, &dwPathSize);
    if (ERROR_SUCCESS != lStatus || '\0' == pWszValue[0])
    {
        qDebug() << QString("RegQueryValue %1 InstallDir Failed: %2").arg(strKey).arg(lStatus);
        return false;
    }

    return true;
}
