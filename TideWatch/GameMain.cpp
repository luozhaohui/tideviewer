/*   
*   Copyright (c) 2008, 飘飘白云(l_zhaohui@163.com)   
*   All rights reserved.   
*     
*   文件名称：GameMain.cpp   
*   摘    要：主框架文件
*     
*   当前版本：1.1   
*   作    者：飘飘白云   
*   完成日期：2008/11/30
*/
#include "WinUtility.h"
#include "Game.h"
#include "CLog.h"

//
// Globals
//
HINSTANCE hInst;	// current instance
ULONG_PTR token_;
Game game;

// Functions
INT_PTR CALLBACK HelpCallback(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AboutCallback(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK LoginCallback(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ModifyCallback(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK LookCallback(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK InputCallback(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK InputCallbackEx(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ComputeDeepthCallback(HWND, UINT, WPARAM, LPARAM);

void updateResultOne(HWND hDlg, bool needSave = true);
void updateResultTwo(HWND hDlg, bool needSave = true);

//
// Framework Functions
//
bool Setup(HWND hwnd)
{
	// Init GDI++
	Gdiplus::GdiplusStartupInput input;
	Gdiplus::GdiplusStartupOutput output = { 0, 0};
	Gdiplus::Status ret = Gdiplus::GdiplusStartup(&token_, &input, &output);
	if (ret != Gdiplus::Ok) {
		MessageBox(NULL, _T("Fail to init GDI++!"), _T("I am sorry~"), MB_OK);
		return false;
	}

	game.init(hwnd);

	return true;
}

void Cleanup()
{
	game.clear();

	// Shut down GDI++
	Gdiplus::GdiplusShutdown(token_);
}

bool Display(float timeDelta)
{
	game.enter();
	game.draw();

	return true;
}

//
// WndProc
//
LRESULT CALLBACK WinUtility::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_DESTROY:
		// 关闭窗口
		::PostQuitMessage(WM_QUIT);
		break;
	case WM_INITMENUPOPUP:
		// 系统菜单
		if (lParam == 0) {

			// 已登录
			if (game.isLogined()) {
				EnableMenuItem((HMENU) wParam, ID_LOGIN, MF_GRAYED);
				EnableMenuItem((HMENU) wParam, ID_MODIFY_PWD, MF_ENABLED);
				EnableMenuItem((HMENU) wParam, ID_INPUT_DATA, MF_ENABLED);
			}

			// 未登录
			else {
				EnableMenuItem((HMENU) wParam, ID_MODIFY_PWD, MF_GRAYED);
				EnableMenuItem((HMENU) wParam, ID_INPUT_DATA, MF_GRAYED);
			}
		}

		// 查看菜单
		else if (lParam == 1) {

			// 未登录，以下菜单禁止操作
			if (!game.isLogined()) {
				EnableMenuItem((HMENU) wParam, ID_TODAY, MF_GRAYED);
				EnableMenuItem((HMENU) wParam, ID_PREV, MF_GRAYED);
				EnableMenuItem((HMENU) wParam, ID_NEXT, MF_GRAYED);
				EnableMenuItem((HMENU) wParam, ID_LOOK, MF_GRAYED);
			}
			else {

				// 已登录
				EnableMenuItem((HMENU) wParam, ID_TODAY, MF_ENABLED);
				EnableMenuItem((HMENU) wParam, ID_PREV, MF_ENABLED);
				EnableMenuItem((HMENU) wParam, ID_NEXT, MF_ENABLED);
				EnableMenuItem((HMENU) wParam, ID_LOOK, MF_ENABLED);
			}
		}
		break;
	case WM_COMMAND:
		if (wParam == ID_START) {

			// 退出菜单
			game.exit();
		}
		else if (wParam == ID_LOGIN) {

			// 登录菜单
			DialogBoxW(hInst, MAKEINTRESOURCE(IDD_LOGIN), hwnd, LoginCallback);
		}
		else if (wParam == ID_LOOK) {

			//查看菜单
			DialogBoxW(hInst, MAKEINTRESOURCE(IDD_LOOK), hwnd, LookCallback);
		}
		else if (wParam == ID_MODIFY_PWD) {

			// 修改密码菜单
			DialogBoxW(hInst, MAKEINTRESOURCE(IDD_MODIFY), hwnd, ModifyCallback);
		}
		else if (wParam == ID_ABOUT) {

			// 关于菜单
			DialogBoxW(hInst, MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutCallback);
		}
		else if (wParam == ID_HELP) {

			// 帮助菜单
			game.fileId = Game::FILE_HELP;
			DialogBoxW(hInst, MAKEINTRESOURCE(IDD_HELP), hwnd, HelpCallback);
		}
		else if (wParam == IDM_BOAT_INFO)
		{
			// 拖轮情况菜单
			game.fileId = Game::FILE_TUOLUN;
			DialogBoxW(hInst, MAKEINTRESOURCE(IDD_HELP), hwnd, HelpCallback);
		}
		else if (wParam == IDM_MATOU_INFO)
		{
			// 码头情况菜单
			game.fileId = Game::FILE_MATOU;
			DialogBoxW(hInst, MAKEINTRESOURCE(IDD_HELP), hwnd, HelpCallback);
		}
		else if (wParam == IDM_HANGDAO_INFO)
		{
			// 航道情况菜单
			game.fileId = Game::FILE_HANGDAO;
			DialogBoxW(hInst, MAKEINTRESOURCE(IDD_HELP), hwnd, HelpCallback);
		}
		else if (wParam == ID_INPUT_DATA) {

			// 数据录入菜单
			//DialogBoxW(hInst, MAKEINTRESOURCE(IDD_INPUT_DATA), hwnd, InputCallback);
			DialogBoxW(hInst, MAKEINTRESOURCE(IDD_INPUT_DATA_EX), hwnd, InputCallbackEx);
		}
		else if (wParam == ID_TODAY) {

			// 查看今天
			game.loadToday();
		}
		else if (wParam == ID_PREV) {

			// 查看前一天
			game.loadPrev();
		}
		else if (wParam == ID_NEXT) {

			// 查看后一天
			game.loadNext();
		}
		else if (wParam == ID_CALCU) {

			// 弹出计算器
			ShellExecute(NULL, _T("open"), _T("calc.exe"), NULL, NULL, SW_SHOWNORMAL);
		}

		else if (wParam == ID_COMPUTER_DEEPTH) {
			// 计算富裕水深
			DialogBoxW(hInst, MAKEINTRESOURCE(IDD_COMPUTE), hwnd, ComputeDeepthCallback);
		}

		else if (LOWORD(wParam) >= MENU_ID_START && LOWORD(wParam) < MENU_ID_START + game.getMenuMax()) {

			// 响应右键菜单
			game.processMenu(LOWORD(wParam) - MENU_ID_START);
		}
		break;

	// 处理键盘消息
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {

			// 按ESC键关闭应用程序
			::PostQuitMessage(WM_QUIT);
		}
		else {
			Point point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			game.onKeyDown((int)wParam, point);
		}
		break;

	// 处理鼠标移动消息
	case WM_MOUSEMOVE: {
			Point point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			game.onMouseMove(point);
		}
		break;

	// 处理鼠标左键消息
	case WM_LBUTTONDOWN: {
			Point point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			game.onLeftButtonDown(point);
		}
		break;

	// 处理鼠标右键键消息
	case WM_RBUTTONDOWN: {
			Point point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			game.onRightButtonDown(point);
		}
		break;
	case WM_PAINT: {
			PAINTSTRUCT ps;
			BeginPaint(hwnd, &ps);	// validate the window
			EndPaint(hwnd, &ps);

			return(0);	// return success
		}
	}

	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

// Message handler for help box.
INT_PTR CALLBACK HelpCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG: {
			HWND parent = ::GetParent(hDlg);
			RECT rcParent = {0,0,0,0};
			RECT rcDlg = {0,0,0,0};
			::GetWindowRect(parent, &rcParent);
			::GetWindowRect(hDlg, &rcDlg);

			int x = int(rcParent.left + ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
			int y = int(rcParent.top + ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2);

			::SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

			HWND hPwd = ::GetDlgItem(hDlg, IDC_HELP_TXT);
			if (hPwd)
			{
#define MAX_SIZE	(1024 * 30)
				wchar_t fullPath[MAX_PATH] = {0, };
				GetModuleFileNameW(0, fullPath, MAX_PATH);
				PathRemoveFileSpecW(fullPath);

				switch(game.fileId)
				{
				case Game::FILE_HELP:
					::SetWindowText(hDlg, L"帮助");
					PathAppend(fullPath, L"help.txt");
					break;
				case Game::FILE_MATOU:
					::SetWindowText(hDlg, L"码头情况");
					PathAppend(fullPath, L"码头情况.txt");
					break;
				case Game::FILE_TUOLUN:
					::SetWindowText(hDlg, L"拖轮情况");
					PathAppend(fullPath, L"拖轮情况.txt");
				    break;
				case Game::FILE_HANGDAO:
					::SetWindowText(hDlg, L"航道情况");
					PathAppend(fullPath, L"航道情况.txt");
				    break;
				default:
				    return (INT_PTR) TRUE;
				}

				CStringW strMode;
				strMode.Format(L"r, ccs=UTF-8");
				_tsetlocale(LC_ALL, _T("chinese-simplified"));

				FILE*  fp = NULL;
				errno_t err = _wfopen_s(&fp, fullPath, strMode);
				if (!err && fp != NULL) {
					wchar_t buf[MAX_SIZE] = { 0, };
					fread(buf, sizeof(wchar_t), MAX_SIZE, fp);
					fgetws(buf, MAX_SIZE, fp);

					for (int i = 0; i < MAX_SIZE && buf[i] != 0; i++) {
						if ((i + 1) < MAX_SIZE && buf[i] == 0x0a && buf[i + 1] == 0x0a) {
							buf[i] = 0x0d;
							i++;
						}
					}

					::SetWindowText(hPwd, buf);
					fclose(fp);
				}
			}
		}

		return(INT_PTR) TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return(INT_PTR) TRUE;
		}
		break;
	}

	return(INT_PTR) FALSE;
}

// Message handler for input box.
INT_PTR CALLBACK InputCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG: {
			HWND parent = ::GetParent(hDlg);
			RECT rcParent = {0,0,0,0};
			RECT rcDlg = {0,0,0,0};
			::GetWindowRect(parent, &rcParent);
			::GetWindowRect(hDlg, &rcDlg);

			int x = int(rcParent.left + ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
			int y = int(rcParent.top + ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2);

			::SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}

		return(INT_PTR) TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK) {
			const static int LENGTH = 512;
			wchar_t*  wc = new wchar_t[LENGTH];
			wmemset(wc, 0, LENGTH);
			GetDlgItemText(hDlg, IDC_PLACE, wc, LENGTH);

			CStringW place(wc);

			if (place.IsEmpty()) {
				delete[] wc;

				MessageBox(hDlg, L"输入观察点不能为空，请重新输入！", L"错误", MB_OK);
				break;
			}

			wmemset(wc, 0, LENGTH);
			GetDlgItemText(hDlg, IDC_INPUT_DATE, wc, LENGTH);

			CStringW month(wc);

			CStringW errInfo;
			CStringW date;
			if (!month.IsEmpty()) {
				date.Format(L"%s01", month);
			}

			if (month.IsEmpty() || !game.checkDate(date)) {
				delete[] wc;

				MessageBox(hDlg, L"输入年份与月份格式不正确，请重新输入！", L"错误", MB_OK);
				SetDlgItemText(hDlg, IDC_INPUT_DATE, L"");
				break;
			}

#if 1
			// Check and input data.
			std::vector<int> clearIndex;
			CStringW day;
			CStringW data;
			for (int i = 0; i < 6; i++) {
				day.Empty();
				data.Empty();

				int dateDlgId = IDC_DATE1 + i;
				int dataDlgId = IDC_DATA1 + i;

				// get date.
				wmemset(wc, 0, LENGTH);
				GetDlgItemText(hDlg, dateDlgId, wc, LENGTH);
				day.Format(L"%s", wc);
				if (day.GetLength() == 1) {

					//day.Format(L"0%s", day);
					day = L"0" + day;
				}

				// check date.
				if (!day.IsEmpty()) {
					date.Format(L"%s%s", month, day);

					if (!game.checkDate(date)) {
						errInfo.Format(L"输入日子（%s）不合法，请重新输入！", day);
						MessageBox(hDlg, errInfo, L"错误", MB_OK);
						SetDlgItemText(hDlg, dateDlgId, L"");
						break;
					}

					// get tide data.
					wmemset(wc, 0, LENGTH);
					GetDlgItemText(hDlg, dataDlgId, wc, LENGTH);
					data.Format(L"%s", wc);

					if (data.IsEmpty() || !game.formatInputData(data) || !game.checkInputData(data)) {
						errInfo.Format(L"输入%s号的潮高数据不符合格式，检查后请重新输入！", day);
						MessageBox(hDlg, errInfo, L"错误", MB_OK);
						break;
					}

					if (game.inputData(place, date, data)) {
						clearIndex.push_back(i);
					}
					else {
						errInfo.Format(L"抱歉，录入%d号的数据时发生了错误！\n请重试或报告错误(l_zhaohui@163.com)", day);
						MessageBox(hDlg, errInfo, L"错误", MB_OK);
					}
				}
			}

			delete[] wc;

			if (clearIndex.size() > 0) {
				game.inputPlace(place);

				errInfo.Format(L"成功录入%d天的数据！", clearIndex.size());
				MessageBox(hDlg, errInfo, L"恭喜", MB_OK);
			}

			for (int i = 0; i < clearIndex.size(); i++) {
				SetDlgItemText(hDlg, IDC_DATE1 + clearIndex[i], L"");
				SetDlgItemText(hDlg, IDC_DATA1 + clearIndex[i], L"");
			}

#else
			wmemset(wc, 0, LENGTH);
			GetDlgItemText(hDlg, IDC_TIDE_DATA, wc, LENGTH);

			CStringW tideData(wc);
			delete[] wc;

			if (tideData.IsEmpty() || !game.formatInputData(tideData) || !game.checkInputData(tideData)) {
				MessageBox(hDlg, L"输入时间与潮高数据不符合格式，检查后请重新输入！", L"错误", MB_OK);
				break;
			}

			if (game.inputData(place, date, tideData)) {

				//EndDialog(hDlg, LOWORD(wParam));
				CStringW info;
				info.Format(L"%s，%s年%s月%s日的数据录入成功！", place, date.Left(4), date.Mid(4, 2), date.Mid(6, 2));
				MessageBox(hDlg, info, L"恭喜", MB_OK);

				//SetDlgItemText(hDlg, IDC_INPUT_DATE, L"");
				SetDlgItemText(hDlg, IDC_TIDE_DATA, L"");
			}
			else {
				MessageBox(
					hDlg,
					L"抱歉，数据录入的时候发生了错误！\n请重试或报告错误(l_zhaohui@163.com)",
					L"错误",
					MB_OK);
			}
#endif
		}
		else if (LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return(INT_PTR) TRUE;
		}
		break;
	}

	return(INT_PTR) FALSE;
}

// Message handler for input data ex box.
INT_PTR CALLBACK InputCallbackEx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG: {
			HWND parent = ::GetParent(hDlg);
			RECT rcParent = {0,0,0,0};
			RECT rcDlg = {0,0,0,0};
			::GetWindowRect(parent, &rcParent);
			::GetWindowRect(hDlg, &rcDlg);

			int x = int(rcParent.left + ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
			int y = int(rcParent.top + ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2);

			::SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

			HWND hPwd = ::GetDlgItem(hDlg, IDC_DATA_FILE);
			if (hPwd) {
				::SetFocus(hPwd);
			}
		}

		return(INT_PTR) TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDE_INPUT_DATA_EX) {
		
			wchar_t*  wc = new wchar_t[MAX_PATH];
			wmemset(wc, 0, MAX_PATH);

			GetDlgItemText(hDlg, IDC_PLACE, wc, MAX_PATH);
			CStringW place(wc);
			if (place.IsEmpty()) {
				delete[] wc;

				MessageBox(hDlg, L"输入观察点不能为空，请重新输入！", L"错误", MB_OK);
				break;
			}

			wmemset(wc, 0, MAX_PATH);
			GetDlgItemText(hDlg, IDC_DATA_FILE, wc, MAX_PATH);

			CStringW filePath(wc);
			delete[] wc;
			wc = NULL;

			if (filePath.IsEmpty() || filePath == L"") {
				MessageBox(hDlg, L"请先导入数据文件！", L"错误", MB_OK);
			}
			else {
				CStringW resultInfo;
				bool isSuccess = game.inputDataEx(filePath, place, resultInfo);
				
				SetDlgItemText(hDlg, IDC_INFO, resultInfo);
				MessageBox(hDlg, resultInfo, isSuccess?L"恭喜":L"错误", MB_OK);
				
				if (isSuccess) {
					SetDlgItemText(hDlg, IDC_DATA_FILE, L"");
				}
			}

			return(INT_PTR) TRUE;
		}
		else if (LOWORD(wParam) == IDC_SEARCH_DATA_FILE) {

			// 打开导入文件对话框
			OPENFILENAME ofn;	// 公共对话框结构。
			TCHAR szFile[MAX_PATH]; // 保存获取文件名称的缓冲区。

			// 初始化选择文件对话框。
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFile = szFile;
			ofn.lpstrFile[0] = _T('\0');
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = _T("文本文件\0*.txt\0\0");
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			//ofn.lpTemplateName =  MAKEINTRESOURCE(ID_TEMP_DIALOG);
			// 显示打开选择文件对话框。
			if (GetOpenFileName(&ofn)) {
				CStringW filePath(szFile);

				CStringW place, date, resultInfo;
				bool isSuccess = game.checkInputDataEx(filePath, place, date, resultInfo);
				
				SetDlgItemText(hDlg, IDC_INFO, resultInfo);
				if (isSuccess) {
					SetDlgItemText(hDlg, IDC_DATA_FILE, szFile);
				}
				else {
					SetDlgItemText(hDlg, IDC_DATA_FILE, L"");
					MessageBox(hDlg, resultInfo, L"错误", MB_OK);
				}
			}
		}
		else if (LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return(INT_PTR) TRUE;
		}
		break;
	}

	return(INT_PTR) FALSE;
}


