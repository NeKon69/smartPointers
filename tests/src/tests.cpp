#include "../include/run_all.h"
#include "raw/shared_ptr.h"

int main() {
	run_all();
	std::shared_ptr<TestObject[]> test_obj = std::make_shared<TestObject[]>(42);
	return 0;
}