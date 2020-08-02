				 /*
 *   Copyright (c) 2008, ƮƮ����(kesalin@gmail.com)
 *   All rights reserved.
 *
 *   �ļ����ƣ�GameMain.cpp
 *   ժ    Ҫ��������ļ�
 *
 *   ��ǰ�汾��1.1
 *   ��    �ߣ�ƮƮ����
 *   ������ڣ�2008/11/30
 */
#include "WinUtility.h"
#include "Game.h"
#include "CLog.h"
#include <map>
#include <vector>

//
// Globals
//
HINSTANCE hInst;  // current instance
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
INT_PTR CALLBACK ComputeEatWaterCallback(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ComputeUpDraftCallback(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ComputeUpDWTDraftCallback(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ComputeDownDraftCallback(HWND, UINT, WPARAM, LPARAM);

void updateResultOne(HWND hDlg, bool needSave = true);
void updateResultTwo(HWND hDlg, bool needSave = true);

void updateWaterHeightResultOne(HWND hDlg, bool needSave = true);
void updateWaterHeightResultTwo(HWND hDlg, bool needSave = true);

//
// Framework Functions
//
bool Setup(HWND hwnd)
{
    // Init GDI++
    Gdiplus::GdiplusStartupInput input;
    Gdiplus::GdiplusStartupOutput output = {0, 0};
    Gdiplus::Status ret = Gdiplus::GdiplusStartup(&token_, &input, &output);
    if (ret != Gdiplus::Ok)
    {
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
    switch (msg)
    {
    case WM_DESTROY:
        // �رմ���
        ::PostQuitMessage(WM_QUIT);
        break;
    case WM_INITMENUPOPUP:
        // ϵͳ�˵�
        if (lParam == 0)
        {
            // �ѵ�¼
            if (game.isLogined())
            {
                EnableMenuItem((HMENU)wParam, ID_LOGIN, MF_GRAYED);
                EnableMenuItem((HMENU)wParam, ID_MODIFY_PWD, MF_ENABLED);
                EnableMenuItem((HMENU)wParam, ID_INPUT_DATA, MF_ENABLED);
            }

            // δ��¼
            else
            {
                EnableMenuItem((HMENU)wParam, ID_MODIFY_PWD, MF_GRAYED);
                EnableMenuItem((HMENU)wParam, ID_INPUT_DATA, MF_GRAYED);
            }
        }

        // �鿴�˵�
        else if (lParam == 1)
        {
            // δ��¼�����²˵���ֹ����
            if (!game.isLogined())
            {
                EnableMenuItem((HMENU)wParam, ID_TODAY, MF_GRAYED);
                EnableMenuItem((HMENU)wParam, ID_PREV, MF_GRAYED);
                EnableMenuItem((HMENU)wParam, ID_NEXT, MF_GRAYED);
                EnableMenuItem((HMENU)wParam, ID_LOOK, MF_GRAYED);
            }
            else
            {
                // �ѵ�¼
                EnableMenuItem((HMENU)wParam, ID_TODAY, MF_ENABLED);
                EnableMenuItem((HMENU)wParam, ID_PREV, MF_ENABLED);
                EnableMenuItem((HMENU)wParam, ID_NEXT, MF_ENABLED);
                EnableMenuItem((HMENU)wParam, ID_LOOK, MF_ENABLED);
            }
        }
        break;
    case WM_COMMAND:
        if (wParam == ID_START)
        {
            // �˳��˵�
            game.exit();
        }
        else if (wParam == ID_LOGIN)
        {
            // ��¼�˵�
            DialogBoxW(hInst, MAKEINTRESOURCE(IDD_LOGIN), hwnd, LoginCallback);
        }
        else if (wParam == ID_LOOK)
        {
            //�鿴�˵�
            DialogBoxW(hInst, MAKEINTRESOURCE(IDD_LOOK), hwnd, LookCallback);
        }
        else if (wParam == ID_MODIFY_PWD)
        {
            // �޸�����˵�
            DialogBoxW(hInst, MAKEINTRESOURCE(IDD_MODIFY), hwnd, ModifyCallback);
        }
        else if (wParam == ID_ABOUT)
        {
            // ���ڲ˵�
            DialogBoxW(hInst, MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutCallback);
        }
        else if (wParam == ID_HELP)
        {
            // �����˵�
            /*
                        game.fileId = Game::FILE_HELP;
                        DialogBoxW(hInst, MAKEINTRESOURCE(IDD_HELP), hwnd, HelpCallback);
            */
            wchar_t fullPath[MAX_PATH] = {
                0,
            };
            GetModuleFileNameW(0, fullPath, MAX_PATH);
            PathRemoveFileSpecW(fullPath);
            PathAppend(fullPath, L"����С����.chm");
            // �򿪺���С����.chm
            ShellExecute(NULL, _T("open"), _T("hh.exe"), fullPath, NULL, SW_SHOWNORMAL);
        }
        else if (wParam == IDM_BOAT_INFO)
        {
            // ��������˵�
            game.fileId = Game::FILE_TUOLUN;
            DialogBoxW(hInst, MAKEINTRESOURCE(IDD_HELP), hwnd, HelpCallback);
        }
        else if (wParam == IDM_MATOU_INFO)
        {
            // ��ͷ����˵�
            game.fileId = Game::FILE_MATOU;
            DialogBoxW(hInst, MAKEINTRESOURCE(IDD_HELP), hwnd, HelpCallback);
        }
        else if (wParam == IDM_HANGDAO_INFO)
        {
            // ��������˵�
            game.fileId = Game::FILE_HANGDAO;
            DialogBoxW(hInst, MAKEINTRESOURCE(IDD_HELP), hwnd, HelpCallback);
        }
        else if (wParam == ID_INPUT_DATA)
        {
            // ����¼��˵�
            // DialogBoxW(hInst, MAKEINTRESOURCE(IDD_INPUT_DATA), hwnd, InputCallback);
            DialogBoxW(hInst, MAKEINTRESOURCE(IDD_INPUT_DATA_EX), hwnd, InputCallbackEx);
        }
        else if (wParam == ID_TODAY)
        {
            // �鿴����
            game.loadToday();
        }
        else if (wParam == ID_PREV)
        {
            // �鿴ǰһ��
            game.loadPrev();
        }
        else if (wParam == ID_NEXT)
        {
            // �鿴��һ��
            game.loadNext();
        }
        else if (wParam == ID_CALCU)
        {
            // ����������
            ShellExecute(NULL, _T("open"), _T("calc.exe"), NULL, NULL, SW_SHOWNORMAL);
        }

        else if (wParam == ID_COMPUTER_DEEPTH)
        {
            // ���㸻ԣˮ��
            DialogBoxW(hInst, MAKEINTRESOURCE(IDD_COMPUTE), hwnd, ComputeDeepthCallback);
        }

        else if (wParam == ID_COMPUTER_EAT_WATER)
        {
            // ��������ˮ
            DialogBoxW(hInst, MAKEINTRESOURCE(IDD_COMPUTER_EAT_WATER), hwnd, ComputeEatWaterCallback);
        }

        else if (wParam == ID_UP_DRAFT)
        {
            // �������У����۴�������ˮ
            DialogBoxW(hInst, MAKEINTRESOURCE(IDD_UP_DRAFT), hwnd, ComputeUpDraftCallback);
        }

        else if (wParam == ID_UP_DWT_DRAFT)
        {
            // �������У�����DWT����7.5��ִ�������ˮ
            DialogBoxW(hInst, MAKEINTRESOURCE(IDD_UP_DWT_DRAFT), hwnd, ComputeUpDWTDraftCallback);
        }

        else if (wParam == ID_DOWN_DRAFT)
        {
            // �������У����۴�������ˮ
            DialogBoxW(hInst, MAKEINTRESOURCE(IDD_DOWN_DRAFT), hwnd, ComputeDownDraftCallback);
        }

        else if (LOWORD(wParam) >= MENU_ID_START && LOWORD(wParam) < MENU_ID_START + game.getMenuMax())
        {
            // ��Ӧ�Ҽ��˵�
            game.processMenu(LOWORD(wParam) - MENU_ID_START);
        }
        break;

    // ���������Ϣ
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            // ��ESC���ر�Ӧ�ó���
            ::PostQuitMessage(WM_QUIT);
        }
        else
        {
            Point point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            game.onKeyDown((int)wParam, point);
        }
        break;

    // ��������ƶ���Ϣ
    case WM_MOUSEMOVE:
    {
        Point point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        game.onMouseMove(point);
    }
    break;

    // ������������Ϣ
    case WM_LBUTTONDOWN:
    {
        Point point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        game.onLeftButtonDown(point);
    }
    break;

    // ��������Ҽ�����Ϣ
    case WM_RBUTTONDOWN:
    {
        Point point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        game.onRightButtonDown(point);
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);  // validate the window
        EndPaint(hwnd, &ps);

        return (0);  // return success
    }
    }

    return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

// Message handler for help box.
INT_PTR CALLBACK HelpCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        HWND parent = ::GetParent(hDlg);
        RECT rcParent = {0, 0, 0, 0};
        RECT rcDlg = {0, 0, 0, 0};
        ::GetWindowRect(parent, &rcParent);
        ::GetWindowRect(hDlg, &rcDlg);

        int x = int(rcParent.left + ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
        int y = int(rcParent.top + ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2);

        ::SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        HWND hPwd = ::GetDlgItem(hDlg, IDC_HELP_TXT);
        if (hPwd)
        {
#define MAX_SIZE (1024 * 30)
            wchar_t fullPath[MAX_PATH] = {
                0,
            };
            GetModuleFileNameW(0, fullPath, MAX_PATH);
            PathRemoveFileSpecW(fullPath);

            switch (game.fileId)
            {
            case Game::FILE_HELP:
                ::SetWindowText(hDlg, L"����");
                PathAppend(fullPath, L"help.txt");
                break;
            case Game::FILE_MATOU:
                ::SetWindowText(hDlg, L"��ͷ���");
                PathAppend(fullPath, L"��ͷ���.txt");
                break;
            case Game::FILE_TUOLUN:
                ::SetWindowText(hDlg, L"�������");
                PathAppend(fullPath, L"�������.txt");
                break;
            case Game::FILE_HANGDAO:
                ::SetWindowText(hDlg, L"�������");
                PathAppend(fullPath, L"�������.txt");
                break;
            default:
                return (INT_PTR)TRUE;
            }

            CStringW strMode;
            strMode.Format(L"r, ccs=UTF-8");
            _tsetlocale(LC_ALL, _T("chinese-simplified"));

            FILE* fp = NULL;
            errno_t err = _wfopen_s(&fp, fullPath, strMode);
            if (!err && fp != NULL)
            {
                wchar_t buf[MAX_SIZE] = {
                    0,
                };
                fread(buf, sizeof(wchar_t), MAX_SIZE, fp);
                fgetws(buf, MAX_SIZE, fp);

                for (int i = 0; i < MAX_SIZE && buf[i] != 0; i++)
                {
                    if ((i + 1) < MAX_SIZE && buf[i] == 0x0a && buf[i + 1] == 0x0a)
                    {
                        buf[i] = 0x0d;
                        i++;
                    }
                }

                ::SetWindowText(hPwd, buf);
                fclose(fp);
            }
        }
    }

        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }

    return (INT_PTR)FALSE;
}

// Message handler for input box.
INT_PTR CALLBACK InputCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        HWND parent = ::GetParent(hDlg);
        RECT rcParent = {0, 0, 0, 0};
        RECT rcDlg = {0, 0, 0, 0};
        ::GetWindowRect(parent, &rcParent);
        ::GetWindowRect(hDlg, &rcDlg);

        int x = int(rcParent.left + ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
        int y = int(rcParent.top + ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2);

        ::SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }

        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            const static int LENGTH = 512;
            wchar_t* wc = new wchar_t[LENGTH];
            wmemset(wc, 0, LENGTH);
            GetDlgItemText(hDlg, IDC_PLACE, wc, LENGTH);

            CStringW place(wc);

            if (place.IsEmpty())
            {
                delete[] wc;

                MessageBox(hDlg, L"����۲�㲻��Ϊ�գ����������룡", L"����", MB_OK);
                break;
            }

            wmemset(wc, 0, LENGTH);
            GetDlgItemText(hDlg, IDC_INPUT_DATE, wc, LENGTH);

            CStringW month(wc);

            CStringW errInfo;
            CStringW date;
            if (!month.IsEmpty())
            {
                date.Format(L"%s01", month);
            }

            if (month.IsEmpty() || !game.checkDate(date))
            {
                delete[] wc;

                MessageBox(hDlg, L"����������·ݸ�ʽ����ȷ�����������룡", L"����", MB_OK);
                SetDlgItemText(hDlg, IDC_INPUT_DATE, L"");
                break;
            }

#if 1
            // Check and input data.
            std::vector<int> clearIndex;
            CStringW day;
            CStringW data;
            for (int i = 0; i < 6; i++)
            {
                day.Empty();
                data.Empty();

                int dateDlgId = IDC_DATE1 + i;
                int dataDlgId = IDC_DATA1 + i;

                // get date.
                wmemset(wc, 0, LENGTH);
                GetDlgItemText(hDlg, dateDlgId, wc, LENGTH);
                day.Format(L"%s", wc);
                if (day.GetLength() == 1)
                {
                    // day.Format(L"0%s", day);
                    day = L"0" + day;
                }

                // check date.
                if (!day.IsEmpty())
                {
                    date.Format(L"%s%s", month, day);

                    if (!game.checkDate(date))
                    {
                        errInfo.Format(L"�������ӣ�%s�����Ϸ������������룡", day);
                        MessageBox(hDlg, errInfo, L"����", MB_OK);
                        SetDlgItemText(hDlg, dateDlgId, L"");
                        break;
                    }

                    // get tide data.
                    wmemset(wc, 0, LENGTH);
                    GetDlgItemText(hDlg, dataDlgId, wc, LENGTH);
                    data.Format(L"%s", wc);

                    if (data.IsEmpty() || !game.formatInputData(data) || !game.checkInputData(data))
                    {
                        errInfo.Format(L"����%s�ŵĳ������ݲ����ϸ�ʽ���������������룡", day);
                        MessageBox(hDlg, errInfo, L"����", MB_OK);
                        break;
                    }

                    if (game.inputData(place, date, data))
                    {
                        clearIndex.push_back(i);
                    }
                    else
                    {
                        errInfo.Format(L"��Ǹ��¼��%d�ŵ�����ʱ�����˴���\n�����Ի򱨸����(l_zhaohui@163.com)",
                                       day);
                        MessageBox(hDlg, errInfo, L"����", MB_OK);
                    }
                }
            }

            delete[] wc;

            if (clearIndex.size() > 0)
            {
                game.inputPlace(place);

                errInfo.Format(L"�ɹ�¼��%d������ݣ�", clearIndex.size());
                MessageBox(hDlg, errInfo, L"��ϲ", MB_OK);
            }

            for (size_t i = 0; i < clearIndex.size(); i++)
            {
                SetDlgItemText(hDlg, IDC_DATE1 + clearIndex[i], L"");
                SetDlgItemText(hDlg, IDC_DATA1 + clearIndex[i], L"");
            }

#else
            wmemset(wc, 0, LENGTH);
            GetDlgItemText(hDlg, IDC_TIDE_DATA, wc, LENGTH);
            CStringW tideData(wc);
            delete[] wc;
            if (tideData.IsEmpty() || !game.formatInputData(tideData) || !game.checkInputData(tideData))
            {
                MessageBox(hDlg, L"����ʱ���볱�����ݲ����ϸ�ʽ���������������룡", L"����", MB_OK);
                break;
            }
            if (game.inputData(place, date, tideData))
            {
                // EndDialog(hDlg, LOWORD(wParam));
                CStringW info;
                info.Format(L"%s��%s��%s��%s�յ�����¼��ɹ���", place, date.Left(4),
                            date.Mid(4, 2), date.Mid(6, 2));
                MessageBox(hDlg, info, L"��ϲ", MB_OK);
                // SetDlgItemText(hDlg, IDC_INPUT_DATE, L"");
                SetDlgItemText(hDlg, IDC_TIDE_DATA, L"");
            }
            else
            {
                MessageBox(hDlg, L"��Ǹ������¼���ʱ�����˴���\n�����Ի򱨸����(l_zhaohui@163.com)",
                           L"����", MB_OK);
            }
#endif
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }

    return (INT_PTR)FALSE;
}

