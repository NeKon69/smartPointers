#include "../include/run_all.h"
#include "raw/shared_ptr.h"

int main() {
	run_all();
	raw::shared_ptr<int> shared_ptr_test = raw::make_shared<int>(42);
	if (shared_ptr_test) {
		std::cout << "Shared pointer test successful, value: " << *shared_ptr_test << "\n";
	} else {
		std::cout << "Shared pointer test failed.\n";
	}
	return 0;
}