//
// Created by progamers on 5/30/25.
//

#include "../include/common_test_utils.h"

int s_active_test_objects = 0;

void verify_active_objects(const std::string& test_name, int expected_count) {
	if (s_active_test_objects != expected_count) {
		std::cerr << "VERIFY FAILED: " << test_name
				  << " - Active objects: " << s_active_test_objects
				  << " (Expected: " << expected_count << ")\n";
		assert(s_active_test_objects == expected_count);
		exit(1);
	}
}