					 /*
 *   Copyright (c) 2008, 飘飘白云(kesalin@gmail.com)
 *   All rights reserved.
 *
 *   文件名称：Game.cpp
 *   摘    要：逻辑框架文件
 *
 *   当前版本：1.1
 *   作    者：飘飘白云
 *   完成日期：2008/11/30
 */

#include "WinUtility.h"
#include "Game.h"
#include "CLog.h"
#include "ScriptManager.h"
#include "ScriptExporter.h"
#include "md5.h"
#include <algorithm>

const static int SCREEN_WIDTH = 820;
const static int SCREEN_HEIGHT = 640;
const static int MAX_SIZE = 1024;

const static float timeW = 28.0f;
const static float tideH = 30.0f;

const static float OFFSET_Y = -50.0f;
const static float TIDE_STEP = 50.0f;
const static int countTime = 26;
const static int countTide = 16;
const static float xLen = timeW * (countTime - 1);
const static float yLen = tideH * countTide;

typedef std::pair<int, std::vector<int>> tidePair;

int getLineFuncX(float y, float x1, float y1, float x2, float y2)
{
    float t1 = x1 / 100 * 60.0f + int(x1) % 100;
    float t2 = x2 / 100 * 60.0f + int(x2) % 100;

    float val = y2 - y1;
    if (val >= 0.0f && val < 0.0001f)
    {
        val = 1.0f;
    }
    else if (val < 0.0f && val > 0.0001f)
    {
        val = -1.0f;
    }

    int t = int((t2 * (y - y1) + t1 * (y2 - y)) / val);
    int realT = int(t / 60) * 100 + t % 60;
    return realT;
}

const static CStringW sysString[] = {
    "时间", "潮高", "第一次高潮", "第一次低潮", "第二次高潮", "第二次低潮",
};

const static CStringW strNums[] = {
    "初一",   "初二",   "初三",   "初四",   "初五",   "初六",   "初七",   "初八",
    "初九",   "初十",   "十一",   "十二",   "十三",   "十四",   "十五",   "十六",
    "十七",   "十八",   "十九",   "二十",   "二十一", "二十二", "二十三", "二十四",
    "二十五", "二十六", "二十七", "二十八", "二十九", "三十",   "三十一",
};

enum
{
    STR_TIME,
    STR_TIDE,
    STR_FIRST_HIGH,
    STR_FIRST_LOW,
    STR_SECOND_HIGH,
    STR_SECOND_LOW,
    STR_MAX,
};

Game::Game()
    : isInited(false),
      isExit(false),
      hWnd(NULL),
      hdcFront(NULL),
      hdcBack(NULL),
      backBmp(0),
      lastTime(0.0f),
      currentTime(0),
      currentTide(0),
      gdiFont(NULL),
      gdiBigFont(NULL),
      boatOne(0),
      boatTwo(0),
      waterOne(0),
      waterTwo(0),
      seaOne(0),
      seaTwo(0),
      cewTideOne(0),
      cewTideTwo(0),
      cewSeaOne(0),
      cewSeaTwo(0),
      cewWaterOne(0),
      cewWaterTwo(0),
      ratioOne(1.12f),
      ratioTwo(1.15f),
      chartDepth(1250),
      isShowTime(true),
      isShowCurrentTide(true),
      fileId(0)
{
    mousePoint.X = mousePoint.Y = 0;
    zeroPoint.X = zeroPoint.Y = 0;

    maxMinTide.resize(8, 0);
    strWNongLi.Format(L"");

    ZeroMemory(&strDate, MAX_PATH * sizeof(wchar_t));
    ZeroMemory(&strPlace, MAX_PATH * sizeof(wchar_t));
    ZeroMemory(&md5, MAX_PATH * sizeof(wchar_t));
    ZeroMemory(&placeOne, MAX_PATH * sizeof(wchar_t));
    ZeroMemory(&placeTwo, MAX_PATH * sizeof(wchar_t));
    ZeroMemory(&cewPlaceOne, MAX_PATH * sizeof(wchar_t));
    ZeroMemory(&cewPlaceTwo, MAX_PATH * sizeof(wchar_t));

    ZeroMemory(&strForbidTimeOne, MAX_PATH * sizeof(wchar_t));
    ZeroMemory(&strForbidTimeTwo, MAX_PATH * sizeof(wchar_t));

    ZeroMemory(&fullPath, MAX_PATH * sizeof(wchar_t));
}

Game::~Game()
{
}

void Game::exit()
{
    ::PostMessage(hWnd, WM_CLOSE, 0, 0);
    isExit = true;
}

