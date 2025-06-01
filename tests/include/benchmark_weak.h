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
		WeakPtrType wp_local(shared_ptrs[j]);
		if (wp_local.lock()) {
			volatile int id_val = wp_local.lock()->id;
			(void)id_val;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename WeakPtrType, typename SharedPtrType, typename MakeSharedFunc>
long long run_weak_copy_construct_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		SharedPtrType sp = make_shared_func(j);
		WeakPtrType	  src_wp(sp);
		WeakPtrType	  dest_wp(src_wp);
		if (dest_wp.lock()) {
			volatile int dummy = dest_wp.lock()->id;
			(void)dummy;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename WeakPtrType, typename SharedPtrType, typename MakeSharedFunc>
long long run_weak_move_construct_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		SharedPtrType sp = make_shared_func(j);
		WeakPtrType	  src_wp(sp);
		WeakPtrType	  dest_wp(std::move(src_wp));
		if (dest_wp.lock()) {
			volatile int dummy = dest_wp.lock()->id;
			(void)dummy;
		}
	}
	auto end = std::chrono::duration_cast<std::chrono::microseconds>(
				   std::chrono::high_resolution_clock::now() - start)
				   .count();
	return end;
}

template<typename WeakPtrType, typename SharedPtrType, typename MakeSharedFunc>
long long run_weak_copy_assign_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		SharedPtrType sp1 = make_shared_func(j);
		SharedPtrType sp2 = make_shared_func(j + 1000);
		WeakPtrType	  src_wp(sp1);
		WeakPtrType	  dest_wp(sp2);
		dest_wp = src_wp;
		if (dest_wp.lock()) {
			volatile int dummy = dest_wp.lock()->id;
			(void)dummy;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename WeakPtrType, typename SharedPtrType, typename MakeSharedFunc>
long long run_weak_move_assign_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		SharedPtrType sp1 = make_shared_func(j);
		SharedPtrType sp2 = make_shared_func(j + 1000);
		WeakPtrType	  src_wp(sp1);
		WeakPtrType	  dest_wp(sp2);
		dest_wp = std::move(src_wp);
		if (dest_wp.lock()) {
			volatile int dummy = dest_wp.lock()->id;
			(void)dummy;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename WeakPtrType, typename SharedPtrType, typename MakeSharedFunc>
long long run_weak_shared_assign_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		SharedPtrType sp = make_shared_func(j);
		WeakPtrType	  wp;
		wp = sp;
		if (wp.lock()) {
			volatile int dummy = wp.lock()->id;
			(void)dummy;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename WeakPtrType, typename SharedPtrType, typename MakeSharedFunc>
long long run_weak_reset_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		SharedPtrType sp = make_shared_func(j);
		WeakPtrType	  wp(sp);
		wp.reset();
		volatile bool is_expired = wp.expired();
		(void)is_expired;
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename WeakPtrType, typename SharedPtrType, typename MakeSharedFunc>
long long run_weak_swap_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		SharedPtrType sp1 = make_shared_func(j);
		SharedPtrType sp2 = make_shared_func(j + 1000);
		WeakPtrType	  wp1(sp1);
		WeakPtrType	  wp2(sp2);
		wp1.swap(wp2);
		if (wp1.lock()) {
			volatile int dummy = wp1.lock()->id;
			(void)dummy;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename WeakPtrType, typename SharedPtrType, typename MakeSharedFunc>
long long run_weak_use_count_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	SharedPtrType p_main  = make_shared_func(0);
	SharedPtrType p_copy1 = p_main;
	SharedPtrType p_copy2 = p_main;
	WeakPtrType	  wp(p_main);

	volatile size_t total_count = 0;
	auto			start		= std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		total_count += wp.use_count();
	}
	auto end = std::chrono::high_resolution_clock::now();
	(void)total_count;
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename WeakPtrType, typename SharedPtrType, typename MakeSharedFunc>
long long run_weak_expired_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	SharedPtrType p_main = make_shared_func(0);
	WeakPtrType	  wp(p_main);

	volatile int expired_count = 0;
	auto		 start		   = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		if (j % 2 == 0) {
			p_main.reset();
		} else if (!p_main) {
			p_main = make_shared_func(j);
			wp	   = p_main;
		}
		if (wp.expired()) {
			expired_count++;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	(void)expired_count;
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename WeakPtrType, typename SharedPtrType, typename MakeSharedFunc>
long long run_weak_lock_test(int operations_per_trial, MakeSharedFunc make_shared_func) {
	SharedPtrType p_main = make_shared_func(0);
	WeakPtrType	  wp(p_main);

	volatile int dummy_sum = 0;
	auto		 start	   = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		if (j % 2 == 0) {
			p_main.reset();
		} else if (!p_main) {
			p_main = make_shared_func(j);
			wp	   = p_main;
		}
		SharedPtrType locked_ptr = wp.lock();
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

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		size_t			   current_array_size = dist_array_size(gen);
		SharedPtrArrayType sp				  = make_shared_array_func(current_array_size);
		WeakPtrArrayType   wp(sp);
		if (wp.lock() && current_array_size > 0) {
			volatile int dummy = wp.lock()[0].id;
			(void)dummy;
		}
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

	auto start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		size_t			   current_array_size = dist_array_size(gen);
		SharedPtrArrayType sp				  = make_shared_array_func(current_array_size);
		WeakPtrArrayType   wp;
		wp = sp;
		if (wp.lock() && current_array_size > 0) {
			volatile int dummy = wp.lock()[0].id;
			(void)dummy;
		}
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

	SharedPtrArrayType p_main = make_shared_array_func(dist_array_size(gen));
	WeakPtrArrayType   wp(p_main);

	volatile int dummy_sum = 0;
	auto		 start	   = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < operations_per_trial; ++j) {
		if (j % 2 == 0) {
			p_main.reset();
		} else if (!p_main) {
			p_main = make_shared_array_func(dist_array_size(gen));
			wp	   = p_main;
		}
		SharedPtrArrayType locked_ptr = wp.lock();
		if (locked_ptr && p_main && dist_array_size(gen) > 0) {
			dummy_sum += locked_ptr[0].id;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	(void)dummy_sum;
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

#endif // SMARTPOINTERS_BENCHMARK_WEAK_H