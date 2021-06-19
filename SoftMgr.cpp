#include "SoftMgr.h"
#include <windows.h>
#include <QDebug>
#include <QProcess>
QString wchar_tToQString(wchar_t *wchar)
{
    QString qMsg = QString::fromWCharArray(wchar);
    return qMsg;
}

BOOL Is64BitSystem()
{
    typedef VOID(WINAPI *FUN_GetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
    SYSTEM_INFO si;
    FUN_GetNativeSystemInfo pfn = (FUN_GetNativeSystemInfo)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "GetNativeSystemInfo");
    if (pfn)
    {
        pfn(&si);

        if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
            si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
        {
            return TRUE;
        }
    }
    return FALSE;
}


SoftMgr::SoftMgr(QObject *parent)
    : QObject(parent)
{
}

SoftMgr::~SoftMgr()
{
}

bool SoftMgr::GetSoftInfo()
{
    LONG lRet;
    int nIndex;
    HKEY hKey = HKEY_LOCAL_MACHINE;
    HKEY hResultKey;
    HKEY hSubResultKey;
    wchar_t  cSubKeyName[512];//注册表项名称
    DWORD lSubKeyName = 512;
    wchar_t cValueBuffer[512] = { 0 };
    DWORD dwValueType;
    DWORD dwSize = 512;
    const wchar_t* strKeyPath = L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    std::wstring strSubKeyPath = strKeyPath;
    m_strError.clear();
    m_vecSoftInfo.clear(); //清空当前的应用列表
    if (Is64BitSystem())
    {
        //32位应用跳转到WOW64_32NODE
        lRet = RegOpenKeyEx(hKey, strKeyPath, 0, KEY_READ | KEY_WOW64_32KEY, &hResultKey);
        if (lRet != 0)
        {
            m_strError = "打开注册表失败, 错误码：" + QString::number(lRet);
            return false;
        }
        nIndex = 0;
        while ((lRet = RegEnumKeyEx(hResultKey, nIndex, cSubKeyName, &lSubKeyName, 0, NULL, NULL, NULL)) == ERROR_SUCCESS)
        {
            strSubKeyPath = strKeyPath;
            strSubKeyPath = strSubKeyPath + L"\\" + cSubKeyName;
            if (RegOpenKeyEx(hKey, strSubKeyPath.c_str(), 0, KEY_READ | KEY_WOW64_32KEY, &hSubResultKey) == ERROR_SUCCESS)
            {
                SOFTINFO stSoftInfoItem = { "" };
                lRet = RegQueryValueEx(hSubResultKey, (L"DisplayName"), 0, &dwValueType, (LPBYTE)cValueBuffer, &dwSize);
                if (lRet == ERROR_SUCCESS)
                {
                    stSoftInfoItem.m_strSoftName = QString::fromUtf16(reinterpret_cast<const ushort *>(cValueBuffer), dwSize / 2 - 1);
                }
                memset(cValueBuffer, 0, sizeof(wchar_t)* dwSize);
                dwSize = 512;
                lRet = RegQueryValueEx(hSubResultKey, (L"DisplayVersion"), 0, &dwValueType, (LPBYTE)cValueBuffer, &dwSize);
                if (lRet == ERROR_SUCCESS)
                {
                    stSoftInfoItem.m_strSoftVersion = QString::fromUtf16(reinterpret_cast<const ushort *>(cValueBuffer), dwSize / 2 - 1);
                }
                memset(cValueBuffer, 0, sizeof(wchar_t)* dwSize);
                dwSize = 512;
                lRet = RegQueryValueEx(hSubResultKey, (L"InstallLocation"), 0, &dwValueType, (LPBYTE)cValueBuffer, &dwSize);
                if (lRet == ERROR_SUCCESS)
                {
                    stSoftInfoItem.m_strInstallLocation = QString::fromUtf16(reinterpret_cast<const ushort *>(cValueBuffer), dwSize / 2 - 1);
                }
                memset(cValueBuffer, 0, sizeof(wchar_t)* dwSize);
                dwSize = 512;
                lRet = RegQueryValueEx(hSubResultKey, (L"Publisher"), 0, &dwValueType, (LPBYTE)cValueBuffer, &dwSize);
                if (lRet == ERROR_SUCCESS)
                {
                    stSoftInfoItem.m_strPublisher = QString::fromUtf16(reinterpret_cast<const ushort *>(cValueBuffer), dwSize / 2 - 1);
                }
                memset(cValueBuffer, 0, sizeof(wchar_t)* dwSize);
                dwSize = 512;
                lRet = RegQueryValueEx(hSubResultKey, (L"InstallLocation"), 0, &dwValueType, (LPBYTE)cValueBuffer, &dwSize);
                if (lRet == ERROR_SUCCESS)
                {
                    stSoftInfoItem.m_strMainProPath = QString::fromUtf16(reinterpret_cast<const ushort *>(cValueBuffer), dwSize / 2 - 1);
                }
                memset(cValueBuffer, 0, sizeof(wchar_t)* dwSize);
                dwSize = 512;
                lRet = RegQueryValueEx(hSubResultKey, (L"UninstallString"), 0, &dwValueType, (LPBYTE)cValueBuffer, &dwSize);
                if (lRet == ERROR_SUCCESS)
                {
                    stSoftInfoItem.m_strUninstallPath = QString::fromUtf16(reinterpret_cast<const ushort *>(cValueBuffer), dwSize / 2 - 1);
                }
                memset(cValueBuffer, 0, sizeof(wchar_t)* dwSize);
                dwSize = 512;
                lRet = RegQueryValueEx(hSubResultKey, (L"InstallDate"), 0, &dwValueType, (LPBYTE)cValueBuffer, &dwSize);
                if (lRet == ERROR_SUCCESS)
                {
                    stSoftInfoItem.m_strInstallDate = QString::fromUtf16(reinterpret_cast<const ushort *>(cValueBuffer), dwSize / 2 - 1);
                }
                memset(cValueBuffer, 0, sizeof(wchar_t)* dwSize);
                dwSize = 512;
                if (stSoftInfoItem.m_strSoftName != "")
                {
                    m_vecSoftInfo.push_back(stSoftInfoItem);
                }
            }
            if (hSubResultKey != NULL)
            {
                RegCloseKey(hSubResultKey);
            }
            nIndex++;
            memset(cSubKeyName, 0, sizeof(wchar_t)*lSubKeyName);
            lSubKeyName = 512;
        }
        if (hResultKey != NULL)
        {
            RegCloseKey(hResultKey);
        }

        //64位应用访问注册表
        lRet = RegOpenKeyEx(hKey, strKeyPath, 0, KEY_READ | KEY_WOW64_64KEY, &hResultKey);
        if (lRet != 0)
        {
            m_strError = "打开注册表失败, 错误码：" + QString::number(lRet);
            return false;
        }
        nIndex = 0;
        while ((lRet = RegEnumKeyEx(hResultKey, nIndex, cSubKeyName, &lSubKeyName, 0, NULL, NULL, NULL)) == ERROR_SUCCESS)
        {
            strSubKeyPath = strKeyPath;
            strSubKeyPath = strSubKeyPath + L"\\" + cSubKeyName;
            if (RegOpenKeyEx(hKey, strSubKeyPath.c_str(), 0, KEY_READ | KEY_WOW64_64KEY, &hSubResultKey) == ERROR_SUCCESS)
            {
                SOFTINFO stSoftInfoItem = { "" };
                lRet = RegQueryValueEx(hSubResultKey, (L"DisplayName"), 0, &dwValueType, (LPBYTE)cValueBuffer, &dwSize);
                if (lRet == ERROR_SUCCESS)
                {
                    stSoftInfoItem.m_strSoftName = QString::fromUtf16(reinterpret_cast<const ushort *>(cValueBuffer), dwSize / 2 - 1);
                }
                memset(cValueBuffer, 0, sizeof(wchar_t)* dwSize);
                dwSize = 512;
                lRet = RegQueryValueEx(hSubResultKey, (L"DisplayVersion"), 0, &dwValueType, (LPBYTE)cValueBuffer, &dwSize);
                if (lRet == ERROR_SUCCESS)
                {
                    stSoftInfoItem.m_strSoftVersion = QString::fromUtf16(reinterpret_cast<const ushort *>(cValueBuffer), dwSize / 2 - 1);
                }
                memset(cValueBuffer, 0, sizeof(wchar_t)* dwSize);
                dwSize = 512;
                lRet = RegQueryValueEx(hSubResultKey, (L"InstallLocation"), 0, &dwValueType, (LPBYTE)cValueBuffer, &dwSize);
                if (lRet == ERROR_SUCCESS)
                {
                    stSoftInfoItem.m_strInstallLocation = QString::fromUtf16(reinterpret_cast<const ushort *>(cValueBuffer), dwSize / 2 - 1);
                }
                memset(cValueBuffer, 0, sizeof(wchar_t)* dwSize);
                dwSize = 512;
                lRet = RegQueryValueEx(hSubResultKey, (L"Publisher"), 0, &dwValueType, (LPBYTE)cValueBuffer, &dwSize);
                if (lRet == ERROR_SUCCESS)
                {
                    stSoftInfoItem.m_strPublisher = QString::fromUtf16(reinterpret_cast<const ushort *>(cValueBuffer), dwSize / 2 - 1);
                }
                memset(cValueBuffer, 0, sizeof(wchar_t)* dwSize);
                dwSize = 512;
                lRet = RegQueryValueEx(hSubResultKey, (L"InstallLocation"), 0, &dwValueType, (LPBYTE)cValueBuffer, &dwSize);
                if (lRet == ERROR_SUCCESS)
                {
                    stSoftInfoItem.m_strMainProPath = QString::fromUtf16(reinterpret_cast<const ushort *>(cValueBuffer), dwSize / 2 - 1);
                }
                memset(cValueBuffer, 0, sizeof(wchar_t)* dwSize);
                dwSize = 512;
                lRet = RegQueryValueEx(hSubResultKey, (L"UninstallString"), 0, &dwValueType, (LPBYTE)cValueBuffer, &dwSize);
                if (lRet == ERROR_SUCCESS)
                {
                    stSoftInfoItem.m_strUninstallPath = QString::fromUtf16(reinterpret_cast<const ushort *>(cValueBuffer), dwSize / 2 - 1);
                }
                memset(cValueBuffer, 0, sizeof(wchar_t)* dwSize);
                dwSize = 512;
                lRet = RegQueryValueEx(hSubResultKey, (L"InstallDate"), 0, &dwValueType, (LPBYTE)cValueBuffer, &dwSize);
                if (lRet == ERROR_SUCCESS)
                {
                    stSoftInfoItem.m_strInstallDate = QString::fromUtf16(reinterpret_cast<const ushort *>(cValueBuffer), dwSize / 2 - 1);
                }
                memset(cValueBuffer, 0, sizeof(wchar_t)* dwSize);
                dwSize = 512;
                if (stSoftInfoItem.m_strSoftName != "")
                {
                    m_vecSoftInfo.push_back(stSoftInfoItem);
                }
            }
            if (hSubResultKey != NULL)
            {
                RegCloseKey(hSubResultKey);
            }
            nIndex++;
            memset(cSubKeyName, 0, sizeof(wchar_t)*lSubKeyName);
            lSubKeyName = 512;
        }
        if (hResultKey != NULL)
        {
            RegCloseKey(hResultKey);
        }
    }
    else
    {
        lRet = RegOpenKeyEx(hKey, strKeyPath, 0, KEY_READ, &hResultKey);
        if (lRet != 0)
        {
            m_strError = "打开注册表失败, 错误码：" + QString::number(lRet);
            return false;
        }
        nIndex = 0;
        while ((lRet = RegEnumKeyEx(hResultKey, nIndex, cSubKeyName, &lSubKeyName, 0, NULL, NULL, NULL)) == ERROR_SUCCESS)
        {
            strSubKeyPath = strKeyPath;
            strSubKeyPath = strSubKeyPath + L"\\" + cSubKeyName;
            if (RegOpenKeyEx(hKey, strSubKeyPath.c_str(), 0, KEY_READ, &hSubResultKey) == ERROR_SUCCESS)
            {
                SOFTINFO stSoftInfoItem = { "" };
                lRet = RegQueryValueEx(hSubResultKey, (L"DisplayName"), 0, &dwValueType, (LPBYTE)cValueBuffer, &dwSize);
                if (lRet == ERROR_SUCCESS)
                {
                    stSoftInfoItem.m_strSoftName = QString::fromUtf16(reinterpret_cast<const ushort *>(cValueBuffer), dwSize / 2 - 1);
                }
                memset(cValueBuffer, 0, sizeof(wchar_t)* dwSize);
                dwSize = 512;
                lRet = RegQueryValueEx(hSubResultKey, (L"DisplayVersion"), 0, &dwValueType, (LPBYTE)cValueBuffer, &dwSize);
                if (lRet == ERROR_SUCCESS)
                {
                    stSoftInfoItem.m_strSoftVersion = QString::fromUtf16(reinterpret_cast<const ushort *>(cValueBuffer), dwSize / 2 - 1);
                }
                memset(cValueBuffer, 0, sizeof(wchar_t)* dwSize);
                dwSize = 512;
                lRet = RegQueryValueEx(hSubResultKey, (L"InstallLocation"), 0, &dwValueType, (LPBYTE)cValueBuffer, &dwSize);
                if (lRet == ERROR_SUCCESS)
                {
                    stSoftInfoItem.m_strInstallLocation = QString::fromUtf16(reinterpret_cast<const ushort *>(cValueBuffer), dwSize / 2 - 1);
                }
                memset(cValueBuffer, 0, sizeof(wchar_t)* dwSize);
                dwSize = 512;
                lRet = RegQueryValueEx(hSubResultKey, (L"Publisher"), 0, &dwValueType, (LPBYTE)cValueBuffer, &dwSize);
                if (lRet == ERROR_SUCCESS)
                {
                    stSoftInfoItem.m_strPublisher = QString::fromUtf16(reinterpret_cast<const ushort *>(cValueBuffer), dwSize / 2 - 1);
                }
                memset(cValueBuffer, 0, sizeof(wchar_t)* dwSize);
                dwSize = 512;
                lRet = RegQueryValueEx(hSubResultKey, (L"InstallLocation"), 0, &dwValueType, (LPBYTE)cValueBuffer, &dwSize);
                if (lRet == ERROR_SUCCESS)
                {
                    stSoftInfoItem.m_strMainProPath = QString::fromUtf16(reinterpret_cast<const ushort *>(cValueBuffer), dwSize / 2 - 1);
                }
                memset(cValueBuffer, 0, sizeof(wchar_t)* dwSize);
                dwSize = 512;
                lRet = RegQueryValueEx(hSubResultKey, (L"UninstallString"), 0, &dwValueType, (LPBYTE)cValueBuffer, &dwSize);
                if (lRet == ERROR_SUCCESS)
                {
                    stSoftInfoItem.m_strUninstallPath = QString::fromUtf16(reinterpret_cast<const ushort *>(cValueBuffer), dwSize / 2 - 1);
                }
                memset(cValueBuffer, 0, sizeof(wchar_t)* dwSize);
                dwSize = 512;
                lRet = RegQueryValueEx(hSubResultKey, (L"InstallDate"), 0, &dwValueType, (LPBYTE)cValueBuffer, &dwSize);
                if (lRet == ERROR_SUCCESS)
                {
                    stSoftInfoItem.m_strInstallDate = QString::fromUtf16(reinterpret_cast<const ushort *>(cValueBuffer), dwSize / 2 - 1);
                }
                memset(cValueBuffer, 0, sizeof(wchar_t)* dwSize);
                dwSize = 512;
                if (stSoftInfoItem.m_strSoftName != "")
                {
                    m_vecSoftInfo.push_back(stSoftInfoItem);
                }
            }
            if (hSubResultKey != NULL)
            {
                RegCloseKey(hSubResultKey);
            }
            nIndex++;
            memset(cSubKeyName, 0, sizeof(wchar_t)*lSubKeyName);
            lSubKeyName = 512;
        }
        if (hResultKey != NULL)
        {
            RegCloseKey(hResultKey);
        }
    }
    return true;
}