// Message handler for computer deepth box.
INT_PTR CALLBACK ComputeDeepthCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG: {
			HWND parent = ::GetParent(hDlg);
			RECT rcParent = {0,0,0,0};
			RECT rcDlg = {0,0,0,0};
			::GetWindowRect(parent, &rcParent);
			::GetWindowRect(hDlg, &rcDlg);

			int x = int(rcParent.left + ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
			int y = int(rcParent.top + ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2);

			::SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			
			// 设定初值
			CStringW place;
			int boat = 0;
			int water = 0;
			int sea = 0;

			game.getRichness(true, place, boat, water, sea);
			SetDlgItemText(hDlg, IDC_PLACE_ONE, place);
			SetDlgItemInt(hDlg, IDC_BOAT_ONE, boat, true);
			SetDlgItemInt(hDlg, IDC_WATER_ONE, water, true);
			SetDlgItemInt(hDlg, IDC_SEA_ONE, sea, true);

			game.getRichness(false, place, boat, water, sea);
			SetDlgItemText(hDlg, IDC_PLACE_TWO, place);
			SetDlgItemInt(hDlg, IDC_BOAT_TWO, boat, true);
			SetDlgItemInt(hDlg, IDC_WATER_TWO, water, true);
			SetDlgItemInt(hDlg, IDC_SEA_TWO, sea, true);

			// 更新结果
			updateResultOne(hDlg, false);
			updateResultTwo(hDlg, false);
		}

		return(INT_PTR) TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return(INT_PTR) TRUE;
		}
		else if (LOWORD(wParam) == IDC_COMPUTE_ONE)
		{
			updateResultOne(hDlg);
		}
		else if (LOWORD(wParam) == IDC_COMPUTE_TWO)
		{
			updateResultTwo(hDlg);
		}

		else if(LOWORD(wParam) == IDOK )
		{
			updateResultOne(hDlg);
			updateResultTwo(hDlg);
		}
		break;
	}

	return(INT_PTR) FALSE;
}

