/*   
*   Copyright (c) 2008, 飘飘白云(l_zhaohui@163.com)   
*   All rights reserved.   
*     
*   文件名称：ScriptExporter.cpp   
*   摘    要：脚本编辑文件
*     
*   当前版本：1.1   
*   作    者：飘飘白云   
*   完成日期：2008/11/30
*/ 

#include "WinUtility.h"
#include "ScriptExporter.h"
#include <locale.h>

////////////////////////////////////////////////////////////////////////////////
ScriptExporter::ScriptExporter()
{
	handle = NULL;
	strPath = "";
}

ScriptExporter::~ScriptExporter(void)
{
	close();
}

bool ScriptExporter::open(const CStringW& fullPath, const CStringW& fileName, const CStringW& mode)
{
	CStringW wcPath;
	wcPath.Format(L"%s\\%s", fullPath, fileName);

	CStringW strMode;
	strMode.Format(L"%s, ccs=UTF-8", mode);

	_tsetlocale(LC_ALL, _T("chinese-simplified"));

	FILE*  fp = NULL;
	errno_t err = _tfopen_s(&fp, wcPath, strMode);
	if (err != NULL) {
		return false;
	}

	handle = (void*)fp;
	return true;
}

void ScriptExporter::close()
{
	FILE*  fp = (FILE*)handle;
	if (fp) {
		fclose(fp);
	}

	handle = NULL;
}

void ScriptExporter::setUint32(int& v, const CStringW& name)
{
	std::vector<int> vec;
	vec.push_back((int) v);
	setUint32(vec, name);
}

void ScriptExporter::setUint32(std::vector<int>& v, const CStringW& name)
{
	CStringW string;
	CStringW param;

	string.Format(name);
	string += L" <- [ ";

	int size = (int) v.size();
	for (int i = 0; i < size; i++) {
		param.Format(L"%d", (int) v[i]);
		if (i > 0) {
			string += L", ";
		}

		string += param;
	}

	string += L" ];\n";

	writeString(string);
}

void ScriptExporter::setString(CStringW& v, const CStringW& name)
{
	CStringW string;
	string.Format(L"%s <- \"%s\";\n", name, v);

	writeString(string);
}

void ScriptExporter::setString(std::vector<CStringW>& v, const CStringW& name)
{
	CStringW string;
	CStringW param;

	string = name;
	string += L" <- [ ";

	int size = (int) v.size();
	for (int i = 0; i < size; i++) {
		if (i > 0) {
			string += L", ";
		}

		param.Format(L"\"%s\"", v[i]);
		string += param;
		param.Format(L"");
	}

	string += L" ];\n";

	writeString(string);
}

void ScriptExporter::writeString(const CStringW& string)
{
	FILE*  fp = (FILE*)handle;
	if (fp) {
		fwprintf(fp, L"%s", string);
	}
}

void ScriptExporter::writeString(LPCSTR string)
{
#if 0
	CStringW wc;
	wc = string;

	FILE*  fp = (FILE*)handle;
	if (fp) {
		_ftprintf(fp, L"%s", wc);
	}
#endif
}

bool ScriptExporter::modifyUint32(const CStringW& fullPath, const CStringW& fileName, const CStringW& item, int& v)
{
	std::vector<int> vec;
	vec.push_back((int) v);
	return modifyUint32(fullPath, fileName, item, vec);
}

bool ScriptExporter::modifyUint32(
	const CStringW&	 fullPath,
	const CStringW&	 fileName,
	const CStringW&	 item,
	std::vector<int>&	v)
{
	CStringW string;
	CStringW param;

	string = item;
	string += L" <- [ ";

	int size = (int) v.size();
	for (int i = 0; i < size; i++) {
		param.Format(L"%d", (int) v[i]);
		if (i > 0) {
			string += L", ";
		}

		string += param;
	}

	string += L" ];\n";

	return modify(fullPath, fileName, item, string);
}

bool ScriptExporter::modifyString(
	const CStringW&	 fullPath,
	const CStringW&	 fileName,
	const CStringW&	 item,
	const CStringW&	 para)
{
	CStringW string;
	string.Format(L"%s <- \"%s\" ;\n", item, para);

	return modify(fullPath, fileName, item, string);
}

