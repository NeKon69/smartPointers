//
// Created by progamers on 5/31/25.
//

#ifndef SMARTPOINTERS_BENCHMARK_WEAK_H
#define SMARTPOINTERS_BENCHMARK_WEAK_H

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

long long run_combined_stress_impl_weak(bool use_raw, int iterations, int max_pointers_in_pool);
void	  performance_comparison_weak_test();

template<typename WeakPtrType, typename SharedPtrType, typename MakeSharedFunc>
long long run_weak_from_shared_creation_test(int			operations_per_trial,
											 MakeSharedFunc make_shared_func) {
	std::vector<SharedPtrType> shared_ptrs;
	shared_ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		shared_ptrs.push_back(make_shared_func(j));
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		WeakPtrType	  wp(shared_ptrs[j]);
		volatile bool expired_val = wp.expired();
		(void)expired_val;
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename WeakPtrType, typename SharedPtrType, typename MakeSharedFunc>
long long run_weak_copy_construct_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	std::vector<SharedPtrType> shared_ptrs;
	shared_ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		shared_ptrs.push_back(make_shared_func(j));
	}
	std::vector<WeakPtrType> src_wps;
	src_wps.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		src_wps.emplace_back(shared_ptrs[j]);
	}
	std::vector<WeakPtrType> dest_wps;
	dest_wps.reserve(operations_per_trial);

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		dest_wps.emplace_back(src_wps[j]);
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename WeakPtrType, typename SharedPtrType, typename MakeSharedFunc>
long long run_weak_move_construct_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	std::vector<SharedPtrType> shared_ptrs;
	shared_ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		shared_ptrs.push_back(make_shared_func(j));
	}
	std::vector<WeakPtrType> src_wps;
	src_wps.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		src_wps.emplace_back(shared_ptrs[j]);
	}
	std::vector<WeakPtrType> dest_wps;
	dest_wps.reserve(operations_per_trial);

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		dest_wps.emplace_back(std::move(src_wps[j]));
	}
	auto end = std::chrono::duration_cast<std::chrono::microseconds>(
				   std::chrono::high_resolution_clock::now() - start)
				   .count();
	return end;
}