// Message handler for input data ex box.
INT_PTR CALLBACK InputCallbackEx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        HWND parent = ::GetParent(hDlg);
        RECT rcParent = {0, 0, 0, 0};
        RECT rcDlg = {0, 0, 0, 0};
        ::GetWindowRect(parent, &rcParent);
        ::GetWindowRect(hDlg, &rcDlg);

        int x = int(rcParent.left + ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
        int y = int(rcParent.top + ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2);

        ::SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        HWND hPwd = ::GetDlgItem(hDlg, IDC_DATA_FILE);
        if (hPwd)
        {
            ::SetFocus(hPwd);
        }
    }

        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDE_INPUT_DATA_EX)
        {
            wchar_t* wc = new wchar_t[MAX_PATH];
            wmemset(wc, 0, MAX_PATH);

            GetDlgItemText(hDlg, IDC_PLACE, wc, MAX_PATH);
            CStringW place(wc);
            if (place.IsEmpty())
            {
                delete[] wc;

                MessageBox(hDlg, L"����۲�㲻��Ϊ�գ����������룡", L"����", MB_OK);
                break;
            }

            wmemset(wc, 0, MAX_PATH);
            GetDlgItemText(hDlg, IDC_DATA_FILE, wc, MAX_PATH);

            CStringW filePath(wc);
            delete[] wc;
            wc = NULL;

            if (filePath.IsEmpty() || filePath == L"")
            {
                MessageBox(hDlg, L"���ȵ��������ļ���", L"����", MB_OK);
            }
            else
            {
                CStringW resultInfo;
                bool isSuccess = game.inputDataEx(filePath, place, resultInfo);

                SetDlgItemText(hDlg, IDC_INFO, resultInfo);
                MessageBox(hDlg, resultInfo, isSuccess ? L"��ϲ" : L"����", MB_OK);

                if (isSuccess)
                {
                    SetDlgItemText(hDlg, IDC_DATA_FILE, L"");
                }
            }

            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDC_SEARCH_DATA_FILE)
        {
            // �򿪵����ļ��Ի���
            OPENFILENAME ofn;        // �����Ի���ṹ��
            TCHAR szFile[MAX_PATH];  // �����ȡ�ļ����ƵĻ�������

            // ��ʼ��ѡ���ļ��Ի���
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hDlg;
            ofn.lpstrFile = szFile;
            ofn.lpstrFile[0] = _T('\0');
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = _T("�ı��ļ�\0*.txt\0\0");
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            // ofn.lpTemplateName =  MAKEINTRESOURCE(ID_TEMP_DIALOG);
            // ��ʾ��ѡ���ļ��Ի���
            if (GetOpenFileName(&ofn))
            {
                CStringW filePath(szFile);

                CStringW place, date, resultInfo;
                bool isSuccess = game.checkInputDataEx(filePath, place, date, resultInfo);

                SetDlgItemText(hDlg, IDC_INFO, resultInfo);
                if (isSuccess)
                {
                    SetDlgItemText(hDlg, IDC_DATA_FILE, szFile);
                }
                else
                {
                    SetDlgItemText(hDlg, IDC_DATA_FILE, L"");
                    MessageBox(hDlg, resultInfo, L"����", MB_OK);
                }
            }
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }

    return (INT_PTR)FALSE;
}

