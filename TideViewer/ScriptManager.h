/*
 *   Copyright (c) 2008, 飘飘白云(kesalin@gmail.com)
 *   All rights reserved.
 *
 *   文件名称：ScriptManager.h
 *   摘    要：脚本读取头文件
 *
 *   当前版本：1.1
 *   作    者：飘飘白云
 *   完成日期：2008/11/30
 */

#pragma once
#include "WinUtility.h"

#pragma comment(lib, "lib/sqstdlib.lib")
#pragma comment(lib, "lib/squirrel.lib")

class ScriptManager
{
public:
    ScriptManager();
    ~ScriptManager(void);

    bool open(const CStringW& filePath, const CStringW& fileName);
    void close();

    void getUint32(int& v, const CStringW& name);
    void getUint32(std::vector<int>& v, const CStringW& name);
    void getString(CStringW& v, const CStringW& name);
    void getString(std::vector<CStringW>& v, const CStringW& name);
#if 0
    static CStringW CStringA2CStringW(const CStringA& strA)
    {
        CStringW strW;
        wchar_t wc[MAX_PATH];
        ZeroMemory(wc,MAX_PATH);
        MultiByteToWideChar(CP_ACP, 0, (LPCSTR) strA, MAX_PATH, (LPWSTR) wc, MAX_PATH);
        strW.Format(L"%s", wc);
        return strW;
    }

    static CStringA CStringW2CStringA(const CStringW& strW)
    {
        CStringA strA;
        char mb[MAX_PATH];
        ZeroMemory(mb,MAX_PATH);
        WideCharToMultiByte(CP_ACP, 0, (LPCWSTR) strW, -1, mb, MAX_PATH, NULL, NULL);
        strA.Format("%s", mb);
        return strA;
    }
#endif
private:
    void* handle;
};