bool Game::init(HWND _hWnd)
{
    CLog::output("Game::init()\n");

    hWnd = _hWnd;

    hdcFront = GetDC(hWnd);

    ::GetClientRect(hWnd, (LPRECT)&mainRect);

    // create an offscreen context to draw to
    hdcBack = CreateCompatibleDC(hdcFront);

    // create an offscreen bitmap
    backBmp =
        CreateCompatibleBitmap(hdcFront, mainRect.right - mainRect.left, mainRect.bottom - mainRect.top);

    if (backBmp != NULL)
    {
        // select the bitmap into the offscreen context
        SelectObject(hdcBack, backBmp);
    }

    lastTime = (float)timeGetTime();
    curTime = lastTime;

    isInited = true;

    mousePoint.X = mousePoint.Y = 0;
    zeroPoint.X = mainRect.left + 40.0f;
    zeroPoint.Y = mainRect.bottom - 42.0f;

    Gdiplus::FontFamily fontFamily(L"MS Shell Dlg");
    gdiFont = ::new Gdiplus::Font(&fontFamily, 14, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    gdiBigFont = ::new Gdiplus::Font(&fontFamily, 18, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);

    loginFlag = true;  // false;
    reloadRequest = true;

    start();

    return true;
}

void Game::clear()
{
    isExit = true;
    CLog::output("Game::clear()\n");

    if (gdiFont)
    {
        ::delete gdiFont;
        gdiFont = NULL;
    }

    if (gdiBigFont)
    {
        ::delete gdiBigFont;
        gdiBigFont = NULL;
    }

    // delete the offscreen bitmap and context
    DeleteObject(backBmp);

    // release the back buffer
    DeleteDC(hdcBack);

    // release the window device context
    ReleaseDC(hWnd, hdcFront);
}

void Game::getRichness(bool isPlaceOne, CStringW& place, int& boat, int& water, int& sea)
{
    if (isPlaceOne)
    {
        place.Format(L"%s", placeOne);
        boat = boatOne;
        water = waterOne;
        sea = seaOne;
    }
    else
    {
        place.Format(L"%s", placeTwo);
        boat = boatTwo;
        water = waterTwo;
        sea = seaTwo;
    }
}

void Game::setRichness(bool isPlaceOne, const CStringW& place, int boat, int water, int sea)
{
    if (isPlaceOne)
    {
        swprintf_s(placeOne, MAX_PATH, place);
        boatOne = boat;
        waterOne = water;
        seaOne = sea;

        ScriptExporter::modifyString(fullPath, L"system.lzh", L"place_one", place);
        ScriptExporter::modifyUint32(fullPath, L"system.lzh", L"boat_one", boat);
        ScriptExporter::modifyUint32(fullPath, L"system.lzh", L"richness_percent", water);
        ScriptExporter::modifyUint32(fullPath, L"system.lzh", L"seaMap_depth_one", sea);
    }
    else
    {
        swprintf_s(placeTwo, MAX_PATH, place);
        boatTwo = boat;
        waterTwo = water;
        seaTwo = sea;

        ScriptExporter::modifyString(fullPath, L"system.lzh", L"place_two", place);
        ScriptExporter::modifyUint32(fullPath, L"system.lzh", L"boat_two", boat);
        ScriptExporter::modifyUint32(fullPath, L"system.lzh", L"richness_depth", water);
        ScriptExporter::modifyUint32(fullPath, L"system.lzh", L"seaMap_depth_two", sea);
    }
}

void Game::getWaterHeight(bool isPlaceOne, CStringW& place, int& tide, int& sea, int& water)
{
    if (isPlaceOne)
    {
        place.Format(L"%s", cewPlaceOne);
        tide = cewTideOne;
        water = waterOne;
        sea = seaOne;
    }
    else
    {
        place.Format(L"%s", cewPlaceTwo);
        tide = cewTideTwo;
        water = cewWaterTwo;
        sea = cewSeaTwo;
    }
}

void Game::setWaterHeight(bool isPlaceOne, const CStringW& place, int tide, int sea, int water)
{
    if (isPlaceOne)
    {
        swprintf_s(cewPlaceOne, MAX_PATH, place);
        cewTideOne = tide;
        cewWaterOne = water;
        cewSeaOne = sea;

        ScriptExporter::modifyString(fullPath, L"system.lzh", L"cew_place_one", place);
        ScriptExporter::modifyUint32(fullPath, L"system.lzh", L"cew_tide_one", tide);
        ScriptExporter::modifyUint32(fullPath, L"system.lzh", L"cew_water_one", water);
        ScriptExporter::modifyUint32(fullPath, L"system.lzh", L"cew_sea_one", sea);
    }
    else
    {
        swprintf_s(cewPlaceTwo, MAX_PATH, place);
        cewTideTwo = tide;
        cewWaterTwo = water;
        cewSeaTwo = sea;

        ScriptExporter::modifyString(fullPath, L"system.lzh", L"cew_place_two", place);
        ScriptExporter::modifyUint32(fullPath, L"system.lzh", L"cew_tide_two", tide);
        ScriptExporter::modifyUint32(fullPath, L"system.lzh", L"cew_water_two", water);
        ScriptExporter::modifyUint32(fullPath, L"system.lzh", L"cew_sea_two", sea);
    }
}

int Game::getChartDepth()
{
    return chartDepth;
}

void Game::setChartDepth(int value)
{
    chartDepth = value;
    ScriptExporter::modifyUint32(fullPath, L"system.lzh", L"chart_depth", chartDepth);

	for (auto it = beiCaoDraftData.begin(); it != beiCaoDraftData.end(); ++it)
    {
		BeiCaoDraftData& draft = *it;
        draft.upDraftOne = calculateBeiCaoDraft(ratioOne, draft.availableUpDraftTide, chartDepth);
        draft.upDWTDraftOne = calculateBeiCaoDraft(ratioOne, draft.availableUpDWTDraftTide, chartDepth);
        draft.downDraftOne = calculateBeiCaoDraft(ratioOne, draft.availableDownDraftTide, chartDepth);
        draft.upDraftTwo = calculateBeiCaoDraft(ratioTwo, draft.availableUpDraftTide, chartDepth);
        draft.upDWTDraftTwo = calculateBeiCaoDraft(ratioTwo, draft.availableUpDWTDraftTide, chartDepth);
        draft.downDraftTwo = calculateBeiCaoDraft(ratioTwo, draft.availableDownDraftTide, chartDepth);
    }
}

float Game::getRatioOne()
{
    return ratioOne;
}

void Game::setRatioOne(float ratio)
{
    ratioOne = ratio;
    ScriptExporter::modifyFloat(fullPath, L"system.lzh", L"chart_ratio_one", ratio);

	for (auto it = beiCaoDraftData.begin(); it != beiCaoDraftData.end(); ++it)
    {
		BeiCaoDraftData& draft = *it;
        draft.upDraftOne = calculateBeiCaoDraft(ratioOne, draft.availableUpDraftTide, chartDepth);
        draft.upDWTDraftOne = calculateBeiCaoDraft(ratioOne, draft.availableUpDWTDraftTide, chartDepth);
        draft.downDraftOne = calculateBeiCaoDraft(ratioOne, draft.availableDownDraftTide, chartDepth);
    }
}

float Game::getRatioTwo()
{
    return ratioTwo;
}

void Game::setRatioTwo(float ratio)
{
    ratioTwo = ratio;
    ScriptExporter::modifyFloat(fullPath, L"system.lzh", L"chart_ratio_two", ratio);

	for (auto it = beiCaoDraftData.begin(); it != beiCaoDraftData.end(); ++it)
    {
		BeiCaoDraftData& draft = *it;
        draft.upDraftTwo = calculateBeiCaoDraft(ratioTwo, draft.availableUpDraftTide, chartDepth);
        draft.upDWTDraftTwo = calculateBeiCaoDraft(ratioTwo, draft.availableUpDWTDraftTide, chartDepth);
        draft.downDraftTwo = calculateBeiCaoDraft(ratioTwo, draft.availableDownDraftTide, chartDepth);
    }
}

void Game::start(LPCWSTR string)
{
    if (!isInited)
    {
        return;
    }

    CStringW place(L"");
    CStringW info;
    CStringW title(L"错误");

    wchar_t fullPath[MAX_PATH];
    GetModuleFileNameW(0, fullPath, MAX_PATH);
    PathRemoveFileSpecW(fullPath);

    CStringW path(fullPath);
    PathAppendW(fullPath, L"data");

    ScriptManager scriptManager;
    /*
        // Get password information
        CStringW serial(L"");
        if (scriptManager.open(path, L"dat")){
            scriptManager.getString(serial, L"serial");
            scriptManager.close();
        }
        else {
    #if 0
            info.Format(L"无法打开系统文件。\n%s\\dat!", path);
            ::MessageBox(hWnd, info, title, MB_OK)；
            exit();
            return;
    #endif
        }
    */
    // Get information from script.
    CStringW strPlaceOne(L"");
    CStringW strPlaceTwo(L"");
    if (scriptManager.open(fullPath, L"system.lzh"))
    {
        scriptManager.getString(place, L"default_place");
        scriptManager.getString(places, L"places");
        // scriptManager.getString(serial, L"serial");

        scriptManager.getUint32(boatOne, "boat_one");
        scriptManager.getUint32(boatTwo, "boat_two");
        scriptManager.getUint32(waterOne, "richness_percent");
        scriptManager.getUint32(waterTwo, "richness_depth");
        scriptManager.getUint32(seaOne, "seaMap_depth_one");
        scriptManager.getUint32(seaTwo, "seaMap_depth_two");
        scriptManager.getString(strPlaceOne, L"place_one");
        scriptManager.getString(strPlaceTwo, L"place_two");

        swprintf_s(placeOne, MAX_PATH, strPlaceOne);
        swprintf_s(placeTwo, MAX_PATH, strPlaceTwo);

        scriptManager.getUint32(cewTideOne, "cew_tide_one");
        scriptManager.getUint32(cewTideTwo, "cew_tide_two");
        scriptManager.getUint32(cewSeaOne, "cew_sea_one");
        scriptManager.getUint32(cewSeaTwo, "cew_sea_two");
        scriptManager.getUint32(cewWaterOne, "cew_water_one");
        scriptManager.getUint32(cewWaterTwo, "cew_water_two");
        scriptManager.getString(strPlaceOne, L"cew_place_one");
        scriptManager.getString(strPlaceTwo, L"cew_place_two");

        scriptManager.getFloat(ratioOne, "chart_ratio_one", 1.12f);
        scriptManager.getFloat(ratioTwo, "chart_ratio_two", 1.15f);
        scriptManager.getUint32(chartDepth, "chart_depth", 1250);

        swprintf_s(cewPlaceOne, MAX_PATH, strPlaceOne);
        swprintf_s(cewPlaceTwo, MAX_PATH, strPlaceTwo);

        scriptManager.close();
    }
    else
    {
        info.Format(L"无法打开文件或文件格式错误。\n%s\\system.lzh!", fullPath);

        if (::MessageBox(hWnd, info, title, MB_OK) == IDOK)
        {
            exit();
        }
        return;
    }

    if (place.IsEmpty() && places.size() <= 0)
    {
        info.Format(L"从文件中读取place数据错误。\n%s\\system.lzh!", fullPath);

        if (::MessageBox(hWnd, info, title, MB_OK) == IDOK)
        {
            exit();
        }
        return;
    }
    else if (place.IsEmpty())
    {
        place = places[0];
    }
    /*
        if (serial.GetLength() == 0){
            CStringW pwd(L"123456");
            CStringA pwdA = WinUtility::convertStringW2A(pwd);
            char mbMd5[MAX_PATH];
            MD5String(mbMd5, pwdA, pwdA.GetLength());
            CStringW md5W = WinUtility::convertStringA2W(mbMd5);
            ScriptExporter exporter;
            exporter.modifyString(path, L"dat", L"serial", md5W);
            swprintf_s(md5, MAX_PATH, md5W);
        }
        else{
            swprintf_s(md5, MAX_PATH, serial);
        }
    */
    CStringW dateString = getDateString();
    swprintf_s(today, MAX_PATH, L"%s", dateString.Left(8));
    swprintf_s(strDate, MAX_PATH, L"%s", dateString.Left(8));
    swprintf_s(strPlace, MAX_PATH, L"%s", place);

    loadToday();
}

/* 插入往地址表中插入地址 */
bool Game::inputPlace(const CStringW& place)
{
    if (place.IsEmpty() || place == L"")
    {
        return false;
    }

    std::vector<CStringW>::const_iterator it = places.begin();
    for (; it != places.end(); it++)
    {
        if ((*it).CompareNoCase(place) == 0)
        {
            return true;
        }
    }

    places.push_back(place);

    ScriptExporter::modifyString(fullPath, L"system.lzh", L"places", places);

    return true;
}

bool Game::loadRawData(const CStringW& dateString, const CStringW& place,
                       std::vector<TIDE_DATA>& result, std::vector<int>& maxMinResult, CStringW& nongLi)
{
    if (place.IsEmpty() || place == L"")
    {
        return false;
    }

    CStringW info;
    CStringW title(L"错误");

    GetModuleFileNameW(0, fullPath, MAX_PATH);
    PathRemoveFileSpecW(fullPath);
    PathAppendW(fullPath, L"data");

    // set item's name.
    CStringW fileName;
    fileName.Format(L"%s.lzh", dateString.Left(6));

    CStringW curItem;
    curItem.Format(L"day%s", dateString);

    wchar_t sptPath[MAX_PATH];
    memcpy_s(sptPath, MAX_PATH, fullPath, MAX_PATH);
    PathAppendW(sptPath, place);
    PathAppendW(sptPath, dateString.Left(4));

    getForbidTime(sptPath, dateString);

    std::vector<int> sptData;
    ScriptManager scriptManager;
    if (scriptManager.open(sptPath, fileName))
    {
        scriptManager.getUint32(sptData, curItem);
        scriptManager.close();
    }
    else
    {
        info.Format(L"无法打开文件%s\\%s，\n请检查该文件是否存在或格式是否正确！", sptPath, fileName);

        if (::MessageBox(hWnd, info, title, MB_OK) == IDOK)
        {
            // exit();
        }
        return false;
    }

    if (sptData.size() < 33)
    {
        info.Format(L"无法从文件%s\\%s中\n正确读取潮高数据[%s]，\n请检查文件中的[%s]数据是否正确！",
                    sptPath, fileName, curItem, curItem);

        if (::MessageBox(hWnd, info, title, MB_OK) == IDOK)
        {
            // exit();
        }
        return false;
    }

    result.clear();
    for (int i = 8; i < 33; i++)
    {
        TIDE_DATA data;
        data.tide = sptData[i] * 1.0f;
        data.time = (i - 8) * 100.0f;

        result.push_back(data);
    }

    maxMinResult.clear();
    maxMinResult.resize(8, 0);
    for (int i = 0; i < 4; i++)
    {
        int time = sptData[i * 2 + 0];
        int tide = sptData[i * 2 + 1];

        maxMinResult[i] = time;
        maxMinResult[i + 4] = tide;

        if (time != 0 && tide != 0)
        {
            TIDE_DATA data;
            data.time = time;
            data.tide = tide;
            result.push_back(data);
        }
    }

    std::sort(result.begin(), result.end(), less);

    // 农历
    nongLi.Format(L"");
    if (sptData.size() >= 8 + 25 + 1)
    {
        int index = sptData[33];
        if (index >= 1 && index <= 31)
        {
            nongLi.Format(L"%s", strNums[index - 1]);
        }
    }

    // Log tide data
    for (auto it = result.begin(); it != result.end(); it++)
    {
        const TIDE_DATA& data = (*it);
        PointF pt = getPosition(data);

        CLog::output("position info - %02d:%02d(%d) : (%3.2f, %3.2f)\n", int(data.time / 100.0f),
                     ((int)data.time) % 100, int(data.tide), pt.X, pt.Y);
    }

    return true;
}

// There might be two high heights.
bool Game::getHighTideHeights(const std::vector<int>& maxMinData, std::vector<int>& highData)
{
    highData.clear();

    int time = 0, tide = 0;
    int maxIndex = -1;

    int size = maxMinData.size() / 2;
    for (int i = 0; i < size; ++i)
    {
        if (maxMinData[i + size] > tide)
        {
            time = maxMinData[i];
            tide = maxMinData[i + size];
            maxIndex = i;
        }
    }

    if (maxIndex >= 0)
    {
        highData.push_back(time);
        highData.push_back(tide);

        CLog::output("长兴第一高潮时间：%02d:%02d，潮高：%d cm\n", int(time / 100), time % 100, tide);

        int secondTime = 0, secondTide = 0;
        int secondIndex = -1;
        for (int i = 0; i < size; ++i)
        {
            if (i != maxIndex && maxMinData[i + size] > secondTide)
            {
                secondTime = maxMinData[i];
                secondTide = maxMinData[i + size];
                secondIndex = i;
            }
        }

        // time interval over 10 hours
		int timeInterval = abs(time - secondTime);
        if (secondIndex >= 0 && timeInterval < 1000)
        {
            highData.push_back(secondTime);
            highData.push_back(secondTide);
            CLog::output("长兴第二高潮时间：%02d:%02d，潮高：%d cm, 时间距离：%02d:%02d\n", int(secondTime / 100),
                         secondTime % 100, secondTide, int(timeInterval / 100), timeInterval % 100);
        }
    }

    return !highData.empty();
}

bool Game::getOffsetDateTime(const CStringW& date, int time, int offset, CStringW& offDate, int& offTime)
{
    int timeHour = int(time * 1.0f / 100);
    int timeMinute = time % 100;
    int offsetHour = int(offset * 1.0f / 100);
    int offsetMinute = offset % 100;

    int resultHour = timeHour + offsetHour;
    int resultMinute = timeMinute + offsetMinute;

    if (resultMinute > 60)
    {
        resultHour += 1;
        resultMinute -= 60;
    }
    else if (resultMinute < 0)
    {
        resultHour -= 1;
        resultMinute += 60;
    }

	offTime = resultHour * 100 + resultMinute;
    offDate = date;

    if (offTime < 0)
    {
        offTime = 2400 + offTime;
        offDate = prevDateString(date);
    }
    else if (offTime >= 2400)
    {
        offTime = offTime - 2400;
        offDate = nextDateString(date);
    }

    CLog::output("时间：%02d:%02d, 间隔时间: %d ===> 目标时间：%02d:%02d\n", int(time * 1.0f / 100.0f),
                 time % 100, offset, int(offTime * 1.0f / 100.0f), offTime % 100);
    return true;
}

int Game::calculateBeiCaoDraft(float ratio, float maxTideHeight, float height)
{
    return int((maxTideHeight + height) / ratio);
}

bool Game::getTideAt(const CStringW& place, const CStringW& dateStr, int time, int& tide)
{
    // load date of ChangXing on the same date.
    CStringW strNongLiTemp;
    std::vector<TIDE_DATA> rawData;
    std::vector<int> maxMin;
    if (!loadRawData(dateStr, place, rawData, maxMin, strNongLiTemp))
    {
        CStringW info;
        info.Format(L"无法读取%s的数据，日期：%s", place, dateStr);
        ::MessageBox(hWnd, info, L"错误", MB_OK);
        return false;
    }

    tide = getTideByTime(time, rawData);
    CLog::output("获取 %02d:%02d 的潮高：%d cm\n", int(time * 1.0f / 100.0f), (time) % 100, tide);
    return true;
}

bool Game::loadBeiCaoMaxDraftData()
{
    beiCaoDraftData.clear();

    CStringW placeChangXin(L"长兴（31°22.9′N，121°40.9′E）");
    CStringW placeJiGuJiao(L"鸡骨礁（31°10.4′N，122°22.9′E）");
    CStringW strDateW(strDate);
    CStringW strNongLiTemp;

    // load date of ChangXing on the same date.
    std::vector<TIDE_DATA> changXingCurTideData;
    std::vector<int> changXingCurMaxMin;
    if (!loadRawData(strDateW, placeChangXin, changXingCurTideData, changXingCurMaxMin, strNongLiTemp))
    {
        CStringW info;
        info.Format(L"无法读取%s的数据，日期：%s", placeChangXin, strDateW);
        ::MessageBox(hWnd, info, L"错误", MB_OK);
        return false;
    }

    // Get high tide and time of ChangXing
    std::vector<int> changXingHighHeights;
    if (!getHighTideHeights(changXingCurMaxMin, changXingHighHeights))
    {
        CStringW info;
        info.Format(L"无法读取%s的高低潮数据，日期：%s", placeChangXin, strDateW);
        ::MessageBox(hWnd, info, L"错误", MB_OK);
        return false;
    }

    int offset = changXingHighHeights.size() / 2;
    for (int i = 0; i < offset; ++i)
    {
        int time = changXingHighHeights[i * offset + 0];
        int tide = changXingHighHeights[i * offset + 1];
        CLog::output("长兴高潮时间: %02d:%02d，潮高：%d cm\n", int(time * 1.0f / 100.0f), (time) % 100, tide);

        // Up: 5 hours ago
        CStringW fiveHoursAgoDate(L"");
        int fiveHoursAgoTime;
        getOffsetDateTime(strDateW, time, -400, fiveHoursAgoDate, fiveHoursAgoTime);

        bool ret = false;
        int fiveHoursAgoTide = 0;
        ret = getTideAt(placeJiGuJiao, fiveHoursAgoDate, fiveHoursAgoTime, fiveHoursAgoTide);
        if (!ret)
        {
            CStringW info;
            info.Format(L"无法读取%s的数据，时间：%s %02d:%02d\n", placeJiGuJiao, fiveHoursAgoDate,
                        int(fiveHoursAgoTime * 1.0f / 100), fiveHoursAgoTime % 100);
            ::MessageBox(hWnd, info, L"错误", MB_OK);
            continue;
        }

        // Up: 4 hours ago
        CStringW fourHoursAgoDate(L"");
        int fourHoursAgoTime;
        getOffsetDateTime(strDateW, time, -400, fourHoursAgoDate, fourHoursAgoTime);

        int fourHoursAgoTide = 0;
        ret = getTideAt(placeJiGuJiao, fourHoursAgoDate, fourHoursAgoTime, fourHoursAgoTide);
        if (!ret)
        {
            CStringW info;
            info.Format(L"无法读取%s的数据，时间：%s %02d:%02d\n", placeJiGuJiao, fourHoursAgoDate,
                        int(fourHoursAgoTime * 1.0f / 100), fourHoursAgoTime % 100);
            ::MessageBox(hWnd, info, L"错误", MB_OK);
            continue;
        }

        // Up: 1 hours later
        CStringW oneHoursLaterDate(L"");
        int oneHoursLaterTime;
        getOffsetDateTime(strDateW, time, 100, oneHoursLaterDate, oneHoursLaterTime);

        int oneHoursLaterTide = 0;
        ret = getTideAt(placeChangXin, oneHoursLaterDate, oneHoursLaterTime, oneHoursLaterTide);
        if (!ret)
        {
            CStringW info;
            info.Format(L"无法读取%s的数据，时间：%s %02d:%02d\n", placeChangXin, oneHoursLaterDate,
                        int(oneHoursLaterTime * 1.0f / 100), oneHoursLaterTime % 100);
            ::MessageBox(hWnd, info, L"错误", MB_OK);
            continue;
        }

        // Down: 2.5 hours ago
        CStringW twoHalfHoursAgoDate(L"");
        int twoHalfHoursAgoTime;
        getOffsetDateTime(strDateW, time, -230, twoHalfHoursAgoDate, twoHalfHoursAgoTime);

        int twoHalfHoursAgoTide = 0;
        ret = getTideAt(placeChangXin, twoHalfHoursAgoDate, twoHalfHoursAgoTime, twoHalfHoursAgoTide);
        if (!ret)
        {
            CStringW info;
            info.Format(L"无法读取%s的数据，时间：%s %02d:%02d\n", placeChangXin, twoHalfHoursAgoDate,
                        int(twoHalfHoursAgoTime * 1.0f / 100), twoHalfHoursAgoTime % 100);
            ::MessageBox(hWnd, info, L"错误", MB_OK);
            continue;
        }

        // Down: 2 hours later
        CStringW twoHoursLaterDate(L"");
        int twoHoursLaterTime;
        getOffsetDateTime(strDateW, time, 200, twoHoursLaterDate, twoHoursLaterTime);

        int twoHoursLaterTide = 0;
        ret = getTideAt(placeJiGuJiao, twoHoursLaterDate, twoHoursLaterTime, twoHoursLaterTide);
        if (!ret)
        {
            CStringW info;
            info.Format(L"无法读取%s的数据，时间：%s %02d:%02d\n", placeJiGuJiao, twoHoursLaterDate,
                        int(twoHoursLaterTime * 1.0f / 100), twoHoursLaterTime % 100);
            ::MessageBox(hWnd, info, L"错误", MB_OK);
            continue;
        }

        BeiCaoDraftData draft;
        draft.time = time;

        draft.availableUpDraftTide = min(fiveHoursAgoTide, oneHoursLaterTide);
        draft.availableUpDWTDraftTide = fourHoursAgoTide;
        draft.availableDownDraftTide = min(twoHalfHoursAgoTide, twoHoursLaterTide);

        draft.upDraftOne = calculateBeiCaoDraft(ratioOne, draft.availableUpDraftTide, chartDepth);
        draft.upDraftTwo = calculateBeiCaoDraft(ratioTwo, draft.availableUpDraftTide, chartDepth);
        draft.upDWTDraftOne = calculateBeiCaoDraft(ratioOne, draft.availableUpDWTDraftTide, chartDepth);
        draft.upDWTDraftTwo = calculateBeiCaoDraft(ratioTwo, draft.availableUpDWTDraftTide, chartDepth);

        draft.downDraftOne = calculateBeiCaoDraft(ratioOne, draft.availableDownDraftTide, chartDepth);
        draft.downDraftTwo = calculateBeiCaoDraft(ratioTwo, draft.availableDownDraftTide, chartDepth);

        beiCaoDraftData.push_back(draft);

        if (beiCaoDraftData.size() >= 2)
        {
            std::sort(beiCaoDraftData.begin(), beiCaoDraftData.end());
            break;
        }
    }

    /*
        // demo
        {
            for (int i = 0; i < 2; ++i)
            {
                BeiCaoDraftData dataOne;
                if (i == 0)
                {
                    dataOne.time = 59;
                }
                else
                {
                    dataOne.time = 1337;
                }
                dataOne.upDraftOne = 479 + i * 80;
                dataOne.upDraftTwo = 379 + i * 80;
                dataOne.upDWTDraftOne = 279 + i * 80;
                dataOne.upDWTDraftTwo = 179 + i * 80;
                dataOne.downDraftOne = 579 + i * 80;
                dataOne.downDraftTwo = 679 + i * 80;
                beiCaoDraftData.push_back(dataOne);
            }
        }
        */
}

std::vector<Game::BeiCaoDraftData>& Game::getBeiCaoDraftData()
{
    return beiCaoDraftData;
}

void Game::loadData(const CStringW& dateString, const CStringW& place)
{
    if (!loadRawData(dateString, place, tideData, maxMinTide, strWNongLi))
    {
        return;
    }

    swprintf_s(strDate, MAX_PATH, L"%s", dateString);
    swprintf_s(strPlace, MAX_PATH, L"%s", place);

    // Load max draft date of BeiCao
    loadBeiCaoMaxDraftData();
}

void Game::getForbidTime(const CStringW& path, const CStringW& dateStr)
{
    // 初始化
    ZeroMemory(&strForbidTimeOne, MAX_PATH * sizeof(wchar_t));
    ZeroMemory(&strForbidTimeTwo, MAX_PATH * sizeof(wchar_t));

    // 取得路径
    wchar_t filePath[MAX_PATH];
    swprintf_s(filePath, MAX_PATH, L"%s", path);
    PathAppendW(filePath, L"gz.lzh");

    // 读取文件
    FILE* fp = NULL;
    _tsetlocale(LC_ALL, _T("chinese-simplified"));
    errno_t err = _wfopen_s(&fp, filePath, L"rt");
    if (err == 0 && fp)
    {
        CStringW date(dateStr);
        date.Insert(4, L'-');
        date.Insert(7, L'-');

        CLog::output("\n\n---- date:[%s], path:[%s] ----\n\n", date, filePath);

        static const int MAX_URL = 1024;
        wchar_t readBytes[MAX_URL] = {
            0,
        };
        CStringW readString(L"");
        wmemset(readBytes, 0, MAX_URL);

        int index = 0;
        while (fgetws(readBytes, MAX_URL, fp) != NULL)
        {
            readString.Format(L"%s", readBytes);
            if (readString.Find(date) == 0)
            {
                readString = readString.Right(readString.GetLength() - 11);
                readString.Trim();
                if (index == 0)
                {
                    index = 1;
                    swprintf_s(strForbidTimeOne, MAX_PATH, L"%s", readString);
                }
                else if (index == 1)
                {
                    swprintf_s(strForbidTimeTwo, MAX_PATH, L"%s", readString);
                    break;
                }
            }
        }
    }

    if (fp)
    {
        fclose(fp);
        fp = NULL;
    }
}

bool Game::enter()
{
    if (!isInited || !loginFlag)
    {
        return false;
    }

    curTime = (float)timeGetTime();
    if (curTime - lastTime > 1000)
    {
        CStringW dateString = getDateString();
        if (dateString.Compare(today) != 0)
        {
            if (wcscmp(strDate, today) == 0)
            {
                loadToday();
            }
            else
            {
                CStringW dateString = getDateString();
                swprintf_s(today, MAX_PATH, L"%s", dateString);
            }
        }

        lastTime = curTime;
    }

    currentTime = getTime(mousePoint.X);
    if (currentTime >= 0 && currentTime <= 2400)
    {
        currentTide = getTideByTime(int(currentTime));
    }

    return true;
}

float Game::getTideByTime(int time, const std::vector<TIDE_DATA>& dataVec)
{
    float tide = 0.0f;
    auto it = dataVec.begin();
    auto end = dataVec.end();

    int pos = 0;
    bool isEqual = false;
    for (int i = 0; it != end; it++, i++)
    {
        const TIDE_DATA& data = (*it);
        if (int(data.time) == time)
        {
            isEqual = true;
            tide = data.tide;
            break;
        }
        else if (data.time > time)
        {
            pos = i;
            break;
        }
    }

    if (!isEqual && pos > 0)
    {
        PointF p1(dataVec[pos - 1].time, dataVec[pos - 1].tide);
        PointF p2(dataVec[pos].time, dataVec[pos].tide);

        int t1 = int(dataVec[pos - 1].time);
        int t2 = int(dataVec[pos].time);
        int t = time;

        p1.X = t1 / 100 * 60.0f + t1 % 100;
        p2.X = t2 / 100 * 60.0f + t2 % 100;
        float realT = t / 100 * 60.0f + t % 100;
        float dis = (p2.X - p1.X);
        if (int(dis) != 0)
        {
            tide = ((p1.Y * (p2.X - realT) + p2.Y * (realT - p1.X)) * 1.0f / dis);
        }
    }

    return tide;
}

float Game::getTideByTime(int time)
{
    return getTideByTime(time, tideData);
}

void Game::draw()
{
    if (!isInited || isExit)
    {
        return;
    }

    Gdiplus::Graphics g(hdcBack);

    Gdiplus::SolidBrush bkg(Gdiplus::Color(255, 255, 255, 255));
    Gdiplus::SolidBrush brRed(Gdiplus::Color(255, 255, 0, 0));
    Gdiplus::SolidBrush brGreen(Gdiplus::Color(255, 0, 255, 0));
    Gdiplus::Color crBlack(0, 0, 0);
    Gdiplus::Color crBlue(0, 0, 255);
    Gdiplus::Color crRed(255, 0, 0);

    // Create a Pen object.
    Pen blackPen(Color(255, 0, 0, 0), 1);
    Pen bluePen(Color(255, 0, 0, 255), 1);
    Pen redPen(Color(255, 255, 0, 0), 1);
    Pen greenPen(Color(255, 0, 255, 0), 1);

    PointF pt1, pt2;

    g.FillRectangle(&bkg, mainRect.left, mainRect.top, mainRect.right - mainRect.left,
                    mainRect.bottom - mainRect.top);

    // draw x,y
    PointF zero(0.0f, 0.0f);
    PointF yTop(0.0f, yLen);
    PointF xRight(xLen, 0.0f);

    // Draw axis.
    drawTideLine(&g, &blackPen, zero, yTop);
    drawTideLine(&g, &blackPen, zero, xRight);

    CStringW info;
    info.Format(L"%s H", sysString[STR_TIME]);
    drawTideString(&g, gdiFont, crBlack, info, xLen + 4, 8, true);
    info.Format(L"%s M", sysString[STR_TIDE]);
    drawTideString(&g, gdiFont, crBlack, info, -20, yLen + 20, true);

    for (int i = 1; i < 25; i++)
    {
        pt1.X = pt2.X = i * timeW;
        pt1.Y = 2;
        pt2.Y = -2;

        drawTideLine(&g, &blackPen, pt1, pt2);

        info.Format(L"%d", i);
        drawTideString(&g, gdiFont, crBlack, info, pt1.X - 8, -8, true);

        int value = getTideByTime(i * 100);
        info.Format(L"%d", value);
        drawTideString(&g, gdiFont, crBlack, info, pt1.X - 12, -22, true);
    }

    for (int i = 0; i < countTide; i++)
    {
        pt1.Y = pt2.Y = i * tideH;
        pt1.X = 2;
        pt2.X = -2;

        drawTideLine(&g, &blackPen, pt1, pt2);

        info.Format(L"%.1f", (OFFSET_Y + i * TIDE_STEP) * 1.0f / 100.0f);
        drawTideString(&g, gdiFont, crBlack, info, -30, pt1.Y + 8, true);
    }

    if (loginFlag && tideData.size() > 0)
    {
        if (wcslen(strPlace) > 0 && wcslen(strDate) >= 8)
        {
            CStringW dateW(strDate);
            if (strWNongLi.IsEmpty() || strWNongLi == L"")
            {
                info.Format(L"%s年%s月%s日，%s潮高分布图", dateW.Mid(0, 4), dateW.Mid(4, 2),
                            dateW.Mid(6, 2), strPlace);
                // drawTideString(&g, gdiBigFont, crBlack, info, 120, yLen + 70, true);
                drawCenterString(&g, gdiBigFont, crBlack, info, yLen + 70);
            }
            else
            {
                info.Format(L"%s年%s月%s日，农历%s，%s潮高分布图", dateW.Mid(0, 4), dateW.Mid(4, 2),
                            dateW.Mid(6, 2), strWNongLi, strPlace);
                // drawTideString(&g, gdiBigFont, crBlack, info, 50, yLen + 70, true);
                drawCenterString(&g, gdiBigFont, crBlack, info, yLen + 70);
            }
        }

        // draw first and second high/low tide.
        if (maxMinTide.size() >= 8)
        {
            for (int i = 0; i < 4; i++)
            {
                if ((maxMinTide[i] == 0) && (maxMinTide[i + 4] == 0))
                {
                    continue;
                }

                info.Format(L"(%02d:%02d， %.2f m)", int(maxMinTide[i] / 100),
                            int(maxMinTide[i]) % 100, maxMinTide[i + 4] * 1.0f / 100.0f);
                float x = 300.0f + i % 2 * 200;
                float y = yLen + 18 + (1 - (i) / 2) * 20;
                drawTideString(&g, gdiFont, crBlack, info, x, y, true);
            }
        }

        // draw tide line
        for (size_t i = 0; i < tideData.size() - 1; i++)
        {
            pt1 = getPosition(tideData[i]);
            pt2 = getPosition(tideData[i + 1]);

            drawTideLine(&g, &bluePen, pt1, pt2);
        }

        // draw current time and tide.
        CStringW curTimeString(getCurrentTimeString());
        if (!curTimeString.IsEmpty())
        {
            int timeValue = _wtoi(curTimeString);
            float tideValue = getTideByTime(timeValue);

            float x = 60;
            float y = yLen + 38;
            info.Format(L"现在时间：%s:%s", curTimeString.Left(2), curTimeString.Mid(2));

            drawTideString(&g, gdiFont, crBlack, info, x, y, true);
            info.Format(L"现在潮高：%.2f m", tideValue * 1.0f / 100.0f);

            y = y - 20;
            drawTideString(&g, gdiFont, crBlack, info, x, y, true);

            TIDE_DATA data;
            data.tide = tideValue;
            data.time = timeValue;

            pt1 = getPosition(data);

            if (isShowCurrentTide)
            {
                info.Format(L"(%02d:%02d %.2f m)", int(data.time / 100), int(data.time) % 100,
                            data.tide * 1.0f / 100.0f);
                drawTideString(&g, gdiFont, crBlack, info, pt1.X - 44, pt1.Y + 20, false);
            }

            if ((int(curTime) % 500) > 250)
            {
                pt1.X -= 3;
                pt1.Y += 3;
                drawTideEllipse(&g, &redPen, &brRed, pt1, 6, 6);
            }
        }

        float x = 0, y = 0;

        // 描绘北槽最大吃水
        if (beiCaoDraftData.size() > 0)
        {
            x = 16;
            y = yLen - 5;
            CStringW beicaoMaxDraft("北槽船舶最大吃水：");
            drawTideString(&g, gdiFont, crBlack, beicaoMaxDraft, x, y, false);

            for (int i = 0; i < beiCaoDraftData.size(); ++i)
            {
                auto data = beiCaoDraftData[i];
                CStringW draftInfo;

                x = 20;
                y -= 20;

                draftInfo.Format(L"长兴高潮时：%02d:%02d  上行：%4.2f m", int(data.time / 100),
                                 int(data.time) % 100, data.upDraftOne / 100);
                drawTideString(&g, gdiFont, crBlack, draftInfo, x, y, false);

                x += 290;
                draftInfo.Format(L"上行(DWT大于7.5万吨)：%4.2f m", data.upDWTDraftOne / 100);
                drawTideString(&g, gdiFont, crBlack, draftInfo, x, y, false);

                x += 278;
                draftInfo.Format(L"下行：%4.2f m", data.downDraftOne / 100);
                drawTideString(&g, gdiFont, crBlack, draftInfo, x, y, false);

                x = 20;
                x += 240;
                draftInfo.Format(L"%4.2f m", data.upDraftTwo / 100);
                drawTideString(&g, gdiFont, crRed, draftInfo, x, y, false);

                x += 276;
                draftInfo.Format(L"%4.2f m", data.upDWTDraftTwo / 100);
                drawTideString(&g, gdiFont, crRed, draftInfo, x, y, false);

                x += 154;
                draftInfo.Format(L"%4.2f m", data.downDraftTwo / 100);
                drawTideString(&g, gdiFont, crRed, draftInfo, x, y, false);
            }
        }

        // 描绘管制时间
        x = 220;
        y = yLen - 5;
        CStringW forbidOne(strForbidTimeOne);
        if (!forbidOne.IsEmpty() && forbidOne != L"")
        {
            int pos1 = forbidOne.Find(L'-');
            pos1 = forbidOne.Find(L' ', pos1 + 2);
            int pos2 = forbidOne.Find(L' ', pos1 + 1);
            forbidOne.Format(L"管制时段：%s  允许最大吃水：进 %s，出 %s",
                             forbidOne.Left(pos1),
                             forbidOne.Mid(pos1 + 1, pos2 - pos1 - 1),
                             forbidOne.Mid(pos2 + 1));

            drawTideString(&g, gdiFont, crBlack, forbidOne, x, y, false);
            y -= 20;
        }

        forbidOne.Format(L"%s", strForbidTimeTwo);
        if (!forbidOne.IsEmpty() && forbidOne != L"")
        {
            int pos1 = forbidOne.Find(L'-');
            pos1 = forbidOne.Find(L' ', pos1 + 2);
            int pos2 = forbidOne.Find(L' ', pos1 + 1);
            forbidOne.Format(L"管制时段：%s  允许最大吃水：进 %s，出 %s",
                             forbidOne.Left(pos1),
                             forbidOne.Mid(pos1 + 1, pos2 - pos1 - 1),
                             forbidOne.Mid(pos2 + 1));

            drawTideString(&g, gdiFont, crBlack, forbidOne, x, y, false);
        }
    }

    // draw mouse
    RectF tideRect;
    tideRect.X = 0.0f;
    tideRect.Y = 0.0f;
    tideRect.Width = xLen;
    tideRect.Height = yLen;
    if (isPointInRect(mousePoint, tideRect))
    {
        pt1.X = pt2.X = mousePoint.X;
        pt1.Y = 0;
        pt2.Y = yLen;
        drawTideLine(&g, &greenPen, pt1, pt2);

#if 1
        pt1.Y = pt2.Y = mousePoint.Y;
        pt1.X = 0;
        pt2.X = xLen;
        drawTideLine(&g, &greenPen, pt1, pt2);
#endif

        if (loginFlag && tideData.size() > 0)
        {
            // draw info;
            if (int(currentTime) >= 0 && int(currentTime) <= 2400 && currentTide >= 0 + OFFSET_Y &&
                currentTide < OFFSET_Y + (countTide - 1) * TIDE_STEP)
            {
                TIDE_DATA data;
                data.time = currentTime;
                data.tide = currentTide;
                pt1 = getPosition(data);

                info.Format(L"(%02d:%02d %.2f m)", int(data.time / 100), int(data.time) % 100,
                            data.tide * 1.0f / 100.0f);
                drawTideString(&g, gdiFont, crBlack, info, pt1.X - 44, pt1.Y + 20, false);

                pt1.X -= 3;
                pt1.Y += 3;
                drawTideEllipse(&g, &greenPen, &brGreen, pt1, 6, 6);

                int mouseTide = (int)getTide(mousePoint.Y);

                info.Format(L"%.2f", mouseTide * 1.0f / 100.f);
                drawTideString(&g, gdiFont, crBlue, info, xLen + 24 /*zero.X - 30*/, mousePoint.Y + 8, false);

                if (isShowTime)
                {
                    for (size_t i = 0; i < tideData.size() - 1; i++)
                    {
                        if (mouseTide >= tideData[i].tide && mouseTide <= tideData[i + 1].tide)
                        {
                            int time = getLineFuncX(mouseTide, tideData[i].time, tideData[i].tide,
                                                    tideData[i + 1].time, tideData[i + 1].tide);
                            if (time != 0)
                            {
                                TIDE_DATA data;
                                data.time = time;
                                data.tide = mouseTide;
                                info.Format(L"(%02d:%02d)", time / 100, time % 100);
                                pt1 = getPosition(data);
                                pt1.Y -= 6;

                                drawTideString(&g, gdiFont, crBlack, info, pt1.X, pt1.Y + 8, false);
                            }
                        }

                        else if (mouseTide <= tideData[i].tide && mouseTide >= tideData[i + 1].tide)
                        {
                            int time = getLineFuncX(mouseTide, tideData[i].time, tideData[i].tide,
                                                    tideData[i + 1].time, tideData[i + 1].tide);

                            if (time != 0)
                            {
                                TIDE_DATA data;
                                data.time = time;
                                data.tide = mouseTide;
                                info.Format(L"(%02d:%02d)", time / 100, time % 100);
                                pt1 = getPosition(data);
                                pt1.Y -= 6;

                                drawTideString(&g, gdiFont, crBlack, info, pt1.X, pt1.Y + 8, false);
                            }
                        }
                    }
                }
            }
        }
    }

    BitBlt(hdcFront, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hdcBack, 0, 0, SRCCOPY);
}

void Game::onKeyDown(int keyCode, const Point& pt)
{
    if (!isInited)
    {
        return;
    }

    switch (keyCode)
    {
    case VK_UP:
        mousePoint.Y += 1;
        if (mousePoint.Y > yLen)
        {
            mousePoint.Y = yLen;
        }
        break;
    case VK_DOWN:
        mousePoint.Y -= 1;
        if (mousePoint.Y < 0)
        {
            mousePoint.Y = 0;
        }
        break;
    case VK_LEFT:
        mousePoint.X -= 1;
        if (mousePoint.X < 0)
        {
            mousePoint.X = 0;
        }
        break;
    case VK_RIGHT:
        mousePoint.X += 1;
        if (mousePoint.X > xLen)
        {
            mousePoint.X = xLen;
        }
        break;
    case VK_PRIOR:
        loadPrev();
        break;
    case VK_NEXT:
        loadNext();
        break;
    case VK_HOME:
        loadToday();
        break;
    case VK_F1:
        isShowTime = !isShowTime;
        break;
    case VK_F2:
        isShowCurrentTide = !isShowCurrentTide;
        break;
    default:
        break;
    }
}

void Game::onLeftButtonDown(const Point& pt)
{
    if (!isInited || !hWnd)
    {
        return;
    }
}

void Game::onRightButtonDown(const Point& pt)
{
    if (!isInited || !hWnd || !loginFlag || places.size() <= 0)
    {
        return;
    }

    HMENU hmenu = CreatePopupMenu();

    if (hmenu == NULL)
        return;

    for (size_t i = 0; i < places.size(); i++)
    {
        AppendMenu(hmenu, MF_ENABLED | MF_STRING, MENU_ID_START + i, places[i]);
    }

    ClientToScreen(hWnd, (LPPOINT)&pt);

    TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.X, pt.Y, 0, hWnd, NULL);

    DestroyMenu(hmenu);
}

