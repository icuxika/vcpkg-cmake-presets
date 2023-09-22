#include <iostream>
#include <memory>
#include <string>

class MyWorker {
  public:
	MyWorker(std::string &name) : Name(name) {}
	void doSomething() {
		std::cout << Name << " is doing something..." << std::endl;
	}

  private:
	std::string &Name;
};

class MyClass {
  public:
	MyClass(std::string &name) : Worker(std::make_unique<MyWorker>(name)) {}

	void makeClass() { Worker->doSomething(); }

  private:
	std::unique_ptr<MyWorker> Worker;
};

int main(int argc, char **argv) {
	std::string name = "Aranaga";
	MyClass obj(name);
	obj.makeClass();
	auto myClass = std::make_unique<MyClass>(name);
	name += " (test)";
	myClass->makeClass();
	obj.makeClass();
	return 0;
}
