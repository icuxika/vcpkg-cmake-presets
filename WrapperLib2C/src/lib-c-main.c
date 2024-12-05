#include "lib-demo-wrapper.h"
#include <stdio.h>

int main(int argc, char **argv) {
	void *instance = createLibDemo();
	setValue(instance, 5);
	printf("%i\n", getValue(instance));
	deleteLibDemo(instance);
	return 0;
}
