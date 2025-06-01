#include "../include/run_all.h"

int main() {
	run_all();
	std::cout << sizeof(raw::shared_ptr<TestObject>) << sizeof(std::shared_ptr<TestObject>)
			  << sizeof(raw::hub) << sizeof(combined<TestObject>);
	return 0;
}