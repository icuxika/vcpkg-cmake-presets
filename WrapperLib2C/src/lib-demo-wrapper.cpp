#include "lib-demo-wrapper.h"
#include "lib-demo.h"

#ifdef __cplusplus
extern "C" {
#endif

LibDemoW *createLibDemo() { return new LibDemo(); }

void deleteLibDemo(LibDemoW *instance) { delete instance; }

void setValue(LibDemoW *instance, int value) { instance->setValue(value); }
int getValue(LibDemoW *instance) { return instance->getValue(); }

#ifdef __cplusplus
}
#endif