// Message handler for computer deepth box.
INT_PTR CALLBACK ComputeDeepthCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        HWND parent = ::GetParent(hDlg);
        RECT rcParent = {0, 0, 0, 0};
        RECT rcDlg = {0, 0, 0, 0};
        ::GetWindowRect(parent, &rcParent);
        ::GetWindowRect(hDlg, &rcDlg);

        int x = int(rcParent.left + ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
        int y = int(rcParent.top + ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2);

        ::SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        // �趨��ֵ
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

        // ���½��
        updateResultOne(hDlg, false);
        updateResultTwo(hDlg, false);
    }

        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDC_COMPUTE_ONE)
        {
            updateResultOne(hDlg);
        }
        else if (LOWORD(wParam) == IDC_COMPUTE_TWO)
        {
            updateResultTwo(hDlg);
        }

        else if (LOWORD(wParam) == IDOK)
        {
            updateResultOne(hDlg);
            updateResultTwo(hDlg);
        }
        break;
    }

    return (INT_PTR)FALSE;
}

void updateResultOne(HWND hDlg, bool needSave)
{
    wchar_t* wc = new wchar_t[MAX_PATH];
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
    if (wcslen(wc) > 0)
    {
        boat = _wtoi(wc);
        wmemset(wc, 0, MAX_PATH);
    }

    GetDlgItemText(hDlg, IDC_WATER_ONE, wc, MAX_PATH);
    if (wcslen(wc) > 0)
    {
        water = _wtoi(wc);
        wmemset(wc, 0, MAX_PATH);
    }

    GetDlgItemText(hDlg, IDC_SEA_ONE, wc, MAX_PATH);
    if (wcslen(wc) > 0)
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
    wchar_t* wc = new wchar_t[MAX_PATH];
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
    if (wcslen(wc) > 0)
    {
        boat = _wtoi(wc);
        wmemset(wc, 0, MAX_PATH);
    }

    GetDlgItemText(hDlg, IDC_WATER_TWO, wc, MAX_PATH);
    if (wcslen(wc) > 0)
    {
        water = _wtoi(wc);
        wmemset(wc, 0, MAX_PATH);
    }

    GetDlgItemText(hDlg, IDC_SEA_TWO, wc, MAX_PATH);
    if (wcslen(wc) > 0)
    {
        sea = _wtoi(wc);
        wmemset(wc, 0, MAX_PATH);
    }

    delete[] wc;
    wc = NULL;

    if (boat != UNKOWN && sea != UNKOWN && water != UNKOWN)
    {
        result = boat + water - sea;
        strResult.Format(L"%d", result);

        if (needSave)
        {
            game.setRichness(false, place, boat, water, sea);
        }
    }

    SetDlgItemText(hDlg, IDC_RESULT_TWO, strResult);
}

