/*   
*   Copyright (c) 2008, 飘飘白云(l_zhaohui@163.com)   
*   All rights reserved.   
*     
*   文件名称：ScriptManager.cpp   
*   摘    要：脚本读取文件
*     
*   当前版本：1.1   
*   作    者：飘飘白云   
*   完成日期：2008/11/30
*/

#include "ScriptManager.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "include/squirrel.h"
#include "include/sqstdio.h"
#include "include/sqstdaux.h"

#ifdef __cplusplus
}
#endif
static void printfunc(HSQUIRRELVM v, const SQChar* s, ...)
{
	OutputDebugString(s);
}

ScriptManager::ScriptManager() :
	handle(NULL)
{
}

ScriptManager::~ScriptManager(void)
{
	assert(handle == NULL);	//
	close();
}

bool ScriptManager::open(const CStringW& filePath, const CStringW& fileName)
{
	HSQUIRRELVM vm = sq_open(1024); 
	if (vm == NULL) {
		return false;
	}

	CStringW fullPath;

	sqstd_seterrorhandlers(vm);
	sq_setprintfunc(vm, printfunc); // 昞帵梡娭悢傪僙僢僩
	sq_pushroottable(vm);

	fullPath.Format(L"%s\\%s", filePath, fileName);

	if (SQ_SUCCEEDED(sqstd_dofile(vm, (const SQChar*)fullPath, 0, 1))) {
		handle = static_cast<void*>(vm);
		return true;
	}

	return false;
}

void ScriptManager::close()
{
	if (handle != NULL) {
		HSQUIRRELVM vm = static_cast<HSQUIRRELVM>(handle);
		sq_pop(vm, 1);
		sq_close(vm);
		handle = NULL;
	}
}

void ScriptManager::getUint32(int& v, const CStringW& name)
{
	HSQUIRRELVM vm = static_cast<HSQUIRRELVM>(handle);
	v = 0;

	SQInteger val;

	sq_pushstring(vm, (const SQChar*)name, -1);

	if (SQ_SUCCEEDED(sq_get(vm, -2))) {
		SQObjectType ObjectType = sq_gettype(vm, -1);

		if (ObjectType == OT_ARRAY) {
			SQInteger size = sq_getsize(vm, -1);
			sq_pushnull(vm);

			if (SQ_SUCCEEDED(sq_next(vm, -2))) {
				sq_getinteger(vm, -1, &val);
				sq_pop(vm, 2);
				v = (int)val;
			}

			sq_pop(vm, 1);
		}

		sq_pop(vm, 1);
	}
}

void ScriptManager::getUint32(std::vector<int>& v, const CStringW& name)
{
	HSQUIRRELVM vm = static_cast<HSQUIRRELVM>(handle);
	v.clear();

	SQInteger val;

	sq_pushstring(vm, (const SQChar*)name, -1);

	if (SQ_SUCCEEDED(sq_get(vm, -2))) {
		SQObjectType ObjectType = sq_gettype(vm, -1);

		if (ObjectType == OT_ARRAY) {
			SQInteger size = sq_getsize(vm, -1);
			sq_pushnull(vm);

			while (SQ_SUCCEEDED(sq_next(vm, -2))) {
				if (SQ_SUCCEEDED(sq_getinteger(vm, -1, &val))) {
					v.push_back((int)val);
				}

				sq_pop(vm, 2);
			}

			sq_pop(vm, 1);
		}

		sq_pop(vm, 1);
	}
}

void ScriptManager::getString(CStringW& v, const CStringW& name)
{
	HSQUIRRELVM vm = static_cast<HSQUIRRELVM>(handle);

	const SQChar*  str = NULL;
	sq_pushstring(vm, (const SQChar*)name, -1);

	if (SQ_SUCCEEDED(sq_get(vm, -2))) {
		if (SQ_SUCCEEDED(sq_getstring(vm, -1, &str))) {
			v.Format(L"%s", str);
		}

		sq_pop(vm, 1);
	}
}

void ScriptManager::getString(std::vector<CStringW>& v, const CStringW& name)
{
	HSQUIRRELVM vm = static_cast<HSQUIRRELVM>(handle);

	CStringW readStr;
	const SQChar* str = NULL;

	sq_pushstring(vm, (const SQChar*)name, -1);

	if (SQ_SUCCEEDED(sq_get(vm, -2))) {
		SQObjectType ObjectType = sq_gettype(vm, -1);

		if (ObjectType == OT_ARRAY) {
			SQInteger size = sq_getsize(vm, -1);
			sq_pushnull(vm);

			while (SQ_SUCCEEDED(sq_next(vm, -2))) {
				if (SQ_SUCCEEDED(sq_getstring(vm, -1, &str))) {
					readStr.Format(L"%s", str);
					v.push_back(readStr);
				}

				sq_pop(vm, 2);
			}

			sq_pop(vm, 1);
		}

		sq_pop(vm, 1);
	}
}