void updateResultOne(HWND hDlg, bool needSave)
{
	wchar_t*  wc = new wchar_t[MAX_PATH];
	wmemset(wc, 0, MAX_PATH);

	const int UNKOWN = -10000;
	int boat = UNKOWN;
	int water = UNKOWN;
	int sea = UNKOWN;
	int result = 0;
	CStringW strResult(L"");
	CStringW place(L"");

	GetDlgItemText(hDlg, IDC_PLACE_ONE, wc, MAX_PATH);
	place.Format(L"%s", wc);
	wmemset(wc, 0, MAX_PATH);

	GetDlgItemText(hDlg, IDC_BOAT_ONE, wc, MAX_PATH);
	if(wcslen(wc) > 0)
	{
		boat = _wtoi(wc);
		wmemset(wc, 0, MAX_PATH);
	}

	GetDlgItemText(hDlg, IDC_WATER_ONE, wc, MAX_PATH);
	if(wcslen(wc) > 0)
	{
		water = _wtoi(wc);
		wmemset(wc, 0, MAX_PATH);
	}

	GetDlgItemText(hDlg, IDC_SEA_ONE, wc, MAX_PATH);
	if(wcslen(wc) > 0)
	{
		sea = _wtoi(wc);
		wmemset(wc, 0, MAX_PATH);
	}

	delete[] wc;
	wc = NULL;

	if (boat != UNKOWN && sea != UNKOWN && water != UNKOWN)
	{
		result = int(boat * water / 100) - sea;
		strResult.Format(L"%d", result);

		if (needSave)
		{
			game.setRichness(true, place, boat, water, sea);
		}
	}

	SetDlgItemText(hDlg, IDC_RESULT_ONE, strResult);
}

