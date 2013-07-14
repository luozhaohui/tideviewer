// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#include <stdio.h>
#include <tchar.h>

#include <string>
#include <assert.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
#include <time.h>

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

#include <utility>
#include <map>
#include <vector>

// TODO: reference additional headers your program requires here
