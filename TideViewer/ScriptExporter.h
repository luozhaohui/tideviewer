/*
 *   Copyright (c) 2008, 飘飘白云(kesalin@gmail.com)
 *   All rights reserved.
 *
 *   文件名称：ScriptExporter.h
 *   摘    要：脚本编辑头文件
 *
 *   当前版本：1.1
 *   作    者：飘飘白云
 *   完成日期：2008/11/30
 */

#pragma once

////////////////////////////////////////////////////////////////////////////////
class ScriptExporter
{
public:
    ScriptExporter();
    ~ScriptExporter();

    bool open(const CStringW& fullPath, const CStringW& fileName, const CStringW& mode);
    void close();

    void setUint32(int& v, const CStringW& name);
    void setUint32(std::vector<int>& v, const CStringW& name);

    void setString(CStringW& v, const CStringW& name);
    void setString(std::vector<CStringW>& v, const CStringW& name);

    void writeString(LPCSTR string);
    void writeString(const CStringW& string);

    static bool modifyUint32(const CStringW& fullPath, const CStringW& fileName, const CStringW& item, int& v);
    static bool modifyUint32(const CStringW& fullPath, const CStringW& fileName,
                             const CStringW& item, std::vector<int>& v);
    static bool modifyString(const CStringW& fullPath, const CStringW& fileName,
                             const CStringW& item, const CStringW& string);
    static bool modifyString(const CStringW& fullPath, const CStringW& fileName,
                             const CStringW& item, const std::vector<CStringW>& string);

private:
    static bool modify(const CStringW& fullPath, const CStringW& fileName, const CStringW& item,
                       const CStringW& string);

    void* handle;

    CStringW strPath;
};