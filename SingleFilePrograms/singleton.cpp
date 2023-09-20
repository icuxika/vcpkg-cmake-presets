#include <iostream>

class Singleton {
  private:
	Singleton() { std::cout << "Constructor Singleton" << std::endl; }

	~Singleton() { std::cout << "Destructor Singleton" << std::endl; }

	Singleton(const Singleton &) = delete;
	Singleton &operator=(const Singleton &) = delete;

  public:
	static Singleton &GetInstance() {
		static Singleton instance;
		return instance;
	}

	unsigned long long getAddress() {
		return reinterpret_cast<unsigned long long>(this);
	}
};

// C++ Singleton
int main(int argc, char **argv) {
	std::cout << "Starting Main" << std::endl;

	Singleton &instance1 = Singleton::GetInstance();
	unsigned long long address1 = instance1.getAddress();
	std::cout << "Address of instance 1: " << address1 << std::endl;

	Singleton &instance2 = Singleton::GetInstance();
	unsigned long long address2 = instance2.getAddress();
	std::cout << "Address of instance 2: " << address2 << std::endl;

	std::cout << "Terminating Main" << std::endl;

	return 0;
}