void updateResultTwo(HWND hDlg, bool needSave)
{
	wchar_t*  wc = new wchar_t[MAX_PATH];
	wmemset(wc, 0, MAX_PATH);

	const int UNKOWN = -10000;
	int boat = UNKOWN;
	int water = UNKOWN;
	int sea = UNKOWN;
	int result = 0;
	CStringW strResult(L"");
	CStringW place(L"");

	GetDlgItemText(hDlg, IDC_PLACE_TWO, wc, MAX_PATH);
	place.Format(L"%s", wc);
	wmemset(wc, 0, MAX_PATH);

	GetDlgItemText(hDlg, IDC_BOAT_TWO, wc, MAX_PATH);
	if(wcslen(wc) > 0)
	{
		boat = _wtoi(wc);
		wmemset(wc, 0, MAX_PATH);
	}

	GetDlgItemText(hDlg, IDC_WATER_TWO, wc, MAX_PATH);
	if(wcslen(wc) > 0)
	{
		water = _wtoi(wc);
		wmemset(wc, 0, MAX_PATH);
	}

	GetDlgItemText(hDlg, IDC_SEA_TWO, wc, MAX_PATH);
	if(wcslen(wc) > 0)
	{
		sea = _wtoi(wc);
		wmemset(wc, 0, MAX_PATH);
	}

	delete[] wc;
	wc = NULL;

	if (boat != UNKOWN && sea != UNKOWN && water != UNKOWN)
	{
		result =  boat + water - sea;
		strResult.Format(L"%d", result);

		if (needSave)
		{
			game.setRichness(false, place, boat, water, sea);
		}
	}

	SetDlgItemText(hDlg, IDC_RESULT_TWO, strResult);
}