void Game::onMouseMove(const Point& pt)
{
    if (!isInited || !hWnd)
    {
        return;
    }

    mousePoint.X = pt.X - zeroPoint.X;
    mousePoint.Y = zeroPoint.Y - pt.Y;
}

bool Game::less(const TIDE_DATA& a, const TIDE_DATA& b)
{
    return a.time < b.time;
}

bool Game::isPointInRect(const PointF& pt, const RectF& rect)
{
    return (pt.X >= rect.X && pt.X <= (rect.X + rect.Width) && pt.Y >= rect.Y && pt.Y <= (rect.Y + rect.Height));
}

PointF Game::getPosition(const TIDE_DATA& data)
{
    PointF pt(0.0f, 0.0f);

    pt.X = ((int(data.time / 100) * timeW + (int(data.time) % 100 / 60.0f) * timeW));
    pt.Y = ((data.tide - OFFSET_Y) * tideH / TIDE_STEP);

    return pt;
}

float Game::getTide(float y)
{
    return (y * TIDE_STEP / tideH + OFFSET_Y);
}

float Game::getTime(float x)
{
    int hour = int(x / timeW);
    float minute = (int(x) % int(timeW) * 60.0f / timeW);

    return (hour * 100 + minute);
}

void Game::drawTideEllipse(Gdiplus::Graphics* g, const Pen* pen, Gdiplus::Brush* brush,
                           const PointF& pt1, float r1, float r2)
{
    if (g)
    {
        g->DrawEllipse(pen, pt1.X + zeroPoint.X, zeroPoint.Y - pt1.Y, r1, r2);
        g->FillEllipse(brush, pt1.X + zeroPoint.X, zeroPoint.Y - pt1.Y, r1, r2);
    }
}
void Game::drawTideLine(Gdiplus::Graphics* g, const Pen* pen, const PointF& pt1, const PointF& pt2)
{
    if (g)
    {
        PointF p1(pt1);
        PointF p2(pt2);
        p1.X += zeroPoint.X;
        p1.Y = zeroPoint.Y - p1.Y;
        p2.X += zeroPoint.X;
        p2.Y = zeroPoint.Y - p2.Y;
        g->DrawLine(pen, p1, p2);
    }
}