bool ScriptExporter::modifyString(
	const CStringW&	 fullPath,
	const CStringW&	 fileName,
	const CStringW&	 item,
	const std::vector<CStringW>&  v)
{
	CStringW string;
	string.Format(L"%s", item);
	string += L" <- [ ";

	CStringW param;
	int size = (int) v.size();
	for (int i = 0; i < size; i++)
	{
		param.Format(L"\"%s\"", v[i]);
		if (i > 0) {
			string += L", ";
		}

		string += param;
	}

	string += L" ];\n";

	return modify(fullPath, fileName, item, string);
}

bool ScriptExporter::modify(const CStringW& fullPath, const CStringW& fileName, const CStringW& item, const CStringW& string)
{
	CStringW path;
	CStringW tempPath;
	CStringW tempFile;
	path.Format(L"%s\\%s", fullPath, fileName);
	tempFile.Format(L"temp-%s", fileName);
	tempPath.Format(L"%s\\%s", fullPath, tempFile);

	// Read and write files
	wchar_t readBytes[1024] = {0,};
	CStringW readString(L"");
	wmemset(readBytes, 0, 1024);

	_tsetlocale(LC_ALL, _T("chinese-simplified"));

	FILE*  dest = NULL;
	FILE*  temp = NULL;
	errno_t destErr = _tfopen_s(&dest, path, _T("rt, ccs=UTF-8")); 
	int found = false;

	if (!destErr && dest) {
		errno_t tempErr = _tfopen_s(&temp, tempPath, _T("wt, ccs=UTF-8"));

		if (!tempErr) {
			while (fgetws(readBytes, 1024, dest) != NULL) {
				readString.Format(L"%s", readBytes);
				if (readString.Find(item) == 0) {
					found = true;
					_ftprintf(temp, L"%s", string);
				}
				else {
					_ftprintf(temp, L"%s", readBytes);
				}
			}

			if (!found) {
				_ftprintf(temp, L"\n%s", string);
			}

			if (dest) {
				fclose(dest);
				dest = NULL;
			}

			if (temp) {
				fclose(temp);
				temp = NULL;
			}


			CopyFile(tempPath, path, FALSE);
			DeleteFile(tempPath);

			return true;
		}
		else {
			if (dest) {
				fclose(dest);
				dest = NULL;
			}
		}
	}

	return false;
}

#if 0
bool ScriptExporter::decodeScript(const CStringA& dstFilePath, const CStringA& srcFilePath)
{
	bool ret = false;

	FILE*  dst = NULL;
	FILE*  src = NULL;

	errno_t srcErr = ftopen_s(&src, srcFilePath, "rb");

	if (!srcErr) {
		errno_t dstErr = fopen_s(&dst, dstFilePath, "wb");
		if (!dstErr) {
			size_t readSize = 0;
			unsigned char readBytes[1024 + 32];
			unsigned char decodeBytes[1024];
			while ((readSize = fread(readBytes, sizeof(unsigned char), 1024 + 32, src)) != 0) {
				memset(decodeBytes, 0, 1024);
				decodeData(decodeBytes, readBytes, 1024);
				if (readSize > 32) {
					fwrite(decodeBytes, sizeof(unsigned char), readSize - 32, dst);
				}
			}

			ret = true;
		}

		if (dst) {
			fclose(dst);
		}
	}

	if (src) {
		fclose(src);
	}

	return ret;
}

bool ScriptExporter::encodeScript(const CStringA& dstFilePath, const CStringA& srcFilePath)
{
	bool ret = false;

	FILE*  dst = NULL;
	FILE*  src = NULL;

	errno_t srcErr = fopen_s(&src, srcFilePath, "rb");

	if (!srcErr) {
		errno_t dstErr = fopen_s(&dst, dstFilePath, "wb");
		if (!dstErr) {
			size_t readSize = 0;
			unsigned char readBytes[1024];
			unsigned char encodeBytes[1024 + 32];
			while ((readSize = fread(readBytes, sizeof(unsigned char), 1024, src)) != 0) {
				memset(encodeBytes, 0, 1024 + 32);
				encodeData(encodeBytes, readBytes, (int)readSize);
				fwrite(encodeBytes, sizeof(unsigned char), readSize + 32, dst);
			}

			ret = true;
		}

		if (dst) {
			fclose(dst);
		}
	}

	if (src) {
		fclose(src);
	}

	return ret;
}
#endif