// Message handler for about box.
INT_PTR CALLBACK AboutCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG: {
			HWND parent = ::GetParent(hDlg);
			RECT rcParent = {0,0,0,0};
			RECT rcDlg = {0,0,0,0};
			::GetWindowRect(parent, &rcParent);
			::GetWindowRect(hDlg, &rcDlg);

			int x = int(rcParent.left + ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
			int y = int(rcParent.top + ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2);

			::SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}

		return(INT_PTR) TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return(INT_PTR) TRUE;
		}
		break;
	}

	return(INT_PTR) FALSE;
}

// Message handler for modify password box.
INT_PTR CALLBACK ModifyCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG: {
			HWND parent = ::GetParent(hDlg);
			RECT rcParent = {0,0,0,0};
			RECT rcDlg = {0,0,0,0};
			::GetWindowRect(parent, &rcParent);
			::GetWindowRect(hDlg, &rcDlg);

			int x = int(rcParent.left + ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
			int y = int(rcParent.top + ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2);

			::SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

			HWND hPwd = ::GetDlgItem(hDlg, IDC_PWD);
			if (hPwd) {
				::SetFocus(hPwd);
			}
		}

		return(INT_PTR) TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK) {
			wchar_t*  wc = new wchar_t[40];
			wmemset(wc, 0, 40);
			GetDlgItemText(hDlg, IDC_OLD_PWD, wc, 40);

			CStringW oldPwd(wc);

			wmemset(wc, 0, 40);
			GetDlgItemText(hDlg, IDC_NEW_PWD, wc, 40);

			CStringW newPwd(wc);

			delete[] wc;
			wc = NULL;

			if (!game.checkPassword(oldPwd)) {
				MessageBox(hDlg, L"原密码不正确，请重新输入！", L"错误", MB_OK);
				SetDlgItemText(hDlg, IDC_OLD_PWD, L"");
			}
			else if (newPwd.IsEmpty()) {
				MessageBox(hDlg, L"新密码不能为空，请输入新密码！", L"错误", MB_OK);
				SetDlgItemText(hDlg, IDC_NEW_PWD, L"");
			}
			else if (newPwd.GetLength() > 24) {
				MessageBox(hDlg, L"新密码长度不能超过12，请重新输入！", L"错误", MB_OK);
				SetDlgItemText(hDlg, IDC_NEW_PWD, L"");
			}
			else {
				EndDialog(hDlg, LOWORD(wParam));

				game.modifyPassword(newPwd);

				MessageBox(hDlg, L"新密码设置成功，下回请用新密码！", L"恭喜", MB_OK);
			}

			return(INT_PTR) TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return(INT_PTR) TRUE;
		}
		break;
	}

	return(INT_PTR) FALSE;
}

