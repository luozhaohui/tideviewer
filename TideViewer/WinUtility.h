/*   
*   Copyright (c) 2008, ƮƮ����(kesalin@gmail.com)   
*   All rights reserved.   
*     
*   �ļ����ƣ�ScriptManager.h   
*   ժ    Ҫ�����ڸ�������ͷ�ļ�
*     
*   ��ǰ�汾��1.1   
*   ��    �ߣ�ƮƮ����   
*   ������ڣ�2008/11/30
*/
#pragma once
#include <Windows.h>
#include <string>
#include <assert.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
#include <time.h>
#include <wchar.h>

// ATL/WTL Header Files
#include <atlbase.h>	//
#include <atlapp.h> //
#include <atlwin.h> // ATL
#include <atlframe.h>	// WTL
#include <atlmisc.h>	// WTL
#include <atlcrack.h>	// WTL
#include <atlstr.h>
#include <atlcoll.h>	// CAtlArray
#include <atlctrls.h>	// CImageList
#include <map>
#include <vector>
#include <algorithm>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#ifdef USE_ATL_MODULE
extern CComModule _Module;	// _Module
#else
extern CAppModule _Module;	// WTL
#endif
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#define MAX_STRING_LENGTH	1024

const static int DIG_MSG_LOGIN = 1000;
const static int MENU_ID_START = 2000;

#include "resource.h"

namespace WinUtility {
HWND InitWindow(
		HINSTANCE hInstance,	// [in] Application instance.
		TCHAR*	title,
		int width,
		int height, // [in] Backbuffer dimensions.
		bool windowed	// [in] Windowed (true)or full screen (false).
		);

int EnterMsgLoop(bool (*ptr_display) (float timeDelta));

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

CStringA convertStringW2A(CStringW srcStr);
CStringW convertStringA2W(CStringA srcStr);

template<class T>
void Release(T t) {
	if (t) {
		t->Release();
		t = 0;
	}
}

template<class T>
void Delete(T t) {
	if (t) {
		delete t;
		t = 0;
	}
}
}
