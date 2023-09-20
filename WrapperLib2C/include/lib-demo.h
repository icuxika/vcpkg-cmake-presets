#pragma once

#include "lib-demo.h"
#ifdef _WIN32
#define LIB_DEMO_DLL_EXPORT __declspec(dllexport)
#else
#define LIB_DEMO_DLL_EXPORT
#endif

class LibDemo {
  private:
	int PValue;

  public:
	LIB_DEMO_DLL_EXPORT void setValue(int value);
	LIB_DEMO_DLL_EXPORT int getValue();
};