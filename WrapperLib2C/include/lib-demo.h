#pragma once

#include "lib-demo.h"
#ifdef _WIN32
#define LIB_DEMO_DLL_EXPORT __declspec(dllexport)
#else
#define LIB_DEMO_DLL_EXPORT
#endif

class LIB_DEMO_DLL_EXPORT LibDemo {
  private:
	int PValue;

  public:
	void setValue(int value);
	int getValue();
};