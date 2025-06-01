#ifndef SMARTPOINTERS_BENCHMARK_SHARED_H
#define SMARTPOINTERS_BENCHMARK_SHARED_H

#include <chrono>
#include <functional>
#include <iomanip>
#include <memory>
#include <numeric>
#include <random>
#include <vector>

#include "../../include/raw_memory.h"
#include "common_test_utils.h"

void		calculate_stats(const std::vector<long long>& durations, long long& min_val,
							long long& max_val, long long& avg_val);
void		print_table_header();
void		print_table_row(const std::string& scenario_name, const TestResults& results,
							int initial_active_objects, int final_active_objects);
TestResults run_benchmark_scenario(const std::string& scenario_name, int num_trials,
								   int operations_per_trial, std::function<long long(int)> std_func,
								   std::function<long long(int)> raw_func);

long long run_combined_stress_impl_shared(bool use_raw, int iterations, int max_pointers_in_pool);
void	  performance_comparison_shared_test();

template<typename SharedPtrType, typename MakeSharedFunc>
long long run_shared_single_obj_creation_test(int			 operations_per_trial,
											  MakeSharedFunc make_shared_func) {
	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		SharedPtrType ptr_local = make_shared_func(j);
		volatile int  dummy_val = ptr_local ? ptr_local->id : -1;
		(void)dummy_val;
		ptr_local.reset();
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename SharedPtrArrayType, typename MakeSharedArrayFunc>
long long run_shared_array_creation_test(int				 operations_per_trial,
										 MakeSharedArrayFunc make_shared_array_func) {
	std::random_device				rd;
	std::mt19937					gen(rd());
	std::uniform_int_distribution<> dist_array_size(1, 10);

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		size_t			   current_array_size = dist_array_size(gen);
		SharedPtrArrayType ptr_local		  = make_shared_array_func(current_array_size);
		if (ptr_local && current_array_size > 0) {
			volatile int dummy_val = ptr_local[current_array_size / 2].id;
			(void)dummy_val;
		}
		ptr_local.reset();
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename SharedPtrType, typename MakeSharedFunc>
long long run_shared_reset_single_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	std::vector<SharedPtrType> ptrs;
	ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		ptrs.push_back(make_shared_func(j));
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		ptrs[j] = nullptr;
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename SharedPtrArrayType, typename MakeSharedArrayFunc>
long long run_shared_reset_array_test(int				  operations_per_trial,
									  MakeSharedArrayFunc make_shared_array_func) {
	std::random_device				rd;
	std::mt19937					gen(rd());
	std::uniform_int_distribution<> dist_array_size(1, 10);

	std::vector<SharedPtrArrayType> ptrs;
	ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		ptrs.push_back(make_shared_array_func(dist_array_size(gen)));
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		ptrs[j] = nullptr;
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename SharedPtrType, typename MakeSharedFunc>
long long run_shared_move_construct_single_test(int			   operations_per_trial,
												MakeSharedFunc make_shared_func) {
	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		SharedPtrType src_ptr = make_shared_func(j);
		SharedPtrType dest_ptr(std::move(src_ptr));
		volatile int  dummy_val = dest_ptr ? dest_ptr->id : -1;
		(void)dummy_val;
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename SharedPtrType, typename MakeSharedFunc>
long long run_shared_move_assign_single_test(int			operations_per_trial,
											 MakeSharedFunc make_shared_func) {
	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		SharedPtrType src_ptr  = make_shared_func(j);
		SharedPtrType dest_ptr = make_shared_func(j + 1000);
		dest_ptr			   = std::move(src_ptr);
		volatile int dummy_val = dest_ptr ? dest_ptr->id : -1;
		(void)dummy_val;
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename SharedPtrArrayType, typename MakeSharedArrayFunc>
long long run_shared_move_assign_array_test(int					operations_per_trial,
											MakeSharedArrayFunc make_shared_array_func) {
	std::random_device				rd;
	std::mt19937					gen(rd());
	std::uniform_int_distribution<> dist_array_size(1, 10);

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		size_t			   current_array_size_src  = dist_array_size(gen);
		size_t			   current_array_size_dest = dist_array_size(gen);
		SharedPtrArrayType src_ptr				   = make_shared_array_func(current_array_size_src);
		SharedPtrArrayType dest_ptr = make_shared_array_func(current_array_size_dest);
		dest_ptr					= std::move(src_ptr);
		if (dest_ptr && current_array_size_src > 0) {
			volatile int dummy_val = dest_ptr[0].id;
			(void)dummy_val;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename SharedPtrType, typename MakeSharedFunc>
long long run_shared_access_single_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	std::vector<SharedPtrType> ptrs;
	ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		ptrs.push_back(make_shared_func(j));
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

template<typename SharedPtrArrayType, typename MakeSharedArrayFunc>
long long run_shared_access_array_element_test(int				   operations_per_trial,
											   MakeSharedArrayFunc make_shared_array_func) {
	std::random_device				rd;
	std::mt19937					gen(rd());
	std::uniform_int_distribution<> dist_array_size(1, 10);

	std::vector<SharedPtrArrayType> ptrs;
	std::vector<size_t>				sizes;
	ptrs.reserve(operations_per_trial);
	sizes.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		size_t current_size = dist_array_size(gen);
		ptrs.push_back(make_shared_array_func(current_size));
		sizes.push_back(current_size);
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		if (ptrs[j] && sizes[j] > 0) {
			size_t		 elem_idx = std::uniform_int_distribution<size_t>(0, sizes[j] - 1)(gen);
			volatile int val	  = ptrs[j][elem_idx].id;
			(void)val;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename SharedPtrType, typename MakeSharedFunc>
long long run_shared_copy_construct_single_test(int			   operations_per_trial,
												MakeSharedFunc make_shared_func) {
	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		SharedPtrType src_ptr = make_shared_func(j);
		SharedPtrType dest_ptr(src_ptr);
		volatile int  dummy_val = dest_ptr ? dest_ptr->id : -1;
		(void)dummy_val;
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename SharedPtrType, typename MakeSharedFunc>
long long run_shared_copy_assign_single_test(int			operations_per_trial,
											 MakeSharedFunc make_shared_func) {
	std::vector<SharedPtrType> src_ptrs;
	src_ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		src_ptrs.push_back(make_shared_func(j));
	}
	std::vector<SharedPtrType> dest_ptrs;
	dest_ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		dest_ptrs.emplace_back(nullptr);
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		dest_ptrs[j] = src_ptrs[j];
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename SharedPtrArrayType, typename MakeSharedArrayFunc>
long long run_shared_copy_assign_array_test(int					operations_per_trial,
											MakeSharedArrayFunc make_shared_array_func) {
	std::random_device				rd;
	std::mt19937					gen(rd());
	std::uniform_int_distribution<> dist_array_size(1, 10);

	std::vector<SharedPtrArrayType> src_ptrs;
	src_ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		src_ptrs.push_back(make_shared_array_func(dist_array_size(gen)));
	}
	std::vector<SharedPtrArrayType> dest_ptrs;
	dest_ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		dest_ptrs.emplace_back(nullptr);
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		dest_ptrs[j] = src_ptrs[j];
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename SharedPtrType, typename MakeSharedFunc>
long long run_shared_use_count_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	SharedPtrType p_main = make_shared_func(0);
	SharedPtrType p_copy = p_main;

	volatile size_t total_count = 0;
	auto			start		= std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		total_count += p_main.use_count();
	}
	auto end = std::chrono::high_resolution_clock::now();
	(void)total_count;
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename SharedPtrType, typename MakeSharedFunc>
long long run_shared_unique_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	SharedPtrType p_main = make_shared_func(0);
	SharedPtrType p_copy = p_main;

	volatile int unique_count = 0;
	auto		 start		  = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		if (p_main.unique()) {
			unique_count++;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	(void)unique_count;
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

#endif // SMARTPOINTERS_BENCHMARK_SHARED_H