void updateDraftResult(HWND hDlg, int index, bool needSave, bool isRatioOne)
{
    assert(index >= 1 && index <= 4);
	
    std::map<int, std::vector<int>> ids_map;

	std::vector<int> v;
	v.push_back(IDC_UP_MAX_TIDE_HEIGHT_ONE);
	v.push_back(IDC_UP_SEA_CHART_DEPTH_ONE);
	v.push_back(IDC_UP_WATER_RATIO_ONE);
	v.push_back(IDC_UP_RESULT_ONE);
	ids_map[1] = v;

	v.clear();	 
	v.push_back(IDC_UP_MAX_TIDE_HEIGHT_TWO);
	v.push_back(IDC_UP_SEA_CHART_DEPTH_TWO);
	v.push_back(IDC_UP_WATER_RATIO_TWO);
	v.push_back(IDC_UP_RESULT_TWO);
	ids_map[2] = v;

	v.clear();	 
	v.push_back(IDC_UP_MAX_TIDE_HEIGHT_THREE);
	v.push_back(IDC_UP_SEA_CHART_DEPTH_THREE);
	v.push_back(IDC_UP_WATER_RATIO_THREE);
	v.push_back(IDC_UP_RESULT_THREE);
	ids_map[3] = v;
	
	v.clear();	 
	v.push_back(IDC_UP_MAX_TIDE_HEIGHT_FOUR);
	v.push_back(IDC_UP_SEA_CHART_DEPTH_FOUR);
	v.push_back(IDC_UP_WATER_RATIO_FOUR);
	v.push_back(IDC_UP_RESULT_FOUR);
	ids_map[4] = v;

    const auto& ids = ids_map[index];

    wchar_t* wc = new wchar_t[MAX_PATH];
    wmemset(wc, 0, MAX_PATH);

    const int UNKOWN = -10000;
    int available = UNKOWN;
    int seaWater = UNKOWN;
    float ratio = UNKOWN;
    int result = 0;
    CStringW strResult(L"");

    GetDlgItemText(hDlg, ids[0], wc, MAX_PATH);
    if (wcslen(wc) > 0)
    {
        available = _wtoi(wc);
        wmemset(wc, 0, MAX_PATH);
    }

    GetDlgItemText(hDlg, ids[1], wc, MAX_PATH);
    if (wcslen(wc) > 0)
    {
        seaWater = _wtoi(wc);
        wmemset(wc, 0, MAX_PATH);
    }

    GetDlgItemText(hDlg, ids[2], wc, MAX_PATH);
    if (wcslen(wc) > 0)
    {
        ratio = _wtof(wc);
        wmemset(wc, 0, MAX_PATH);
    }

    delete[] wc;
    wc = NULL;

    if (available != UNKOWN && seaWater != UNKOWN && abs(ratio - UNKOWN) > 0.000001)
    {
        result = game.calculateBeiCaoDraft(ratio, available, seaWater);
        strResult.Format(L"%d", result);   
		SetDlgItemText(hDlg, ids[3], strResult);
    }
}


