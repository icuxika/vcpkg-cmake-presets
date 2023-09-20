#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LibDemo LibDemoW;

LibDemoW *createLibDemo();
void deleteLibDemo(LibDemoW *instance);

void setValue(LibDemoW *instance, int value);
int getValue(LibDemoW *instance);

#ifdef __cplusplus
}
#endif