template<typename WeakPtrType, typename SharedPtrType, typename MakeSharedFunc>
long long run_weak_copy_assign_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	std::vector<SharedPtrType> shared_ptrs;
	shared_ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		shared_ptrs.push_back(make_shared_func(j));
	}
	std::vector<WeakPtrType> src_wps;
	src_wps.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		src_wps.emplace_back(shared_ptrs[j]);
	}
	std::vector<WeakPtrType> dest_wps;
	dest_wps.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		dest_wps.emplace_back(); // ИСПРАВЛЕНО: используем конструктор по умолчанию
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		dest_wps[j] = src_wps[j];
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename WeakPtrType, typename SharedPtrType, typename MakeSharedFunc>
long long run_weak_move_assign_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	std::vector<SharedPtrType> shared_ptrs;
	shared_ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		shared_ptrs.push_back(make_shared_func(j));
	}
	std::vector<WeakPtrType> src_wps;
	src_wps.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		src_wps.emplace_back(shared_ptrs[j]);
	}
	std::vector<WeakPtrType> dest_wps;
	dest_wps.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		dest_wps.emplace_back(); // ИСПРАВЛЕНО: используем конструктор по умолчанию
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		dest_wps[j] = std::move(src_wps[j]);
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename WeakPtrType, typename SharedPtrType, typename MakeSharedFunc>
long long run_weak_shared_assign_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	std::vector<SharedPtrType> shared_ptrs;
	shared_ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		shared_ptrs.push_back(make_shared_func(j));
	}
	std::vector<WeakPtrType> wps;
	wps.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		wps.emplace_back(); // ИСПРАВЛЕНО: используем конструктор по умолчанию
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		wps[j] = shared_ptrs[j];
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename WeakPtrType, typename SharedPtrType, typename MakeSharedFunc>
long long run_weak_reset_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	std::vector<SharedPtrType> shared_ptrs;
	shared_ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		shared_ptrs.push_back(make_shared_func(j));
	}
	std::vector<WeakPtrType> wps;
	wps.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		wps.emplace_back(shared_ptrs[j]);
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		wps[j].reset();
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename WeakPtrType, typename SharedPtrType, typename MakeSharedFunc>
long long run_weak_swap_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	std::vector<SharedPtrType> shared_ptrs_a;
	std::vector<SharedPtrType> shared_ptrs_b;
	shared_ptrs_a.reserve(operations_per_trial);
	shared_ptrs_b.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		shared_ptrs_a.push_back(make_shared_func(j));
		shared_ptrs_b.push_back(make_shared_func(j + operations_per_trial));
	}

	std::vector<WeakPtrType> wp1_vec;
	std::vector<WeakPtrType> wp2_vec;
	wp1_vec.reserve(operations_per_trial);
	wp2_vec.reserve(operations_per_trial);

	for (int j = 0; j < operations_per_trial; ++j) {
		wp1_vec.emplace_back(shared_ptrs_a[j]);
		wp2_vec.emplace_back(shared_ptrs_b[j]);
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		wp1_vec[j].swap(wp2_vec[j]);
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename WeakPtrType, typename SharedPtrType, typename MakeSharedFunc>
long long run_weak_use_count_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	std::vector<SharedPtrType> shared_ptrs;
	std::vector<WeakPtrType>   weak_ptrs;
	shared_ptrs.reserve(operations_per_trial);
	weak_ptrs.reserve(operations_per_trial);

	for (int j = 0; j < operations_per_trial; ++j) {
		shared_ptrs.push_back(make_shared_func(j));
		weak_ptrs.emplace_back(shared_ptrs[j]);
		if (j % 2 == 0) {
			SharedPtrType temp = shared_ptrs[j];
			(void)temp;
		}
	}

	volatile size_t total_count = 0;
	auto			start		= std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		total_count += weak_ptrs[j].use_count();
	}
	auto end = std::chrono::high_resolution_clock::now();
	(void)total_count;
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename WeakPtrType, typename SharedPtrType, typename MakeSharedFunc>
long long run_weak_expired_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	std::vector<SharedPtrType> shared_ptrs;
	std::vector<WeakPtrType>   weak_ptrs;
	shared_ptrs.reserve(operations_per_trial);
	weak_ptrs.reserve(operations_per_trial);

	for (int j = 0; j < operations_per_trial; ++j) {
		shared_ptrs.push_back(make_shared_func(j));
		weak_ptrs.emplace_back(shared_ptrs[j]);
	}

	for (int j = 0; j < operations_per_trial; ++j) {
		if (j % 3 == 0) {
			shared_ptrs[j].reset();
		}
	}

	volatile int expired_count = 0;
	auto		 start		   = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		if (weak_ptrs[j].expired()) {
			expired_count++;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	(void)expired_count;
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename WeakPtrType, typename SharedPtrType, typename MakeSharedFunc>
long long run_weak_lock_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	std::vector<SharedPtrType> shared_ptrs;
	std::vector<WeakPtrType>   weak_ptrs;
	shared_ptrs.reserve(operations_per_trial);
	weak_ptrs.reserve(operations_per_trial);

	for (int j = 0; j < operations_per_trial; ++j) {
		shared_ptrs.push_back(make_shared_func(j));
		weak_ptrs.emplace_back(shared_ptrs[j]);
	}

	for (int j = 0; j < operations_per_trial; ++j) {
		if (j % 3 == 0) {
			shared_ptrs[j].reset();
		}
	}

	volatile int dummy_sum = 0;
	auto		 start	   = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		SharedPtrType locked_ptr = weak_ptrs[j].lock();
		if (locked_ptr) {
			dummy_sum += locked_ptr->id;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	(void)dummy_sum;
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename WeakPtrArrayType, typename SharedPtrArrayType, typename MakeSharedArrayFunc>
long long run_weak_array_from_shared_creation_test(int				   operations_per_trial,
												   MakeSharedArrayFunc make_shared_array_func) {
	std::random_device				rd;
	std::mt19937					gen(rd());
	std::uniform_int_distribution<> dist_array_size(1, 10);

	std::vector<SharedPtrArrayType> shared_ptrs;
	shared_ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		shared_ptrs.push_back(make_shared_array_func(dist_array_size(gen)));
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		WeakPtrArrayType wp(shared_ptrs[j]);
		volatile bool	 expired_val = wp.expired();
		(void)expired_val;
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename WeakPtrArrayType, typename SharedPtrArrayType, typename MakeSharedArrayFunc>
long long run_weak_array_shared_assign_test(int					operations_per_trial,
											MakeSharedArrayFunc make_shared_array_func) {
	std::random_device				rd;
	std::mt19937					gen(rd());
	std::uniform_int_distribution<> dist_array_size(1, 10);

	std::vector<SharedPtrArrayType> shared_ptrs;
	shared_ptrs.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		shared_ptrs.push_back(make_shared_array_func(dist_array_size(gen)));
	}
	std::vector<WeakPtrArrayType> wps;
	wps.reserve(operations_per_trial);
	for (int j = 0; j < operations_per_trial; ++j) {
		wps.emplace_back(); // ИСПРАВЛЕНО: используем конструктор по умолчанию
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		wps[j] = shared_ptrs[j];
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename WeakPtrArrayType, typename SharedPtrArrayType, typename MakeSharedArrayFunc>
long long run_weak_array_lock_test(int				   operations_per_trial,
								   MakeSharedArrayFunc make_shared_array_func) {
	std::random_device				rd;
	std::mt19937					gen(rd());
	std::uniform_int_distribution<> dist_array_size(1, 10);

	std::vector<SharedPtrArrayType> shared_ptrs;
	std::vector<WeakPtrArrayType>	weak_ptrs;
	std::vector<size_t>				array_sizes;
	shared_ptrs.reserve(operations_per_trial);
	weak_ptrs.reserve(operations_per_trial);
	array_sizes.reserve(operations_per_trial);

	for (int j = 0; j < operations_per_trial; ++j) {
		size_t current_size = dist_array_size(gen);
		shared_ptrs.push_back(make_shared_array_func(current_size));
		weak_ptrs.emplace_back(shared_ptrs[j]);
		array_sizes.push_back(current_size);
	}

	for (int j = 0; j < operations_per_trial; ++j) {
		if (j % 3 == 0) {
			shared_ptrs[j].reset();
		}
	}

	volatile int dummy_sum = 0;
	auto		 start	   = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		SharedPtrArrayType locked_ptr = weak_ptrs[j].lock();
		if (locked_ptr && array_sizes[j] > 0) {
			dummy_sum += locked_ptr[0].id;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	(void)dummy_sum;
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

#endif // SMARTPOINTERS_BENCHMARK_WEAK_H