void updateDraftResultOne(HWND hDlg, bool needSave)
{
    updateDraftResult(hDlg, 1, needSave, true);
}

void updateDraftResultTwo(HWND hDlg, bool needSave)
{
    updateDraftResult(hDlg, 2, needSave, false);
}

void updateDraftResultThree(HWND hDlg, bool needSave)
{
    updateDraftResult(hDlg, 3, needSave, true);
}

void updateDraftResultFour(HWND hDlg, bool needSave)
{
    updateDraftResult(hDlg, 4, needSave, false);
}

void saveDraftParameters(HWND hDlg)
{
    wchar_t* wc = new wchar_t[MAX_PATH];
    wmemset(wc, 0, MAX_PATH);

    const int UNKOWN = -10000;
    int chartDepth = UNKOWN;
    float ratioOne = UNKOWN;	
    float ratioTwo = UNKOWN;

    GetDlgItemText(hDlg, IDC_SEA_CHART_DEPTH, wc, MAX_PATH);
    if (wcslen(wc) > 0)
    {
        chartDepth = _wtoi(wc);
        wmemset(wc, 0, MAX_PATH);

        SetDlgItemInt(hDlg, IDC_UP_SEA_CHART_DEPTH_ONE, chartDepth, true);
        SetDlgItemInt(hDlg, IDC_UP_SEA_CHART_DEPTH_TWO, chartDepth, true);
        SetDlgItemInt(hDlg, IDC_UP_SEA_CHART_DEPTH_TWO, chartDepth, true);
        SetDlgItemInt(hDlg, IDC_UP_SEA_CHART_DEPTH_THREE, chartDepth, true);
    }

    GetDlgItemText(hDlg, IDC_WATER_RATIO_ONE, wc, MAX_PATH);
    if (wcslen(wc) > 0)
    {
        ratioOne = _wtof(wc);
        wmemset(wc, 0, MAX_PATH);
		
		CStringW ratio;
        ratio.Format(L"%.2f", ratioOne);
        SetDlgItemText(hDlg, IDC_UP_WATER_RATIO_ONE, ratio);   
        SetDlgItemText(hDlg, IDC_UP_WATER_RATIO_THREE, ratio);
    }

    GetDlgItemText(hDlg, IDC_WATER_RATIO_TWO, wc, MAX_PATH);
    if (wcslen(wc) > 0)
    {
        ratioTwo = _wtof(wc);
        wmemset(wc, 0, MAX_PATH);
		
		CStringW ratio;
        ratio.Format(L"%.2f", ratioTwo);
        SetDlgItemText(hDlg, IDC_UP_WATER_RATIO_TWO, ratio);   
        SetDlgItemText(hDlg, IDC_UP_WATER_RATIO_FOUR, ratio);
    }

    delete[] wc;
    wc = NULL;

    if (chartDepth != UNKOWN && abs(ratioOne - UNKOWN) > 0.000001 && abs(ratioTwo - UNKOWN) > 0.000001)
    {
		game.setChartDepth(chartDepth);	
        game.setRatioOne(ratioOne);
        game.setRatioTwo(ratioTwo);

		// ���½��
		updateDraftResultOne(hDlg, false);
		updateDraftResultTwo(hDlg, false);
		updateDraftResultThree(hDlg, false);
		updateDraftResultFour(hDlg, false);
	}
}

