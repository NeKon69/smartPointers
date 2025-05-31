//
// Created by progamers on 5/30/25.
//

#ifndef SMARTPOINTERS_BENCHMARK_UNIQUE_H
#define SMARTPOINTERS_BENCHMARK_UNIQUE_H

#include <chrono>
#include <functional>
#include <iomanip>
#include <numeric>
#include <random>
#include <vector>

#include "common_test_utils.h"

void		calculate_stats(const std::vector<long long>& durations, long long& min_val,
							long long& max_val, long long& avg_val);
void		print_table_header();
void		print_table_row(const std::string& scenario_name, const TestResults& results,
							int initial_active_objects, int final_active_objects);
TestResults run_benchmark_scenario(const std::string& scenario_name, int num_trials,
								   int operations_per_trial, std::function<long long(int)> std_func,
								   std::function<long long(int)> raw_func);
long long	run_combined_stress_impl(bool use_raw, int iterations, int max_pointers_in_pool);
void		performance_comparison_unique_test();

template<typename UniquePtrType, typename MakeUniqueFunc>
long long run_single_obj_creation_test(int operations_per_trial, MakeUniqueFunc make_unique_func) {
	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		UniquePtrType ptr		= make_unique_func(j);
		volatile int  dummy_val = ptr ? ptr->id : -1;
		(void)dummy_val;
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename UniquePtrArrayType, typename MakeUniqueArrayFunc>
long long run_array_creation_test(int				  operations_per_trial,
								  MakeUniqueArrayFunc make_unique_array_func) {
	std::random_device				rd;
	std::mt19937					gen(rd());
	std::uniform_int_distribution<> dist_array_size(1, 10);

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		size_t			   current_array_size = dist_array_size(gen);
		UniquePtrArrayType ptr				  = make_unique_array_func(current_array_size);
		if (ptr && current_array_size > 0) {
			volatile int dummy_val = ptr[0].id;
			(void)dummy_val;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename UniquePtrType, typename MakeUniqueFunc>
long long run_reset_single_test(int operations_per_trial, MakeUniqueFunc make_unique_func) {
	std::vector<UniquePtrType> ptrs;
	ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		ptrs.push_back(make_unique_func(j));
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		ptrs[j].reset();
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename UniquePtrArrayType, typename MakeUniqueArrayFunc>
long long run_reset_array_test(int				   operations_per_trial,
							   MakeUniqueArrayFunc make_unique_array_func) {
	std::random_device				rd;
	std::mt19937					gen(rd());
	std::uniform_int_distribution<> dist_array_size(1, 10);

	std::vector<UniquePtrArrayType> ptrs;
	ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		ptrs.push_back(make_unique_array_func(dist_array_size(gen)));
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		ptrs[j].reset();
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename UniquePtrType, typename MakeUniqueFunc>
long long run_move_construct_single_test(int			operations_per_trial,
										 MakeUniqueFunc make_unique_func) {
	std::vector<UniquePtrType> src_ptrs;
	src_ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		src_ptrs.push_back(make_unique_func(j));
	}
	std::vector<UniquePtrType> dest_ptrs;
	dest_ptrs.reserve(operations_per_trial);

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		dest_ptrs.emplace_back(std::move(src_ptrs[j]));
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename UniquePtrType, typename MakeUniqueFunc>
long long run_move_assign_single_test(int operations_per_trial, MakeUniqueFunc make_unique_func) {
	std::vector<UniquePtrType> src_ptrs;
	src_ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		src_ptrs.push_back(make_unique_func(j));
	}
	std::vector<UniquePtrType> dest_ptrs;
	dest_ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		dest_ptrs.emplace_back(nullptr);
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		dest_ptrs[j] = std::move(src_ptrs[j]);
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename UniquePtrArrayType, typename MakeUniqueArrayFunc>
long long run_move_assign_array_test(int				 operations_per_trial,
									 MakeUniqueArrayFunc make_unique_array_func) {
	std::random_device				rd;
	std::mt19937					gen(rd());
	std::uniform_int_distribution<> dist_array_size(1, 10);

	std::vector<UniquePtrArrayType> src_ptrs;
	src_ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		src_ptrs.push_back(make_unique_array_func(dist_array_size(gen)));
	}
	std::vector<UniquePtrArrayType> dest_ptrs;
	dest_ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		dest_ptrs.emplace_back(nullptr);
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		dest_ptrs[j] = std::move(src_ptrs[j]);
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename UniquePtrType, typename MakeUniqueFunc>
long long run_access_single_test(int operations_per_trial, MakeUniqueFunc make_unique_func) {
	std::vector<UniquePtrType> ptrs;
	ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		ptrs.push_back(make_unique_func(j));
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		volatile int val = ptrs[j]->id;
		(void)val;
		(void)ptrs[j].get();
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename UniquePtrArrayType, typename MakeUniqueArrayFunc>
long long run_access_array_element_test(int					operations_per_trial,
										MakeUniqueArrayFunc make_unique_array_func) {
	std::random_device				rd;
	std::mt19937					gen(rd());
	std::uniform_int_distribution<> dist_array_size(1, 10);

	std::vector<UniquePtrArrayType> ptrs;
	std::vector<size_t>				sizes;
	ptrs.reserve(operations_per_trial);
	sizes.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		size_t current_size = dist_array_size(gen);
		ptrs.push_back(make_unique_array_func(current_size));
		sizes.push_back(current_size);
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		if (ptrs[j] && sizes[j] > 0) {
			volatile int val = ptrs[j][0].id;
			(void)val;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

#endif // SMARTPOINTERS_BENCHMARK_UNIQUE_H