// Message handler for look box.
INT_PTR CALLBACK LookCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG: {
			HWND parent = ::GetParent(hDlg);
			RECT rcParent = {0,0,0,0};
			RECT rcDlg = {0,0,0,0};
			::GetWindowRect(parent, &rcParent);
			::GetWindowRect(hDlg, &rcDlg);

			int x = int(rcParent.left + ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
			int y = int(rcParent.top + ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2);

			::SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

			HWND hPwd = ::GetDlgItem(hDlg, IDC_PWD);
			if (hPwd) {
				::SetFocus(hPwd);
			}
		}

		return(INT_PTR) TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK) {
			wchar_t*  wc = new wchar_t[40];
			wmemset(wc, 0, 40);

			GetDlgItemText(hDlg, IDC_DATE, wc, 40);

			CStringW date(wc);
			delete[] wc;
			wc = NULL;

			if (game.checkDate(date)) {
				EndDialog(hDlg, LOWORD(wParam));

				game.loadDay(date);
			}
			else {
				MessageBox(hDlg, L"输入日期格式不正确，请重新输入！", L"错误", MB_OK);
				SetDlgItemText(hDlg, IDC_DATE, L"");
			}

			return(INT_PTR) TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return(INT_PTR) TRUE;
		}
		break;
	}

	return(INT_PTR) FALSE;
}