void initDraftDialog(HWND hDlg, int draftOne, int draftTwo)
{
    auto& draftData = game.getBeiCaoDraftData();
    float ratioOne = game.getRatioOne();
    float ratioTwo = game.getRatioTwo();
    int chartDepth = game.getChartDepth();

    CStringW ratio;

    ratio.Format(L"%.2f", ratioOne);	  
    SetDlgItemText(hDlg, IDC_WATER_RATIO_ONE, ratio);	   
    SetDlgItemInt(hDlg, IDC_SEA_CHART_DEPTH, chartDepth, true);
    SetDlgItemInt(hDlg, IDC_UP_MAX_TIDE_HEIGHT_ONE, draftOne, true);	
    SetDlgItemInt(hDlg, IDC_UP_SEA_CHART_DEPTH_ONE, chartDepth, true);	
    SetDlgItemText(hDlg, IDC_UP_WATER_RATIO_ONE, ratio);
    SetDlgItemInt(hDlg, IDC_UP_RESULT_ONE, 0, true);

    ratio.Format(L"%.2f", ratioTwo);	  
    SetDlgItemText(hDlg, IDC_WATER_RATIO_TWO, ratio);
    SetDlgItemInt(hDlg, IDC_UP_MAX_TIDE_HEIGHT_TWO, draftOne, true);
    SetDlgItemInt(hDlg, IDC_UP_SEA_CHART_DEPTH_TWO, chartDepth, true); 
    SetDlgItemText(hDlg, IDC_UP_WATER_RATIO_TWO, ratio);
    SetDlgItemInt(hDlg, IDC_UP_RESULT_TWO, 0, true);

    ratio.Format(L"%.2f", ratioOne);
    SetDlgItemInt(hDlg, IDC_UP_MAX_TIDE_HEIGHT_THREE, draftTwo, true);
    SetDlgItemInt(hDlg, IDC_UP_SEA_CHART_DEPTH_THREE, chartDepth, true);
    SetDlgItemText(hDlg, IDC_UP_WATER_RATIO_THREE, ratio);
    SetDlgItemInt(hDlg, IDC_UP_RESULT_THREE, 0, true);

    ratio.Format(L"%.2f", ratioTwo);
    SetDlgItemInt(hDlg, IDC_UP_MAX_TIDE_HEIGHT_FOUR, draftTwo, true);
    SetDlgItemInt(hDlg, IDC_UP_SEA_CHART_DEPTH_FOUR, chartDepth, true);
    SetDlgItemText(hDlg, IDC_UP_WATER_RATIO_FOUR, ratio);
    SetDlgItemInt(hDlg, IDC_UP_RESULT_FOUR, 0, true);

    // ���½��
    updateDraftResultOne(hDlg, false);
    updateDraftResultTwo(hDlg, false);
    updateDraftResultThree(hDlg, false);
    updateDraftResultFour(hDlg, false);
}
		 
// �������У����۴�������ˮ
INT_PTR CALLBACK ComputeUpDraftCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        HWND parent = ::GetParent(hDlg);
        RECT rcParent = {0, 0, 0, 0};
        RECT rcDlg = {0, 0, 0, 0};
        ::GetWindowRect(parent, &rcParent);
        ::GetWindowRect(hDlg, &rcDlg);

        int x = int(rcParent.left + ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
        int y = int(rcParent.top + ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2);

        ::SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        // �趨��ֵ
        auto& draftData = game.getBeiCaoDraftData();

        int draftOne = draftData.size() > 0 ? draftData[0].availableUpDraftTide : 0;
        int draftTwo = draftData.size() > 1 ? draftData[1].availableUpDraftTide : 0;

		initDraftDialog(hDlg, draftOne, draftTwo);
    }
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDC_CEW_COMPUTER_ONE)
        {
            updateDraftResultOne(hDlg, false);
        }
        else if (LOWORD(wParam) == IDC_CEW_COMPUTER_TWO)
        {
            updateDraftResultTwo(hDlg, false);
        }
        else if (LOWORD(wParam) == IDC_CEW_COMPUTER_THREE)
        {
            updateDraftResultThree(hDlg, false);
        }
        else if (LOWORD(wParam) == IDC_CEW_COMPUTER_FOUR)
        {
            updateDraftResultFour(hDlg, false);
        }
        else if (LOWORD(wParam) == IDOK || LOWORD(wParam) == ID_SET)
        {
			saveDraftParameters(hDlg);
        }

        break;
    }
    return (INT_PTR)FALSE;
}

// �������У�����DWT����7.5��ִ�������ˮ
INT_PTR CALLBACK ComputeUpDWTDraftCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        HWND parent = ::GetParent(hDlg);
        RECT rcParent = {0, 0, 0, 0};
        RECT rcDlg = {0, 0, 0, 0};
        ::GetWindowRect(parent, &rcParent);
        ::GetWindowRect(hDlg, &rcDlg);

        int x = int(rcParent.left + ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
        int y = int(rcParent.top + ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2);

        ::SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        // �趨��ֵ
        auto& draftData = game.getBeiCaoDraftData();
        int draftOne = draftData.size() > 0 ? draftData[0].availableUpDWTDraftTide : 0;
        int draftTwo = draftData.size() > 1 ? draftData[1].availableUpDWTDraftTide : 0;

		initDraftDialog(hDlg, draftOne, draftTwo);
    }
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDC_CEW_COMPUTER_ONE)
        {
            updateDraftResultOne(hDlg, false);
        }
        else if (LOWORD(wParam) == IDC_CEW_COMPUTER_TWO)
        {
            updateDraftResultTwo(hDlg, false);
        }
        else if (LOWORD(wParam) == IDC_CEW_COMPUTER_THREE)
        {
            updateDraftResultThree(hDlg, false);
        }
        else if (LOWORD(wParam) == IDC_CEW_COMPUTER_FOUR)
        {
            updateDraftResultFour(hDlg, false);
        }
        else if (LOWORD(wParam) == IDOK || LOWORD(wParam) == ID_SET)
        {
			saveDraftParameters(hDlg);
        }

        break;
    }
    return (INT_PTR)FALSE;
}

// �������У����۴�������ˮ
INT_PTR CALLBACK ComputeDownDraftCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        HWND parent = ::GetParent(hDlg);
        RECT rcParent = {0, 0, 0, 0};
        RECT rcDlg = {0, 0, 0, 0};
        ::GetWindowRect(parent, &rcParent);
        ::GetWindowRect(hDlg, &rcDlg);

        int x = int(rcParent.left + ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
        int y = int(rcParent.top + ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2);

        ::SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        // �趨��ֵ
        auto& draftData = game.getBeiCaoDraftData();
        int draftOne = draftData.size() > 0 ? draftData[0].availableDownDraftTide : 0;
        int draftTwo = draftData.size() > 1 ? draftData[1].availableDownDraftTide : 0;

		initDraftDialog(hDlg, draftOne, draftTwo);
    }
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDC_CEW_COMPUTER_ONE)
        {
            updateDraftResultOne(hDlg, false);
        }
        else if (LOWORD(wParam) == IDC_CEW_COMPUTER_TWO)
        {
            updateDraftResultTwo(hDlg, false);
        }
        else if (LOWORD(wParam) == IDC_CEW_COMPUTER_THREE)
        {
            updateDraftResultThree(hDlg, false);
        }
        else if (LOWORD(wParam) == IDC_CEW_COMPUTER_FOUR)
        {
            updateDraftResultFour(hDlg, false);
        }
        else if (LOWORD(wParam) == IDOK || LOWORD(wParam) == ID_SET)
        {
			saveDraftParameters(hDlg);
        }

        break;
    }
    return (INT_PTR)FALSE;
}

