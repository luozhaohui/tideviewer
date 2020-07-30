/*   
*   Copyright (c) 2008, ƮƮ����(kesalin@gmail.com)   
*   All rights reserved.   
*     
*   �ļ����ƣ�ScriptExporter.h   
*   ժ    Ҫ���ű��༭ͷ�ļ�
*     
*   ��ǰ�汾��1.1   
*   ��    �ߣ�ƮƮ����   
*   ������ڣ�2008/11/30
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
class ScriptExporter {
public:
	ScriptExporter();
	~ ScriptExporter(void);

	bool open(const CStringW& fullPath, const CStringW& fileName, const CStringW& mode);
	void close();

	void setUint32(int& v, const CStringW& name);
	void setUint32(std::vector<int> &v, const CStringW& name);

	void setString(CStringW& v, const CStringW& name);
	void setString(std::vector<CStringW> &v, const CStringW& name);

	void writeString(LPCSTR string);
	void writeString(const CStringW& string);

	static bool modifyUint32(const CStringW& fullPath, const CStringW& fileName, const CStringW& item, int& v);
	static bool modifyUint32(const CStringW& fullPath, const CStringW& fileName, const CStringW& item, std::vector<int> &v);
	static bool modifyString(const CStringW& fullPath, const CStringW& fileName, const CStringW& item, const CStringW& string);
	static bool modifyString(const CStringW& fullPath, const CStringW& fileName, const CStringW& item,  const std::vector<CStringW>& string);

private:
	static bool modify(const CStringW& fullPath, const CStringW& fileName, const CStringW& item, const CStringW& string);
	
	void*  handle;

	CStringW strPath;
};
