/*   
*   Copyright (c) 2008, 飘飘白云(l_zhaohui@163.com)   
*   All rights reserved.   
*     
*   文件名称：WinUtility.cpp   
*   摘    要：窗口辅助函数
*     
*   当前版本：1.1   
*   作    者：飘飘白云   
*   完成日期：2008/11/30
*/

#include "WinUtility.h"

HWND WinUtility::InitWindow(HINSTANCE hInstance, TCHAR *title, int width, int height, bool windowed) {

	//
	// Create the main application window.
	//
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC) WinUtility::WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wc.hCursor = LoadCursor(0, IDC_HAND);
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wc.lpszClassName = _T("WinUtility");

	if (!RegisterClass(&wc)) {
		::MessageBox(0, _T("RegisterClass() - FAILED"), 0, 0);
		return 0;
	}

	HMENU hMenu = ::LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
	HWND hwnd = 0;
	hwnd = ::CreateWindow
		(
			_T("WinUtility"), 
			title, 
			WS_EX_TOPMOST | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME, 
			100, 0, width, height, 
			0 /*parent hwnd*/, 
			hMenu /* menu */, 
			hInstance, 
			0	/*extra*/
		);

	if (!hwnd) {
		::MessageBox(0, _T("CreateWindow() - FAILED"), 0, 0);
		return 0;
	}
	::ShowWindow(hwnd, SW_SHOW);
	::UpdateWindow(hwnd);

	return hwnd;
}

int WinUtility::EnterMsgLoop(bool (*ptr_display) (float timeDelta))
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	static float lastTime = (float)timeGetTime();

	do {
		if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else {
			float currTime = (float)timeGetTime();
			float timeDelta = (currTime - lastTime) * 0.001f;

			ptr_display(timeDelta);

			lastTime = currTime;
			Sleep(10);
		}
	} while (msg.message != WM_QUIT);

	return (int)msg.wParam;
}

// Convert CStringA to CStringW
CStringW WinUtility::convertStringA2W(CStringA srcStr)
{
	wchar_t wc[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR) srcStr, -1, wc, MAX_PATH);

	CStringW dstStr;
	dstStr.Format(L"%s", wc);
	return dstStr;
}

// Convert CStringW to CStringA
CStringA WinUtility::convertStringW2A(CStringW srcStr)
{
	char multiByte[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0, (LPCWSTR) srcStr, -1, multiByte, MAX_PATH, NULL, NULL);

	CStringA dstStr;
	dstStr.Format("%s", multiByte);
	return dstStr;
}