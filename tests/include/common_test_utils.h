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

extern int s_active_test_objects;

class TestObject {
public:
	int id;
	TestObject() : id(0) {
		s_active_test_objects++;
	}

	TestObject(int _id) : id(_id) {
		s_active_test_objects++;
	}

	~TestObject() {
		s_active_test_objects--;
	}

	TestObject(const TestObject& other) : id(other.id) {
		s_active_test_objects++;
	}

	TestObject& operator=(const TestObject& other) {
		if (this != &other) {
			id = other.id;
		}
		return *this;
	}

	TestObject(TestObject&& other) noexcept : id(other.id) {
		other.id = -1;
	}

	TestObject& operator=(TestObject&& other) noexcept {
		if (this != &other) {
			id		 = other.id;
			other.id = -1;
		}
		return *this;
	}
};

void verify_active_objects(const std::string& test_name, int expected_count);

struct TestResults {
	long long std_min_us, std_max_us, std_avg_us;
	long long raw_min_us, raw_max_us, raw_avg_us;
	double	  raw_vs_std_avg_percent;
};

#endif // SMARTPOINTERS_COMMON_TEST_UTILS_H