void Game::drawTideString(Gdiplus::Graphics* g, Gdiplus::Font* font, Gdiplus::Color color,
                          const CStringW& string, float x, float y, bool bAlignLeft /* = true */)
{
    drawString(g, font, color, string, x + zeroPoint.X, zeroPoint.Y - y, bAlignLeft);
}

void Game::drawString(Gdiplus::Graphics* g, Gdiplus::Font* font, Gdiplus::Color color,
                      const CStringW& string, float x, float y, bool bAlignLeft /* = true */)
{
    if (!g || !font)
    {
        return;
    }

    Gdiplus::StringFormat format;
    format.SetAlignment(bAlignLeft ? Gdiplus::StringAlignmentNear : Gdiplus::StringAlignmentFar);

    Gdiplus::RectF rect;
    g->MeasureString(string, -1, font, Gdiplus::RectF(0, 0, 0, 0), &rect);

    rect.X = Gdiplus::REAL(x);
    rect.Y = Gdiplus::REAL(y);
    Gdiplus::SolidBrush brush(color);
    g->DrawString(string, string.GetLength(), font, rect, &format, &brush);
}

void Game::drawCenterString(Gdiplus::Graphics* g, Gdiplus::Font* font, Gdiplus::Color color,
                            const CStringW& string, float y)
{
    if (!g || !font)
    {
        return;
    }

    Gdiplus::RectF rect;
    g->MeasureString(string, -1, font, Gdiplus::RectF(0, 0, 0, 0), &rect);

    rect.X = Gdiplus::REAL((mainRect.right - mainRect.left - rect.Width) / 2.0f);
    rect.Y = Gdiplus::REAL(zeroPoint.Y - y);
    Gdiplus::SolidBrush brush(color);

    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentCenter);

    g->DrawString(string, string.GetLength(), font, rect, &format, &brush);
}