// Message handler for login box.
INT_PTR CALLBACK LoginCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG: {
			HWND parent = ::GetParent(hDlg);
			RECT rcParent = {0,0,0,0};
			RECT rcDlg = {0,0,0,0};
			::GetWindowRect(parent, &rcParent);
			::GetWindowRect(hDlg, &rcDlg);

			int x = int(rcParent.left + ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
			int y = int(rcParent.top + ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2);

			::SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

			HWND hPwd = ::GetDlgItem(hDlg, IDC_PWD);
			if (hPwd) {
				::SetFocus(hPwd);
			}
		}

		return(INT_PTR) TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK) {
			wchar_t*  wc = new wchar_t[40];
			wmemset(wc, 0, 40);

			GetDlgItemText(hDlg, IDC_PWD, wc, 40);

			CStringW pwd(wc);
			delete[] wc;
			wc = NULL;

			if (game.checkPassword(pwd)) {
				EndDialog(hDlg, LOWORD(wParam));

				game.loadToday();
			}
			else {
				MessageBox(hDlg, L"密码不正确，请重新输入！", L"错误", MB_OK);
				SetDlgItemText(hDlg, IDC_PWD, L"");
			}

			return(INT_PTR) TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return(INT_PTR) TRUE;
		}
		break;
	}

	return(INT_PTR) FALSE;
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	hInst = hinstance;

	HWND hwnd = WinUtility::InitWindow(hinstance, _T("电子潮汐图表"), 820, 650, true);	//, 600, true);
	if (!hwnd) {
		::MessageBox(0, _T("Init game window - FAILED"), 0, 0);
		return 0;
	}

	if (!Setup(hwnd)) {
		::MessageBox(0, _T("Setup() - FAILED"), 0, 0);
		return 0;
	}

	WinUtility::EnterMsgLoop(Display);

	Cleanup();

	return 0;
}
