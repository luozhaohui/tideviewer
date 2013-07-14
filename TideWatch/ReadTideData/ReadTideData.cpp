// ReadTideData.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

static const int MAX_SIZE = 2048;
typedef std::pair <int, std::vector<int>> tidePair;

void output(CStringW str)
{
	//OutputDebugStringW(str);

	wprintf_s(str);
}

bool getDataFromFile(CStringW& place, CStringW& date, CStringW& the25th, std::map<int, std::vector<int>>& tideMap, const CStringW& filePath)
{
	FILE* file = NULL;

	errno_t err = _wfopen_s(&file, filePath, L"rt");

	CStringW year, month;
	year.Empty();
	month.Empty();

	if (!err && file){
		wchar_t readBytes[MAX_SIZE] = {0, };
		wmemset(readBytes, 0, MAX_SIZE);

		CStringW readString(L"");
		CStringW temp(L"");

		int i = 0;
		while (fgetws(readBytes, MAX_SIZE, file) != NULL) {
			readString.Format(L"%s", readBytes);
			readString.Replace(L'\t', ' ');
			readString.Trim();

			output(readString);
			output(L"\n");

			if (readString.Find(L"place=") == 0){
				place = readString.Mid(6);
				place.Trim();
			}
			else if (readString.Find(L"year=") == 0){
				year = readString.Mid(5);
				year.Trim();
			}
			else if (readString.Find(L"month=") == 0){
				month = readString.Mid(6);
				month.Trim();
			}
			else if (readString.Find(L"25th=") == 0){
				the25th = readString.Mid(5);
				the25th.Trim();
			}
			else if (readString != L""){
				readString.MakeUpper();
				readString.Replace(L'L', '1');
				readString.Replace(L'I', '1');
				readString.Replace(L'O', '0');

				std::vector<int> data;
				int pos = 0;

				temp = readString.Tokenize(L", ", pos);
				while(temp != L""){
					int value = _wtoi(temp);
					data.push_back(value);

					temp = readString.Tokenize(L", ", pos);
				}

				if (data.size() > 0){
					tideMap.insert(tidePair(i, data));
					++i;
				}
			}
		}
	}

	if (file){
		fclose(file);
		file = NULL;
	}

	if (year.IsEmpty() || month.IsEmpty()){
		return false;
	}

	date.Format(L"%s%s", year, month);

	output(L"\n");

	return true;
}

bool checkDayCount(int& dayCount, int& errorLine, const std::map<int, std::vector<int>>& tideMap)
{
	dayCount = 0;
	errorLine = 0;

	if (tideMap.size() != 32){
		return false;
	}

	std::map<int, std::vector<int>>::const_iterator iter = tideMap.begin();
	for (; iter != tideMap.end(); iter++){
		++errorLine;

		const std::vector<int>& dataVec = iter->second;
		if (dayCount == 0){
			dayCount = (int)dataVec.size();
		}
		else if (dayCount != dataVec.size()){
			return false;
		}
	}

	errorLine = 0;
	return true;
}

bool generateTideDayData(std::map<int, std::vector<int>>& dstMap, int startDay, const std::map<int, std::vector<int>>& srcMap, int the25th)
{
	int dayCount = 0;
	int dataCount = (int)(srcMap.size());	// 32
	assert(dataCount == 32);

	std::map<int, std::vector<int>>::const_iterator cstIter = srcMap.find(0);
	if (cstIter != srcMap.end()){
		dayCount = (int)(cstIter->second).size();
	}
	
	for (int dayIndex = 0; dayIndex < dayCount; dayIndex++){
		std::vector<int> vec;

		for (int dataIndex = 0; dataIndex < dataCount + 1; dataIndex++){
			if ( dataIndex == 24){
				if (dayIndex == dayCount - 1){
					vec.push_back(the25th);
				}
				else {
					cstIter = srcMap.find(0);
					if (cstIter != srcMap.end()){
						const std::vector<int>& originalVec = (cstIter->second);
						vec.push_back(originalVec[dayIndex + 1]);
					}
					else {
						assert(0);
						return false;
					}
				}
			}
			else {
				int index = dataIndex > 24? dataIndex - 1:dataIndex;
				cstIter = srcMap.find(index);
				if (cstIter != srcMap.end()){
					const std::vector<int>& originalVec = (cstIter->second);
					vec.push_back(originalVec[dayIndex]);
				}
				else {
					assert(0);
					return false;
				}
			}
		}

		int day = startDay + dayIndex;
		dstMap.insert(tidePair(day, vec));
	}

	return true;
}

int _tmain(int argc, _TCHAR* argv[])
{
	static const CStringW fileName = L"data.txt";
	CStringW data;
	CStringW date;
	CStringW place;
	CStringW the25th;

	std::map<int, std::vector<int>> originalTideMap;

	std::map<int, std::vector<int>> tideMap;
	
	printf("\n=============== START ===============\n");
	
	if (getDataFromFile(place, date, the25th, originalTideMap, fileName)){

		CStringW info;
		info.Format(L"\n Read tide data from %s on %s.\n", place, date);
		output(info);

		int dayCount = 0;
		int errLine = 0;
		int iThe25th = _wtoi(the25th);
		int startDay = _wtoi(date.Mid(6));

		if (checkDayCount(dayCount, errLine, originalTideMap)){
			if (generateTideDayData(tideMap, startDay, originalTideMap, iThe25th)){
				
				std::vector<CStringW> strVec;
				CStringW str;

				std::map<int, std::vector<int>>::const_iterator iter = tideMap.begin();
				for (int i = 0; iter != tideMap.end(); iter++, i++){
					str.Format(L" day_%02d : ", startDay + i);
					
					const std::vector<int>& dataVec = iter->second;
					std::vector<int>::const_iterator dataIter = dataVec.begin();
					for (int j = 0; dataIter != dataVec.end(); dataIter++, j++){
						str.Append(L"%d ", (*dataIter));
					}

					strVec.push_back(str);
				}

				output(L"\n ----------------------------------\n");
				
			}
			else {
				info.Format(L"\n generateTideDayData error.\n");
				output(info);
			}
		}
		else{
			info.Format(L"\n checkDayCount:Read tide data line %d error.\n", errLine);
			output(info);
		}
	}

	printf("\n=============== END ===============\n\n");

	system("pause");
	return 0;
}