CStringW Game::getDateString()
{
    char tmpbuf[9];
    int dateList[] = {0, 1, 3, 4};
    CStringW dateStr(L"20");
    int i = 0;

    errno_t err = _strdate_s(tmpbuf, 9);
    if (err)
    {
        ZeroMemory(tmpbuf, 9);
    }

    dateStr.AppendChar(tmpbuf[6]);
    dateStr.AppendChar(tmpbuf[7]);

    for (i = 0; i < 4; i++)
    {
        dateStr.AppendChar(tmpbuf[dateList[i]]);
    };

    return dateStr;
}

CStringW Game::getCurrentTimeString()
{
    char tmpbuf[128];
    _tzset();
    errno_t err = _strtime_s(tmpbuf, 128);
    if (err)
    {
        ZeroMemory(tmpbuf, 128);
        return L"";
    }

    CStringW timeStr(L"");

    timeStr.AppendChar(tmpbuf[0]);
    timeStr.AppendChar(tmpbuf[1]);
    timeStr.AppendChar(tmpbuf[3]);
    timeStr.AppendChar(tmpbuf[4]);

    return timeStr;
}

void Game::processMenu(int menuId)
{
    CStringW date(strDate);
    loadData(date, places[menuId]);
}

bool Game::checkPassword(const CStringW& pwd)
{
    CStringA pwdA = WinUtility::convertStringW2A(pwd);
    char newMd5[MAX_PATH];
    MD5String(newMd5, pwdA, pwdA.GetLength());

    CStringA newMd5A(newMd5);
    CStringW newMd5W = WinUtility::convertStringA2W(newMd5A);

    if (newMd5W.Compare(md5) == 0)
    {
        return true;
    }

    return false;
}

