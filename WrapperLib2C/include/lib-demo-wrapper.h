#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define LIB_DEMO_DLL_EXPORT __declspec(dllexport)
#else
#define LIB_DEMO_DLL_EXPORT
#endif

LIB_DEMO_DLL_EXPORT void *createLibDemo();
LIB_DEMO_DLL_EXPORT void deleteLibDemo(void *instance);

LIB_DEMO_DLL_EXPORT void setValue(void *instance, int value);
LIB_DEMO_DLL_EXPORT int getValue(void *instance);

#ifdef __cplusplus
}
#endif