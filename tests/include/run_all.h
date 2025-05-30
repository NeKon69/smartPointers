//
// Created by progamers on 5/30/25.
//

#ifndef SMARTPOINTERS_RUN_ALL_TESTS_H
#define SMARTPOINTERS_RUN_ALL_TESTS_H

#include "benchmark_shared.h"
#include "benchmark_unique.h"
#include "benchmark_weak.h"
#include "unit_shared.h"
#include "unit_unique.h"
#include "unit_weak.h"

void run_all() {
	std::cout
		<< "------------------------------------------- Starting all tests -------------------------------------------\n";
	run_all_unique_tests();
	run_all_shared_tests();
	run_all_weak_tests();
	std::cout
		<< "------------------------------------------- Unit tests completed -------------------------------------------\n";
	performance_comparison_unique_test();
	performance_comparison_shared_test();
	performance_comparison_weak_test();
	std::cout
		<< "------------------------------------------- Performance tests completed -------------------------------------------\n";
}

#endif // SMARTPOINTERS_RUN_ALL_TESTS_H
