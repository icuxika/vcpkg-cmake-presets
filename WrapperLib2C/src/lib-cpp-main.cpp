#include "lib-demo.h"
#include <iostream>

int main(int argc, char **argv) {
	LibDemo *libDemo = new LibDemo();
	libDemo->setValue(10);
	std::cout << libDemo->getValue() << std::endl;
	return 0;
}