void Game::modifyPassword(const CStringW& newPwd)
{
    CStringW info;
    CStringW title(L"错误");

    wchar_t path[MAX_PATH];
    GetModuleFileNameW(0, path, MAX_PATH);
    PathRemoveFileSpecW(path);

    ScriptManager scriptManager;

    // Get password information
    CStringW serial(L"");
    if (scriptManager.open(path, L"dat"))
    {
        scriptManager.close();
    }
    else
    {
        info.Format(L"无法打开系统文件。\n%s\\dat!", path);

        if (::MessageBox(hWnd, info, title, MB_OK) == IDOK)
        {
            exit();
        }
        return;
    }

    CStringA pwdA = WinUtility::convertStringW2A(newPwd);
    char mbMd5[MAX_PATH];
    MD5String(mbMd5, pwdA, pwdA.GetLength());
    CStringW md5W = WinUtility::convertStringA2W(mbMd5);

    ScriptExporter exporter;
    exporter.modifyString(path, L"dat", L"serial", md5W);

    swprintf_s(md5, MAX_PATH, md5W);
}

void Game::loadDay(const CStringW& dateString)
{
    loadData(dateString, strPlace);
}

void Game::loadToday()
{
    loginFlag = true;

    // 更新日期
    CStringW dateString = getDateString();
    swprintf_s(today, MAX_PATH, L"%s", dateString);
    swprintf_s(strDate, MAX_PATH, L"%s", dateString);

    loadData(today, strPlace);
}

