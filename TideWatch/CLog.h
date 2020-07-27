/*   
*   Copyright (c) 2008, 飘飘白云(kesalin@gmail.com)   
*   All rights reserved.   
*     
*   文件名称：CLog.h   
*   摘    要：Log类实现
*     
*   当前版本：1.1   
*   作    者：飘飘白云   
*   完成日期：2008/11/30
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
