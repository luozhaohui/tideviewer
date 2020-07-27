/*
 *   Copyright (c) 2008, 飘飘白云(kesalin@gmail.com)
 *   All rights reserved.
 *
 *   文件名称：Game.h
 *   摘    要：逻辑框架头文件
 *
 *   当前版本：1.1
 *   作    者：飘飘白云
 *   完成日期：2008/11/30
 */

#pragma once
#include "WinUtility.h"

using namespace Gdiplus;

class Game
{
public:
    Game();
    ~Game();

    bool init(HWND hWnd);
    void clear();

    bool enter();

    //////////////////////////////////////////////////////////////////////////

    /* 响应键盘按键消息 */
    void onKeyDown(int keyCode, const Point& pt);

    /* 响应鼠标左键消息 */
    void onLeftButtonDown(const Point& pt);

    /* 响应鼠标右键消息 */
    void onRightButtonDown(const Point& pt);

    /* 响应鼠标移动消息 */
    void onMouseMove(const Point& pt);

    /* 描绘 */
    void draw();

    /* 退出应用程序 */
    void exit();

    /* 载入今天的潮高数据 */
    void loadToday();

    /* 载入前一天的潮高数据 */
    void loadPrev();

    /* 载入后一天的潮高数据*/
    void loadNext();

    /* 载入指定日期的潮高数据 */
    void loadDay(const CStringW& dateString);

    /* 显示潮高数据 */
    void start(LPCWSTR dateString = NULL);

    /* 取得右键菜单的菜单数目 */
    int getMenuMax()
    {
        return (int)(MENU_ID_START + places.size());
    }

    /* 检查密码师傅匹配 */
    bool checkPassword(const CStringW& password);

    /* 检查日期的合法性 */
    bool checkDate(const CStringW& date);

    /* 插入往地址表中插入地址 */
    bool inputPlace(const CStringW& place);

    /* 格式化录入数据 */
    bool formatInputData(CStringW& inputData);

    /* 检查录入数据的合法性 */
    bool checkInputData(const CStringW& inputData);

    /* 录入数据 */
    bool inputData(const CStringW& place, const CStringW& date, const CStringW& data);

    /* 修改密码 */
    void modifyPassword(const CStringW& password);

    /* 判断是否已经登录 */
    bool isLogined()
    {
        return loginFlag;
    }

    /* 载入右键菜单指定地点的潮高数据 */
    void processMenu(int menuId);

    /* 检查录入数据的合法性:ex */
    bool checkInputDataEx(const CStringW& filePath, CStringW& place, CStringW& date, CStringW& resultInfo);
    /* 录入数据:ex */
    bool inputDataEx(const CStringW& filePath, const CStringW& place, CStringW& resultInfo);

    void setRichness(bool isPlaceOne, const CStringW& place, int boat, int water, int sea);
    void getRichness(bool isPlaceOne, CStringW& place, int& boat, int& water, int& sea);

    void setWaterHeight(bool isPlaceOne, const CStringW& place, int tide, int sea, int water);
    void getWaterHeight(bool isPlaceOne, CStringW& place, int& tide, int& sea, int& water);

    enum
    {
        FILE_NONE,
        FILE_HELP,
        FILE_TUOLUN,
        FILE_MATOU,
        FILE_HANGDAO,
    };
    int fileId;  // 查看文本文件的索引

protected:
    bool writeToScript(const CStringW& place, const CStringW& date, std::vector<std::vector<int>> data);

private:
    HWND hWnd;
    HDC hdcFront;
    HDC hdcBack;
    HBITMAP backBmp;

    ::RECT mainRect;
    bool isInited;
    bool isExit;
    bool isShowTime;
    bool isShowCurrentTide;

    PointF mousePoint;
    PointF zeroPoint;

    Gdiplus::Font* gdiFont;
    Gdiplus::Font* gdiBigFont;

    struct TIDE_DATA
    {
        TIDE_DATA() : tide(0), time(0){};

        float tide;
        float time;
    };

    struct ChangeXinDraftData
    {
        ChangeXinDraftData()
            : time(0),
              upDraftOne(0),
              upDraftTwo(0),
              upDWTDraftOne(0),
              upDWTDraftTwo(0),
              downDraftOne(0),
              downDraftTwo(0){};

        float time;
        float upDraftOne;
        float upDraftTwo;
        float upDWTDraftOne;
        float upDWTDraftTwo;
        float downDraftOne;
        float downDraftTwo;
    };

    std::vector<ChangeXinDraftData> changeXinDraftData;

    std::vector<TIDE_DATA> tideData;
    float maxMinTide[8];

    wchar_t strLongLi[MAX_PATH];
    wchar_t today[MAX_PATH];
    wchar_t strDate[MAX_PATH];
    wchar_t strPlace[MAX_PATH];
    wchar_t md5[MAX_PATH];

    wchar_t fullPath[MAX_PATH];

    wchar_t placeOne[MAX_PATH];
    int boatOne;
    int waterOne;
    int seaOne;

    wchar_t placeTwo[MAX_PATH];
    int boatTwo;
    int waterTwo;
    int seaTwo;

    wchar_t cewPlaceOne[MAX_PATH];
    int cewTideOne;
    int cewWaterOne;
    int cewSeaOne;

    wchar_t cewPlaceTwo[MAX_PATH];
    int cewTideTwo;
    int cewWaterTwo;
    int cewSeaTwo;

    wchar_t strForbidTimeOne[MAX_PATH];
    wchar_t strForbidTimeTwo[MAX_PATH];

    float lastTime;
    float curTime;

    float currentTime;
    float currentTide;

    bool reloadRequest;

    static bool less(const TIDE_DATA& a, const TIDE_DATA& b);

    PointF getPosition(const TIDE_DATA& data);

    float getTide(float y);
    float getTime(float x);

    float getTideByTime(int time);
    void getForbidTime(const CStringW& path, const CStringW& dateStr);

    void drawTideLine(Gdiplus::Graphics* g, const Pen* pen, const PointF& pt1, const PointF& pt2);
    void drawTideEllipse(Gdiplus::Graphics* g, const Pen* pen, Gdiplus::Brush* brush, const PointF& pt1, float r1, float r2);
    void drawTideString(Gdiplus::Graphics* g,
                        Gdiplus::Font* font,
                        Gdiplus::Color color,
                        const CStringW& string,
                        float x,
                        float y,
                        bool bAlignLeft = true);

    void drawString(Gdiplus::Graphics* g,
                    Gdiplus::Font* font,
                    Gdiplus::Color color,
                    const CStringW& string,
                    float x,
                    float y,
                    bool bAlignLeft = true);

    void drawCenterString(Gdiplus::Graphics* g, Gdiplus::Font* font, Gdiplus::Color color, const CStringW& string, float y);

    bool Game::isPointInRect(const PointF& pt, const RectF& rect);

    CStringW getDateString();

    CStringW getCurrentTimeString();

    void loadData(const CStringW& dateString, const CStringW& place);

    std::vector<CStringW> places;

    bool loginFlag;
    int getDaysOfMonth(int year, int month);
};