// Message handler for computer Eat Water box.
INT_PTR CALLBACK ComputeEatWaterCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        HWND parent = ::GetParent(hDlg);
        RECT rcParent = {0, 0, 0, 0};
        RECT rcDlg = {0, 0, 0, 0};
        ::GetWindowRect(parent, &rcParent);
        ::GetWindowRect(hDlg, &rcDlg);

        int x = int(rcParent.left + ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
        int y = int(rcParent.top + ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2);

        ::SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        // �趨��ֵ
        CStringW place;
        int tide = 0;
        int water = 0;
        int sea = 0;

        game.getWaterHeight(true, place, tide, sea, water);
        SetDlgItemText(hDlg, IDC_CEW_PLACE_ONE, place);
        SetDlgItemInt(hDlg, IDC_CEW_TIDE_ONE, tide, true);
        SetDlgItemInt(hDlg, IDC_CEW_SEA_ONE, sea, true);
        SetDlgItemInt(hDlg, IDC_CEW_WATER_ONE, water, true);

        game.getWaterHeight(false, place, tide, sea, water);
        SetDlgItemText(hDlg, IDC_CEW_PLACE_TWO, place);
        SetDlgItemInt(hDlg, IDC_CEW_TIDE_TWO, tide, true);
        SetDlgItemInt(hDlg, IDC_CEW_SEA_TWO, sea, true);
        SetDlgItemInt(hDlg, IDC_CEW_WATER_TWO, water, true);

        // ���½��
        updateWaterHeightResultOne(hDlg, false);
        updateWaterHeightResultTwo(hDlg, false);
    }

        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }

        else if (LOWORD(wParam) == IDC_CEW_COMPUTER_ONE)
        {
            updateWaterHeightResultOne(hDlg);
        }
        else if (LOWORD(wParam) == IDC_CEW_COMPUTER_TWO)
        {
            updateWaterHeightResultTwo(hDlg);
        }

        else if (LOWORD(wParam) == IDOK)
        {
            updateWaterHeightResultOne(hDlg);
            updateWaterHeightResultTwo(hDlg);
        }

        break;
    }

    return (INT_PTR)FALSE;
}

void updateWaterHeightResultOne(HWND hDlg, bool needSave)
{
    wchar_t* wc = new wchar_t[MAX_PATH];
    wmemset(wc, 0, MAX_PATH);

    const int UNKOWN = -10000;
    int water = UNKOWN;
    int tide = UNKOWN;
    int sea = UNKOWN;
    int result = 0;
    CStringW strResult(L"");
    CStringW place(L"");

    GetDlgItemText(hDlg, IDC_CEW_PLACE_ONE, wc, MAX_PATH);
    place.Format(L"%s", wc);
    wmemset(wc, 0, MAX_PATH);

    GetDlgItemText(hDlg, IDC_CEW_TIDE_ONE, wc, MAX_PATH);
    if (wcslen(wc) > 0)
    {
        tide = _wtoi(wc);
        wmemset(wc, 0, MAX_PATH);
    }

    GetDlgItemText(hDlg, IDC_CEW_SEA_ONE, wc, MAX_PATH);
    if (wcslen(wc) > 0)
    {
        sea = _wtoi(wc);
        wmemset(wc, 0, MAX_PATH);
    }

    GetDlgItemText(hDlg, IDC_CEW_WATER_ONE, wc, MAX_PATH);
    if (wcslen(wc) > 0)
    {
        water = _wtoi(wc);
        wmemset(wc, 0, MAX_PATH);
    }

    delete[] wc;
    wc = NULL;

    if (tide != UNKOWN && sea != UNKOWN && water != UNKOWN)
    {
        result = int((tide + sea) * 100 / water);
        strResult.Format(L"%d", result);

        if (needSave)
        {
            game.setWaterHeight(true, place, tide, sea, water);
        }
    }

    SetDlgItemText(hDlg, IDC_CEW_RESULT_ONE, strResult);
}

void updateWaterHeightResultTwo(HWND hDlg, bool needSave)
{
    wchar_t* wc = new wchar_t[MAX_PATH];
    wmemset(wc, 0, MAX_PATH);

    const int UNKOWN = -10000;
    int tide = UNKOWN;
    int water = UNKOWN;
    int sea = UNKOWN;
    int result = 0;
    CStringW strResult(L"");
    CStringW place(L"");

    GetDlgItemText(hDlg, IDC_CEW_PLACE_TWO, wc, MAX_PATH);
    place.Format(L"%s", wc);
    wmemset(wc, 0, MAX_PATH);

    GetDlgItemText(hDlg, IDC_CEW_TIDE_TWO, wc, MAX_PATH);
    if (wcslen(wc) > 0)
    {
        tide = _wtoi(wc);
        wmemset(wc, 0, MAX_PATH);
    }

    GetDlgItemText(hDlg, IDC_CEW_SEA_TWO, wc, MAX_PATH);
    if (wcslen(wc) > 0)
    {
        sea = _wtoi(wc);
        wmemset(wc, 0, MAX_PATH);
    }

    GetDlgItemText(hDlg, IDC_CEW_WATER_TWO, wc, MAX_PATH);
    if (wcslen(wc) > 0)
    {
        water = _wtoi(wc);
        wmemset(wc, 0, MAX_PATH);
    }

    delete[] wc;
    wc = NULL;

    if (tide != UNKOWN && sea != UNKOWN && water != UNKOWN)
    {
        result = int(tide + sea - water);
        strResult.Format(L"%d", result);

        if (needSave)
        {
            game.setWaterHeight(false, place, tide, sea, water);
        }
    }

    SetDlgItemText(hDlg, IDC_CEW_RESULT_TWO, strResult);
}

