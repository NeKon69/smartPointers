//
// Created by progamers on 5/30/25.
//

#ifndef SMARTPOINTERS_COMMON_TEST_UTILS_H
#define SMARTPOINTERS_COMMON_TEST_UTILS_H

#pragma once

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "../../include/classes/unique_ptr/unique_ptr.h"
#include "../../include/raw_namespace_functions.h"

extern int s_active_test_objects;

class TestObject {
public:
	int id;
	explicit TestObject(int _id = 0) : id(_id) {
		s_active_test_objects++;
	}

	~TestObject() {
		s_active_test_objects--;
	}

	TestObject(TestObject&& other) noexcept : id(other.id) {
		other.id = -1;
	}

	TestObject& operator=(TestObject&& other) noexcept {
		id		 = other.id;
		other.id = -1;
		return *this;
	}
};

void verify_active_objects(const std::string& test_name, int expected_count);

#endif // SMARTPOINTERS_COMMON_TEST_UTILS_H