bool SoftMgr::uninstallApp(const wchar_t *wszName)
{
    //GetSoftInfo();
    if (!m_strError.isEmpty()) {
        qDebug() << m_strError;
    }
    qDebug() << m_vecSoftInfo.size();
    for (int i = 0; i < m_vecSoftInfo.size(); i++)
    {
        //qDebug() << QString("%1 %2 %3 %4 %5")
        //    .arg(m_vecSoftInfo[i].m_strSoftName)
        //    .arg(m_vecSoftInfo[i].m_strSoftVersion)
        //    .arg(m_vecSoftInfo[i].m_strPublisher)
        //    .arg(m_vecSoftInfo[i].m_strInstallLocation)
        //    .arg(m_vecSoftInfo[i].m_strInstallDate);

        DWORD dwSize = wcslen(wszName);
        QString name = QString::fromUtf16(reinterpret_cast<const ushort *>(wszName), dwSize);
        if (m_vecSoftInfo[i].m_strSoftName == name)
        {
            QString uninstall = m_vecSoftInfo[i].m_strUninstallPath;
            int index = uninstall.indexOf("MsiExec.exe /I",0, Qt::CaseInsensitive);
            if (index != -1) {
                uninstall = QString("MsiExec.exe /x ") + uninstall.mid(index + QString("MsiExec.exe /I").length()) + QString(" /quiet");
            }
            else {
                index = uninstall.indexOf("MsiExec.exe /X", 0, Qt::CaseInsensitive);
                if (index != -1) {
                    uninstall += QString(" /quiet");
                }
            }
            
            QProcess pp;
            qInfo() << "uninstall "<< wszName <<"run " << uninstall;
            pp.execute(uninstall);
            auto ss = QString(pp.readAllStandardError());
            if (!ss.isEmpty()) {
                qDebug() << "error: " << ss;
            }
            return ss.isEmpty();
        }
    }
    return false;
}
