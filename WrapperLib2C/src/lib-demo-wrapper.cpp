#include "lib-demo-wrapper.h"
#include "lib-demo.h"

#ifdef __cplusplus
extern "C" {
#endif

void *createLibDemo() { return new LibDemo(); }

void deleteLibDemo(void *instance) { delete static_cast<LibDemo *>(instance); }

void setValue(void *instance, int value) {
	static_cast<LibDemo *>(instance)->setValue(value);
}

int getValue(void *instance) {
	return static_cast<LibDemo *>(instance)->getValue();
}

#ifdef __cplusplus
}
#endif