CStringW Game::prevDateString(const CStringW& dateString)
{
    CStringW prevDate(L"");
    if (dateString.GetLength() == 8)
    {
        int year = _wtoi(dateString.Left(4));
        int month = _wtoi(dateString.Mid(4, 2));
        int day = _wtoi(dateString.Mid(6, 2));

        if (month >= 1 && month <= 12 && year >= 1000 && year < 10000)
        {
            if (day == 1)
            {
                if (month == 1)
                {
                    year = year - 1;
                    month = 12;
                    day = getDaysOfMonth(year, month);
                }
                else
                {
                    month = month - 1;
                    day = getDaysOfMonth(year, month);
                }
            }
            else
            {
                day = day - 1;
            }

            prevDate.Format(L"%04d%02d%02d", year, month, day);
        }
    }

    return prevDate;
}

CStringW Game::nextDateString(const CStringW& dateString)
{
    CStringW nextDate(L"");
    if (dateString.GetLength() == 8)
    {
        int year = _wtoi(dateString.Left(4));
        int month = _wtoi(dateString.Mid(4, 2));
        int day = _wtoi(dateString.Mid(6, 2));

        if (month >= 1 && month <= 12 && year >= 1000 && year < 10000)
        {
            int maxDayOfCurMonth = getDaysOfMonth(year, month);
            if (day == maxDayOfCurMonth)
            {
                if (month == 12)
                {
                    year = year + 1;
                    month = 1;
                    day = 1;
                }
                else
                {
                    month = month + 1;
                    day = 1;
                }
            }
            else
            {
                day = day + 1;
            }

            nextDate.Format(L"%04d%02d%02d", year, month, day);
        }
    }

    return nextDate;
}

void Game::loadPrev()
{
    CStringW prevDate = prevDateString(strDate);
    if (prevDate != L"" && prevDate.GetLength() == 8)
    {
        loadData(prevDate, strPlace);
    }
    else
    {
        CStringW info;
        info.Format(L"日期 %s 不合法，请检查数据段%s是否合理。", prevDate, strDate);
        ::MessageBox(hWnd, info, L"错误", MB_OK);
    }
}

void Game::loadNext()
{
    CStringW nextDate = nextDateString(strDate);
    if (nextDate != L"" && nextDate.GetLength() == 8)
    {
        loadData(nextDate, strPlace);
    }
    else
    {
        CStringW info;
        info.Format(L"日期 %s 不合法，请检查数据段%s是否合理。", nextDate, strDate);
        ::MessageBox(hWnd, info, L"错误", MB_OK);
    }
}

bool Game::checkDate(const CStringW& date)
{
    if (date.GetLength() != 8)
    {
        return false;
    }

    int year = _wtoi(date.Left(4));
    int month = _wtoi(date.Mid(4, 2));
    int day = _wtoi(date.Mid(6, 2));

    if (month >= 1 && month <= 12 && year >= 1000 && year < 10000)
    {
        int maxDayOfCurMonth = getDaysOfMonth(year, month);
        if (day > 0 && day <= maxDayOfCurMonth)
        {
            return true;
        }
    }

    return false;
}

int Game::getDaysOfMonth(int year, int month)
{
    switch (month)
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        return 31;
    case 4:
    case 6:
    case 9:
    case 11:
        return 30;
    case 2:
        if (year % 4 == 0 && year % 100 != 0 || year % 400 == 0)
            return 29;
        else
            return 28;
    default:
        return 0;
    }
}

bool Game::formatInputData(CStringW& inputData)
{
    inputData.TrimLeft();
    inputData.TrimRight();
    inputData.TrimRight(_T("\r\n"));

    if (inputData.IsEmpty())
    {
        return false;
    }

    inputData.Replace(_T("\r\n"), _T(", "));

    int i = 0;
    int pos = inputData.ReverseFind(_T(','));

    CStringW info;
    info.Format(L"\nOriginal : %s\n\n", inputData);
    OutputDebugStringW(info);

    CStringW temp = inputData.Left(pos);
    for (; !temp.IsEmpty() && i < 7; i++)
    {
        pos = temp.ReverseFind(_T(','));
        temp = temp.Left(pos);

        info.Format(L"\nAfter : %s\n\n", temp);
        OutputDebugStringW(info);
    }

    CStringW part1 = inputData.Mid(pos + 2);

    info.Format(L"\nAfter part1 : %s\n\n", part1);
    OutputDebugStringW(info);

    info.Format(L"\nAfter part2 : %s\n\n", temp);
    OutputDebugStringW(info);

    inputData.Format(L"%s, %s", part1, temp);

    info.Format(L"\nAfter adjust : %s\n\n", inputData);
    OutputDebugStringW(info);

    return true;
}

bool Game::checkInputData(const CStringW& inputData)
{
    if (inputData.IsEmpty())
    {
        return false;
    }

    std::vector<int> values;

    int curPos = 0;
    CStringW temp = inputData.Tokenize(L", ", curPos);
    while (!temp.IsEmpty())
    {
        int v = _wtoi(temp);
        if (v < 0)
        {
            return false;
        }

        values.push_back(v);

        temp = inputData.Tokenize(L", ", curPos);
    };

    if (values.size() < 33)
    {
        return false;
    }

    if (values[0] < 0 || values[0] > 2400 || values[2] < 0 || values[2] > 2400 || values[4] < 0 ||
        values[4] > 2400 || values[6] < 0 || values[6] > 2400 || values[1] < 0 ||
        values[1] > 1000 || values[3] < 0 || values[3] > 1000 || values[5] < 0 ||
        values[5] > 1000 || values[7] < 0 || values[7] > 1000)
    {
        return false;
    }

    for (size_t i = 8; i < values.size(); i++)
    {
        if (values[i] < 0 || values[i] > 1000)
        {
            return false;
        }
    }

    return true;
}

bool Game::inputData(const CStringW& place, const CStringW& date, const CStringW& data)
{
    std::vector<int> values;

    int curPos = 0;
    CStringW temp = data.Tokenize(L", ", curPos);
    while (!temp.IsEmpty())
    {
        int v = _wtoi(temp);
        values.push_back(v);

        temp = data.Tokenize(L", ", curPos);
    };

    wchar_t dataPath[MAX_PATH];
    GetModuleFileNameW(0, dataPath, MAX_PATH);
    PathRemoveFileSpecW(dataPath);
    PathAppendW(dataPath, L"data");
    CStringW path(dataPath);

    PathAppendW(dataPath, (LPCWSTR)place);
    CStringW placePath(dataPath);

    PathAppendW(dataPath, (LPCWSTR)(date.Left(4)));

    CStringW fileName;
    fileName.Format(L"%s.lzh", date.Left(6));

    CStringW fullPath;
    fullPath.Format(L"%s\\%s", dataPath, fileName);

    bool needCreate = false;
    FILE* fp = NULL;
    errno_t err = _wfopen_s(&fp, fullPath, L"rt");
    if (err == 0 && fp)
    {
        fclose(fp);
        fp = NULL;
    }
    else
    {
        needCreate = true;
    }

    if (needCreate)
    {
        // copy default file
        CStringW defaultFile;
        defaultFile.Format(L"%s\\default.lzh", path);

        if (!CopyFileW(defaultFile, fullPath, FALSE))
        {
            bool failed = false;
            // create directory
            if (CreateDirectoryEx(path, placePath, NULL) == ERROR_PATH_NOT_FOUND)
            {
                failed = true;
            }
            else if (CreateDirectoryEx(placePath, dataPath, NULL) == ERROR_PATH_NOT_FOUND)
            {
                failed = true;
            }

            if (!failed && !CopyFileW(defaultFile, fullPath, FALSE))
            {
                failed = true;
            }

            if (failed)
            {
                CStringW info;
                info.Format(L"无法拷贝文件%s\n至%s。\n", defaultFile, fullPath);
                ::MessageBoxW(hWnd, info, L"错误！", MB_OK);

                return false;
            }
        }
    }

    // open script
    ScriptManager sptMng;
    if (!sptMng.open(dataPath, fileName))
    {
        CStringW info;
        info.Format(L"文件%s\n格式不正确。\n", fullPath);
        ::MessageBoxW(hWnd, info, L"错误！", MB_OK);
        return false;
    }
    sptMng.close();

    // modify string
    CStringW item;
    item.Format(L"day%s", date);

    CStringW name;
    name.Format(L"%s-%s", date.Left(4), date.Mid(4, 2));
    bool ret1 = ScriptExporter::modifyString(dataPath, fileName, L"name", name);
    bool ret2 = ScriptExporter::modifyUint32(dataPath, fileName, item, values);

    if (!ret1 || !ret2)
    {
        return false;
    }

    // Modify places
    bool found = false;
    for (size_t i = 0; i < places.size(); i++)
    {
        if (places[i].Compare(place) == 0)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        places.push_back(place);
        ScriptExporter::modifyString(dataPath, L"system.lzh", L"places", places);
    }

    return true;
}

