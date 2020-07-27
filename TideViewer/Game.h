/*   
*   Copyright (c) 2008, ƮƮ����(kesalin@gmail.com)   
*   All rights reserved.   
*     
*   �ļ����ƣ�Game.h   
*   ժ    Ҫ���߼����ͷ�ļ�
*     
*   ��ǰ�汾��1.1   
*   ��    �ߣ�ƮƮ����   
*   ������ڣ�2008/11/30
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

	/* ��Ӧ���̰�����Ϣ */
	void onKeyDown(int keyCode, const Point& pt);

	/* ��Ӧ��������Ϣ */
	void onLeftButtonDown(const Point& pt);
	
	/* ��Ӧ����Ҽ���Ϣ */
	void onRightButtonDown(const Point& pt);
	
	/* ��Ӧ����ƶ���Ϣ */
	void onMouseMove(const Point& pt);

	/* ��� */
	void draw();

	/* �˳�Ӧ�ó��� */
	void exit();

	/* �������ĳ������� */
	void loadToday();

	/* ����ǰһ��ĳ������� */
	void loadPrev();

	/* �����һ��ĳ�������*/
	void loadNext();

	/* ����ָ�����ڵĳ������� */
	void loadDay(const CStringW& dateString);
	
	/* ��ʾ�������� */
	void start(LPCWSTR dateString = NULL);

	/* ȡ���Ҽ��˵��Ĳ˵���Ŀ */
	int getMenuMax(){
		return (int)(MENU_ID_START + places.size());
	}

	/* �������ʦ��ƥ�� */
	bool checkPassword(const CStringW& password);
	
	/* ������ڵĺϷ��� */
	bool checkDate(const CStringW& date);

	/* ��������ַ���в����ַ */
	bool inputPlace(const CStringW& place);
	
	/* ��ʽ��¼������ */
	bool formatInputData(CStringW& inputData);
	
	/* ���¼�����ݵĺϷ��� */
	bool checkInputData(const CStringW& inputData);
	
	/* ¼������ */
	bool inputData(const CStringW& place, const CStringW& date, const CStringW& data);

	/* �޸����� */
	void modifyPassword(const CStringW& password);

	/* �ж��Ƿ��Ѿ���¼ */
	bool isLogined()
	{
		return loginFlag;
	}

	/* �����Ҽ��˵�ָ���ص�ĳ������� */
	void processMenu(int menuId);

	/* ���¼�����ݵĺϷ���:ex */
	bool checkInputDataEx(const CStringW& filePath, CStringW& place, CStringW& date, CStringW& resultInfo);
	/* ¼������:ex */
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
	int fileId;	// �鿴�ı��ļ�������

protected:
	bool writeToScript(const CStringW& place, const CStringW& date, std::vector<std::vector<int> > data);
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

	Gdiplus::Font * gdiFont;
	Gdiplus::Font * gdiBigFont;
	
	struct TIDE_DATA {
		TIDE_DATA():
			tide(0),
			time(0){
		};

		float tide;
		float time;
	};

	struct ChangeXinDraftData {
		ChangeXinDraftData():
			time(0),
			upDraftOne(0),
			upDraftTwo(0),
			upDWTDraftOne(0),
			upDWTDraftTwo(0),
			downDraftOne(0),
			downDraftTwo(0)
		{
		};

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

	void drawTideLine(Gdiplus::Graphics* g, const Pen *pen, const PointF &pt1, const PointF &pt2);
	void drawTideEllipse(Gdiplus::Graphics* g, const Pen *pen, Gdiplus::Brush * brush, const PointF &pt1, float r1, float r2);
	void drawTideString(
		Gdiplus::Graphics * g,
		Gdiplus::Font * font,
		Gdiplus::Color color,
		const CStringW& string,
		float x,
		float y,
		bool bAlignLeft = true);

	void drawString(
		Gdiplus::Graphics * g,
		Gdiplus::Font * font,
		Gdiplus::Color color,
		const CStringW& string,
		float x,
		float y,
		bool bAlignLeft = true);

	void drawCenterString(
		Gdiplus::Graphics * g,
		Gdiplus::Font * font,
		Gdiplus::Color color,
		const CStringW& string,
		float y);

	bool Game::isPointInRect(const PointF& pt, const RectF& rect);

	CStringW getDateString();

	CStringW getCurrentTimeString();

	void loadData(const CStringW& dateString, const CStringW& place);

	std::vector<CStringW> places;
	
	bool loginFlag;
	int getDaysOfMonth(int year, int month);
};