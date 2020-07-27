/*   
*   Copyright (c) 2008, ƮƮ����(kesalin@gmail.com)   
*   All rights reserved.   
*     
*   �ļ����ƣ�CLog.h   
*   ժ    Ҫ��Log��ʵ��
*     
*   ��ǰ�汾��1.1   
*   ��    �ߣ�ƮƮ����   
*   ������ڣ�2008/11/30
*/

class CLog {
public:

	static void output(const char* fmt, ...)
	{
#ifndef _DEBUG
		va_list ap;
		char buf[MAX_STRING_LENGTH] = {0,};

		va_start(ap, fmt);

		size_t len = _vscprintf(fmt, ap);

		if (len >= MAX_STRING_LENGTH) {
			return;
		}

		vsprintf_s(buf, len + 1, fmt, ap);
		va_end(ap);

		OutputDebugStringA(buf);
#endif
	}

protected:


private:

};
