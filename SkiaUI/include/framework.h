// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
// 更改头文件的排序会导致导入问题
// NOLINTBEGIN
// Windows Header Files
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <minidumpapiset.h>
#include <pathcch.h>
// C RunTime Header Files
#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include <tchar.h>
// NOLINTEND