bool Game::writeToScript(const CStringW& place, const CStringW& date, std::vector<std::vector<int>> data)
{
    wchar_t dataPath[MAX_PATH];
    GetModuleFileNameW(0, dataPath, MAX_PATH);
    PathRemoveFileSpecW(dataPath);
    PathAppendW(dataPath, L"data");
    CStringW path(dataPath);

    PathAppendW(dataPath, (LPCWSTR)place);
    CStringW placePath(dataPath);

    PathAppendW(dataPath, (LPCWSTR)(date.Left(4)));

    CStringW fileName;
    fileName.Format(L"%s.lzh", date.Left(6));

    CStringW fullPath;
    fullPath.Format(L"%s\\%s", dataPath, fileName);

    bool needCreate = false;
    FILE* fp = NULL;
    errno_t err = _wfopen_s(&fp, fullPath, L"rt");
    if (err == 0 && fp)
    {
        fclose(fp);
        fp = NULL;
    }
    else
    {
        needCreate = true;
    }

    if (needCreate)
    {
        // copy default file
        CStringW defaultFile;
        defaultFile.Format(L"%s\\default.lzh", path);

        if (!CopyFileW(defaultFile, fullPath, FALSE))
        {
            bool failed = false;
            // create directory
            if (CreateDirectoryEx(path, placePath, NULL) == ERROR_PATH_NOT_FOUND)
            {
                failed = true;
            }
            else if (CreateDirectoryEx(placePath, dataPath, NULL) == ERROR_PATH_NOT_FOUND)
            {
                failed = true;
            }

            if (!failed && !CopyFileW(defaultFile, fullPath, FALSE))
            {
                failed = true;
            }

            if (failed)
            {
                CStringW info;
                info.Format(L"无法拷贝文件%s\n至%s。\n", defaultFile, fullPath);
                ::MessageBoxW(hWnd, info, L"错误！", MB_OK);

                return false;
            }
        }
    }

    // open script
    ScriptManager sptMng;
    if (!sptMng.open(dataPath, fileName))
    {
        CStringW info;
        info.Format(L"文件%s\n格式不正确。\n", fullPath);
        ::MessageBoxW(hWnd, info, L"错误！", MB_OK);
        return false;
    }
    sptMng.close();

    // modify string
    CStringW name;
    name.Format(L"%s-%s", date.Left(4), date.Mid(4, 2));
    bool ret1 = ScriptExporter::modifyString(dataPath, fileName, L"name", name);

    if (!ret1)
    {
        return false;
    }

    int dayCount = data[0].size();
    for (int i = 0; i < dayCount; i++)
    {
        std::vector<int> values;
        // 4个高低潮数据以及时间
        for (int j = 0; j < 8; j++)
        {
            values.push_back(data[2 + 24 + j][i]);
        }

        // 24个潮高数据
        for (int j = 0; j < 24; j++)
        {
            values.push_back(data[2 + j][i]);
        }

        values.push_back(data[34][i]);  // 第25个数据
        values.push_back(data[1][i]);   // 农历日

        CStringW item;
        item.Format(L"day%s%02d", date.Left(6), data[0][i]);
        bool ret2 = ScriptExporter::modifyUint32(dataPath, fileName, item, values);
        if (!ret2)
        {
            return false;
        }
    }

    // Modify places
    bool found = false;
    for (size_t i = 0; i < places.size(); i++)
    {
        if (places[i].Compare(place) == 0)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        places.push_back(place);
        ScriptExporter::modifyString(dataPath, L"system.lzh", L"places", places);
    }

    return true;
}

/* 检查录入数据的合法性:ex */
bool Game::checkInputDataEx(const CStringW& filePath, CStringW& place, CStringW& date, CStringW& resultInfo)
{
    place.Empty();
    date.Empty();

    resultInfo.Format(L"【错误】：\"%s\" 文件格式不正确。请检查重新输入。", filePath);
    std::vector<std::vector<int>> tideData;

    int dayCount = 0;

    CStringW year, month, the25th;
    year.Empty();
    month.Empty();
    the25th.Empty();

    CStringW strMode;
    strMode.Format(L"r, ccs=UTF-8");
    _tsetlocale(LC_ALL, _T("chinese-simplified"));

    FILE* file = NULL;
    // errno_t err = _wfopen_s(&file, filePath, L"rt");
    errno_t err = _wfopen_s(&file, filePath, strMode);

    if (!err && file)
    {
        wchar_t readBytes[MAX_SIZE] = {
            0,
        };
        wmemset(readBytes, 0, MAX_SIZE);

        CStringW readString(L"");
        CStringW temp(L"");

        bool isError = false;

        while (fgetws(readBytes, MAX_SIZE, file) != NULL)
        {
            readString.Format(L"%s", readBytes);
            readString.Replace(L'\t', ' ');
            readString.Trim();

            OutputDebugStringW(readString);
            OutputDebugStringW(L"\n");

            if (readString.Find(L"place=") == 0)
            {
                place = readString.Mid(6);
                place.Trim();
            }
            else if (readString.Find(L"year=") == 0)
            {
                year = readString.Mid(5);
                year.Trim();
            }
            else if (readString.Find(L"month=") == 0)
            {
                month = readString.Mid(6);
                month.Trim();
            }
            else if (readString.Find(L"25th=") == 0)
            {
                the25th = readString.Mid(5);
                the25th.Trim();
            }
            else if (readString != L"")
            {
                readString.MakeUpper();
                readString.Replace(L'L', '1');
                readString.Replace(L'I', '1');
                readString.Replace(L'O', '0');

                std::vector<int> data;
                int pos = 0;
                int days = 0;
                temp = readString.Tokenize(L", ", pos);
                while (temp != L"")
                {
                    int value = _wtoi(temp);
                    data.push_back(value);

                    temp = readString.Tokenize(L", ", pos);

                    ++days;
                }

                if (dayCount == 0)
                {
                    dayCount = days;
                }
                else if (dayCount != days)
                {
                    isError = true;
                    break;
                }

                if (data.size() > 0)
                {
                    tideData.push_back(data);
                }
            }
        }

        if (file)
        {
            fclose(file);
            file = NULL;
        }

        if (year.IsEmpty() || month.IsEmpty() || the25th.IsEmpty())
        {
            isError = true;
        }

        if (isError)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    if (dayCount <= 0 || tideData.size() < 34)
    {
        return false;
    }

    for (int i = 0; i < dayCount; i++)
    {
        if (tideData[0][i] < 1 || tideData[0][i] > 31 || tideData[1][i] < 1 || tideData[1][i] > 31)
        {
            return false;
        }
    }

    resultInfo.Format(L"【成功】：成功导入数据文件 \"%s\"，可录入 %s.%s.%02d 至 %s.%s.%02d 期间 %d 天的数据。请点击【录入数据】按钮录入数据。",
                      filePath, year, month, tideData[0][0], year, month, tideData[0][dayCount - 1], dayCount);
    return true;
}

/* 录入数据:ex */
bool Game::inputDataEx(const CStringW& filePath, const CStringW& place2, CStringW& resultInfo)
{
    resultInfo.Format(L"【错误】：从文件 \"%s\" 录入数据错误，请检查文件格式重试。", filePath);

    CStringW strMode;
    strMode.Format(L"r, ccs=UTF-8");
    _tsetlocale(LC_ALL, _T("chinese-simplified"));

    FILE* file = NULL;
    // errno_t err = _wfopen_s(&file, filePath, L"rt");
    errno_t err = _wfopen_s(&file, filePath, strMode);

    if (err != 0 || file == NULL)
    {
        return false;
    }

    int dayCount = 0;
    CStringW place, date, year, month, the25th;
    resultInfo.Empty();
    place.Empty();
    date.Empty();
    year.Empty();
    month.Empty();
    the25th.Empty();

    //存储数据：公历日 + 农历日 + 24 小时的数据 + 8个时间点以及潮高
    std::vector<std::vector<int>> tideData;

    wchar_t readBytes[MAX_SIZE] = {
        0,
    };
    wmemset(readBytes, 0, MAX_SIZE);

    CStringW readString(L"");
    CStringW temp(L"");

    while (fgetws(readBytes, MAX_SIZE, file) != NULL)
    {
        readString.Format(L"%s", readBytes);
        readString.Replace(L'\t', ' ');
        readString.Trim();

        if (readString.Find(L"place=") == 0)
        {
            place = readString.Mid(6);
            place.Trim();
        }
        else if (readString.Find(L"year=") == 0)
        {
            year = readString.Mid(5);
            year.Trim();
        }
        else if (readString.Find(L"month=") == 0)
        {
            month = readString.Mid(6);
            month.Trim();
        }
        else if (readString.Find(L"25th=") == 0)
        {
            the25th = readString.Mid(5);
            the25th.Trim();
        }
        else if (readString != L"")
        {
            readString.MakeUpper();
            readString.Replace(L'L', '1');
            readString.Replace(L'I', '1');
            readString.Replace(L'O', '0');

            std::vector<int> data;
            int pos = 0;
            temp = readString.Tokenize(L", ", pos);
            while (temp != L"")
            {
                int value = _wtoi(temp);
                data.push_back(value);

                temp = readString.Tokenize(L", ", pos);
            }

            if (data.size() > 0)
            {
                tideData.push_back(data);
            }
        }
    }

    // 关闭文件句柄
    if (file)
    {
        fclose(file);
        file = NULL;
    }

    // 插入25号数据
    dayCount = tideData[0].size();
    std::vector<int> lastData;
    for (int i = 0; i < dayCount - 1; i++)
    {
        lastData.push_back(tideData[2][i + 1]);
    }
    lastData.push_back(_wtoi(the25th));
    tideData.push_back(lastData);

    // 写入脚本文件
    date.Format(L"%s%s", year, month);
    if (writeToScript(place2, date, tideData) == false)
    {
        return false;
    }

    resultInfo.Format(L"【成功】:成功从文件 \"%s\" 录入 %s.%s.%02d - %s.%s.%02d 期间 %d 天的潮高数据。", filePath,
                      year, month, tideData[0][0], year, month, tideData[0][dayCount - 1], dayCount);
    return true;
}