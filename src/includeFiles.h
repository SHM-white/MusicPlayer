#pragma once

//DEBUG
#ifdef _DEBUG
//#undef _WIN32

#endif // 


#ifdef _WIN32
#include <windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")	
#define NOMINMAX
#endif	// _WIN32

#include <qpainter.h>