// Message handler for about box.
INT_PTR CALLBACK AboutCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        HWND parent = ::GetParent(hDlg);
        RECT rcParent = {0, 0, 0, 0};
        RECT rcDlg = {0, 0, 0, 0};
        ::GetWindowRect(parent, &rcParent);
        ::GetWindowRect(hDlg, &rcDlg);

        int x = int(rcParent.left + ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
        int y = int(rcParent.top + ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2);

        ::SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }

        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }

    return (INT_PTR)FALSE;
}

// Message handler for modify password box.
INT_PTR CALLBACK ModifyCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        HWND parent = ::GetParent(hDlg);
        RECT rcParent = {0, 0, 0, 0};
        RECT rcDlg = {0, 0, 0, 0};
        ::GetWindowRect(parent, &rcParent);
        ::GetWindowRect(hDlg, &rcDlg);

        int x = int(rcParent.left + ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
        int y = int(rcParent.top + ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2);

        ::SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        HWND hPwd = ::GetDlgItem(hDlg, IDC_PWD);
        if (hPwd)
        {
            ::SetFocus(hPwd);
        }
    }

        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            wchar_t* wc = new wchar_t[40];
            wmemset(wc, 0, 40);
            GetDlgItemText(hDlg, IDC_OLD_PWD, wc, 40);

            CStringW oldPwd(wc);

            wmemset(wc, 0, 40);
            GetDlgItemText(hDlg, IDC_NEW_PWD, wc, 40);

            CStringW newPwd(wc);

            delete[] wc;
            wc = NULL;

            if (!game.checkPassword(oldPwd))
            {
                MessageBox(hDlg, L"ԭ���벻��ȷ�����������룡", L"����", MB_OK);
                SetDlgItemText(hDlg, IDC_OLD_PWD, L"");
            }
            else if (newPwd.IsEmpty())
            {
                MessageBox(hDlg, L"�����벻��Ϊ�գ������������룡", L"����", MB_OK);
                SetDlgItemText(hDlg, IDC_NEW_PWD, L"");
            }
            else if (newPwd.GetLength() > 24)
            {
                MessageBox(hDlg, L"�����볤�Ȳ��ܳ���12�����������룡", L"����", MB_OK);
                SetDlgItemText(hDlg, IDC_NEW_PWD, L"");
            }
            else
            {
                EndDialog(hDlg, LOWORD(wParam));

                game.modifyPassword(newPwd);

                MessageBox(hDlg, L"���������óɹ����»����������룡", L"��ϲ", MB_OK);
            }

            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }

    return (INT_PTR)FALSE;
}

// Message handler for look box.
INT_PTR CALLBACK LookCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        HWND parent = ::GetParent(hDlg);
        RECT rcParent = {0, 0, 0, 0};
        RECT rcDlg = {0, 0, 0, 0};
        ::GetWindowRect(parent, &rcParent);
        ::GetWindowRect(hDlg, &rcDlg);

        int x = int(rcParent.left + ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
        int y = int(rcParent.top + ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2);

        ::SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        HWND hPwd = ::GetDlgItem(hDlg, IDC_PWD);
        if (hPwd)
        {
            ::SetFocus(hPwd);
        }
    }

        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            wchar_t* wc = new wchar_t[40];
            wmemset(wc, 0, 40);

            GetDlgItemText(hDlg, IDC_DATE, wc, 40);

            CStringW date(wc);
            delete[] wc;
            wc = NULL;

            if (game.checkDate(date))
            {
                EndDialog(hDlg, LOWORD(wParam));

                game.loadDay(date);
            }
            else
            {
                MessageBox(hDlg, L"�������ڸ�ʽ����ȷ�����������룡", L"����", MB_OK);
                SetDlgItemText(hDlg, IDC_DATE, L"");
            }

            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }

    return (INT_PTR)FALSE;
}

// Message handler for login box.
INT_PTR CALLBACK LoginCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        HWND parent = ::GetParent(hDlg);
        RECT rcParent = {0, 0, 0, 0};
        RECT rcDlg = {0, 0, 0, 0};
        ::GetWindowRect(parent, &rcParent);
        ::GetWindowRect(hDlg, &rcDlg);

        int x = int(rcParent.left + ((rcParent.right - rcParent.left) - (rcDlg.right - rcDlg.left)) / 2);
        int y = int(rcParent.top + ((rcParent.bottom - rcParent.top) - (rcDlg.bottom - rcDlg.top)) / 2);

        ::SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        HWND hPwd = ::GetDlgItem(hDlg, IDC_PWD);
        if (hPwd)
        {
            ::SetFocus(hPwd);
        }
    }

        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            wchar_t* wc = new wchar_t[40];
            wmemset(wc, 0, 40);

            GetDlgItemText(hDlg, IDC_PWD, wc, 40);

            CStringW pwd(wc);
            delete[] wc;
            wc = NULL;

            if (game.checkPassword(pwd))
            {
                EndDialog(hDlg, LOWORD(wParam));

                game.loadToday();
            }
            else
            {
                MessageBox(hDlg, L"���벻��ȷ�����������룡", L"����", MB_OK);
                SetDlgItemText(hDlg, IDC_PWD, L"");
            }

            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }

    return (INT_PTR)FALSE;
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    hInst = hinstance;

    HWND hwnd = WinUtility::InitWindow(hinstance, _T("���ӳ�ϫͼ��"), 820, 650, true);  //, 600, true);
    if (!hwnd)
    {
        ::MessageBox(0, _T("Init game window - FAILED"), 0, 0);
        return 0;
    }

    if (!Setup(hwnd))
    {
        ::MessageBox(0, _T("Setup() - FAILED"), 0, 0);
        return 0;
    }

    WinUtility::EnterMsgLoop(Display);

    Cleanup();

    return 0;
}