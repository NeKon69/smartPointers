//
// Created by progamers on 5/31/25.
//

#include "../include/benchmark_weak.h"

#include <iostream>

long long run_combined_stress_impl_weak(bool use_raw, int iterations, int max_pointers_in_pool) {
	std::random_device				rd;
	std::mt19937					gen(rd());
	std::uniform_int_distribution<> dist_op(0, 12);
	std::uniform_int_distribution<> dist_idx(0, max_pointers_in_pool - 1);
	std::uniform_int_distribution<> dist_val(0, 9999);
	std::uniform_int_distribution<> dist_array_size_gen(1, 10);
	std::uniform_int_distribution<> dist_reset_chance(0, 4);

	std::vector<std::shared_ptr<TestObject>>   std_shared_single_ptrs;
	std::vector<std::weak_ptr<TestObject>>	   std_weak_single_ptrs;
	std::vector<std::shared_ptr<TestObject[]>> std_shared_array_ptrs;
	std::vector<std::weak_ptr<TestObject[]>>   std_weak_array_ptrs;

	std::vector<raw::shared_ptr<TestObject>>   raw_shared_single_ptrs;
	std::vector<raw::weak_ptr<TestObject>>	   raw_weak_single_ptrs;
	std::vector<raw::shared_ptr<TestObject[]>> raw_shared_array_ptrs;
	std::vector<raw::weak_ptr<TestObject[]>>   raw_weak_array_ptrs;

	std::vector<size_t> array_actual_sizes(max_pointers_in_pool, 0);

	if (use_raw) {
		raw_shared_single_ptrs.resize(max_pointers_in_pool);
		raw_weak_single_ptrs.resize(max_pointers_in_pool);
		raw_shared_array_ptrs.resize(max_pointers_in_pool);
		raw_weak_array_ptrs.resize(max_pointers_in_pool);
	} else {
		std_shared_single_ptrs.resize(max_pointers_in_pool);
		std_weak_single_ptrs.resize(max_pointers_in_pool);
		std_shared_array_ptrs.resize(max_pointers_in_pool);
		std_weak_array_ptrs.resize(max_pointers_in_pool);
	}

	for (int i = 0; i < max_pointers_in_pool; ++i) {
		if (use_raw) {
			raw_shared_single_ptrs[i] = raw::make_shared<TestObject>(dist_val(gen));
			size_t current_array_size = dist_array_size_gen(gen);
			raw_shared_array_ptrs[i]  = raw::make_shared<TestObject[]>(current_array_size);
			array_actual_sizes[i]	  = current_array_size;

			raw_weak_single_ptrs[i] = raw_shared_single_ptrs[i];
			raw_weak_array_ptrs[i]	= raw_shared_array_ptrs[i];
		} else {
			std_shared_single_ptrs[i] = std::make_shared<TestObject>(dist_val(gen));
			size_t current_array_size = dist_array_size_gen(gen);
			std_shared_array_ptrs[i]  = std::make_shared<TestObject[]>(current_array_size);
			array_actual_sizes[i]	  = current_array_size;

			std_weak_single_ptrs[i] = std_shared_single_ptrs[i];
			std_weak_array_ptrs[i]	= std_shared_array_ptrs[i];
		}
	}

	auto start_time = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < iterations; ++i) {
		int op	 = dist_op(gen);
		int idx1 = dist_idx(gen);
		int idx2 = dist_idx(gen);

		if (use_raw) {
			switch (op) {
			case 0:
				raw_shared_single_ptrs[idx1] = raw::make_shared<TestObject>(dist_val(gen));
				raw_weak_single_ptrs[idx1]	 = raw_shared_single_ptrs[idx1];
				break;
			case 1:
				raw_weak_single_ptrs[idx1] = raw_weak_single_ptrs[idx2];
				break;
			case 2:
				raw_weak_single_ptrs[idx1] = std::move(raw_weak_single_ptrs[idx2]);
				break;
			case 3:
				raw_weak_single_ptrs[idx1].reset();
				break;
			case 4:
				raw_weak_single_ptrs[idx1].swap(raw_weak_single_ptrs[idx2]);
				break;
			case 5: {
				volatile size_t count = raw_weak_single_ptrs[idx1].use_count();
				(void)count;
			} break;
			case 6: {
				volatile bool exp = raw_weak_single_ptrs[idx1].expired();
				(void)exp;
			} break;
			case 7: {
				raw::shared_ptr<TestObject> locked_ptr = raw_weak_single_ptrs[idx1].lock();
				if (locked_ptr) {
					volatile int id = locked_ptr->id;
					(void)id;
				}
			} break;
			case 8: {
				int							  array_idx	 = dist_idx(gen);
				raw::shared_ptr<TestObject[]> locked_ptr = raw_weak_array_ptrs[array_idx].lock();
				if (locked_ptr && array_actual_sizes[array_idx] > 0) {
					volatile int id = locked_ptr[0].id;
					(void)id;
				}
			} break;
			case 9:
				if (raw_shared_single_ptrs[idx1] && dist_reset_chance(gen) < 2) {
					raw_shared_single_ptrs[idx1].reset();
				}
				break;
			case 10:
				if (raw_shared_array_ptrs[idx1] && dist_reset_chance(gen) < 2) {
					raw_shared_array_ptrs[idx1].reset();
					array_actual_sizes[idx1] = 0;
				}
				break;
			case 11: {
				raw::shared_ptr<TestObject> temp_sp = raw::make_shared<TestObject>(dist_val(gen));
				raw_weak_single_ptrs[idx1]			= temp_sp;
				if (dist_reset_chance(gen) < 1) {
					temp_sp.reset();
				}
				volatile bool exp_after_reset = raw_weak_single_ptrs[idx1].expired();
				(void)exp_after_reset;
			} break;
			case 12: {
				raw_weak_single_ptrs[idx1] = raw_shared_single_ptrs[idx2];
				if (dist_reset_chance(gen) < 2) {
					raw_shared_single_ptrs[idx2].reset();
				}
				raw::shared_ptr<TestObject> locked_after_reset = raw_weak_single_ptrs[idx1].lock();
				if (locked_after_reset) {
					volatile int id = locked_after_reset->id;
					(void)id;
				}
			} break;
			default:
				break;
			}
		} else {
			switch (op) {
			case 0:
				std_shared_single_ptrs[idx1] = std::make_shared<TestObject>(dist_val(gen));
				std_weak_single_ptrs[idx1]	 = std_shared_single_ptrs[idx1];
				break;
			case 1:
				std_weak_single_ptrs[idx1] = std_weak_single_ptrs[idx2];
				break;
			case 2:
				std_weak_single_ptrs[idx1] = std::move(std_weak_single_ptrs[idx2]);
				break;
			case 3:
				std_weak_single_ptrs[idx1].reset();
				break;
			case 4:
				std_weak_single_ptrs[idx1].swap(std_weak_single_ptrs[idx2]);
				break;
			case 5: {
				volatile size_t count = std_weak_single_ptrs[idx1].use_count();
				(void)count;
			} break;
			case 6: {
				volatile bool exp = std_weak_single_ptrs[idx1].expired();
				(void)exp;
			} break;
			case 7: {
				std::shared_ptr<TestObject> locked_ptr = std_weak_single_ptrs[idx1].lock();
				if (locked_ptr) {
					volatile int id = locked_ptr->id;
					(void)id;
				}
			} break;
			case 8: {
				int							  array_idx	 = dist_idx(gen);
				std::shared_ptr<TestObject[]> locked_ptr = std_weak_array_ptrs[array_idx].lock();
				if (locked_ptr && array_actual_sizes[array_idx] > 0) {
					volatile int id = locked_ptr[0].id;
					(void)id;
				}
			} break;
			case 9:
				if (std_shared_single_ptrs[idx1] && dist_reset_chance(gen) < 2) {
					std_shared_single_ptrs[idx1].reset();
				}
				break;
			case 10:
				if (std_shared_array_ptrs[idx1] && dist_reset_chance(gen) < 2) {
					std_shared_array_ptrs[idx1].reset();
					array_actual_sizes[idx1] = 0;
				}
				break;
			case 11: {
				std::shared_ptr<TestObject> temp_sp = std::make_shared<TestObject>(dist_val(gen));
				std_weak_single_ptrs[idx1]			= temp_sp;
				if (dist_reset_chance(gen) < 1) {
					temp_sp.reset();
				}
				volatile bool exp_after_reset = std_weak_single_ptrs[idx1].expired();
				(void)exp_after_reset;
			} break;
			case 12: {
				std_weak_single_ptrs[idx1] = std_shared_single_ptrs[idx2];
				if (dist_reset_chance(gen) < 2) {
					std_shared_single_ptrs[idx2].reset();
				}
				std::shared_ptr<TestObject> locked_after_reset = std_weak_single_ptrs[idx1].lock();
				if (locked_after_reset) {
					volatile int id = locked_after_reset->id;
					(void)id;
				}
			} break;
			default:
				break;
			}
		}
	}

	auto end_time = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
}

void performance_comparison_weak_test() {
	std::cout << "\n--- Performance Comparison Test: raw::weak_ptr vs std::weak_ptr ---\n";

	const int NUM_TRIALS		= 20;
	const int OPS_PER_TRIAL		= 1000000;
	const int STRESS_ITERATIONS = 1000000;
	const int POOL_SIZE			= 100;

	print_table_header();

	int initial_active_objects_before_test = s_active_test_objects;

	auto std_make_shared_single = [](int val) {
		return std::make_shared<TestObject>(val);
	};
	auto raw_make_shared_single = [](int val) {
		return raw::make_shared<TestObject>(val);
	};
	auto std_make_shared_array = [](size_t s) {
		return std::make_shared<TestObject[]>(s);
	};
	auto raw_make_shared_array = [](size_t s) {
		return raw::make_shared<TestObject[]>(s);
	};

	TestResults from_shared_single_results = run_benchmark_scenario(
		"Weak from Shared Creation (Single)", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_weak_from_shared_creation_test<std::weak_ptr<TestObject>,
													  std::shared_ptr<TestObject>>(
				ops, std_make_shared_single);
		},
		[&](int ops) {
			return run_weak_from_shared_creation_test<raw::weak_ptr<TestObject>,
													  raw::shared_ptr<TestObject>>(
				ops, raw_make_shared_single);
		});
	print_table_row("Weak from Shared Creation (Single)", from_shared_single_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Weak from Shared Creation (Single)", initial_active_objects_before_test);

	TestResults from_shared_array_results = run_benchmark_scenario(
		"Weak from Shared Creation (Array)", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_weak_array_from_shared_creation_test<std::weak_ptr<TestObject[]>,
															std::shared_ptr<TestObject[]>>(
				ops, std_make_shared_array);
		},
		[&](int ops) {
			return run_weak_array_from_shared_creation_test<raw::weak_ptr<TestObject[]>,
															raw::shared_ptr<TestObject[]>>(
				ops, raw_make_shared_array);
		});
	print_table_row("Weak from Shared Creation (Array)", from_shared_array_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Weak from Shared Creation (Array)", initial_active_objects_before_test);

	TestResults copy_construct_results = run_benchmark_scenario(
		"Weak Copy Construct", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_weak_copy_construct_test<std::weak_ptr<TestObject>,
												std::shared_ptr<TestObject>>(
				ops, std_make_shared_single);
		},
		[&](int ops) {
			return run_weak_copy_construct_test<raw::weak_ptr<TestObject>,
												raw::shared_ptr<TestObject>>(
				ops, raw_make_shared_single);
		});
	print_table_row("Weak Copy Construct", copy_construct_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Weak Copy Construct", initial_active_objects_before_test);

	TestResults move_construct_results = run_benchmark_scenario(
		"Weak Move Construct", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_weak_move_construct_test<std::weak_ptr<TestObject>,
												std::shared_ptr<TestObject>>(
				ops, std_make_shared_single);
		},
		[&](int ops) {
			return run_weak_move_construct_test<raw::weak_ptr<TestObject>,
												raw::shared_ptr<TestObject>>(
				ops, raw_make_shared_single);
		});
	print_table_row("Weak Move Construct", move_construct_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Weak Move Construct", initial_active_objects_before_test);

	TestResults copy_assign_results = run_benchmark_scenario(
		"Weak Copy Assign", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_weak_copy_assign_test<std::weak_ptr<TestObject>,
											 std::shared_ptr<TestObject>>(ops,
																		  std_make_shared_single);
		},
		[&](int ops) {
			return run_weak_copy_assign_test<raw::weak_ptr<TestObject>,
											 raw::shared_ptr<TestObject>>(ops,
																		  raw_make_shared_single);
		});
	print_table_row("Weak Copy Assign", copy_assign_results, initial_active_objects_before_test,
					s_active_test_objects);
	verify_active_objects("Weak Copy Assign", initial_active_objects_before_test);

	TestResults move_assign_results = run_benchmark_scenario(
		"Weak Move Assign", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_weak_move_assign_test<std::weak_ptr<TestObject>,
											 std::shared_ptr<TestObject>>(ops,
																		  std_make_shared_single);
		},
		[&](int ops) {
			return run_weak_move_assign_test<raw::weak_ptr<TestObject>,
											 raw::shared_ptr<TestObject>>(ops,
																		  raw_make_shared_single);
		});
	print_table_row("Weak Move Assign", move_assign_results, initial_active_objects_before_test,
					s_active_test_objects);
	verify_active_objects("Weak Move Assign", initial_active_objects_before_test);

	TestResults shared_assign_single_results = run_benchmark_scenario(
		"Weak Shared Assign (Single)", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_weak_shared_assign_test<std::weak_ptr<TestObject>,
											   std::shared_ptr<TestObject>>(ops,
																			std_make_shared_single);
		},
		[&](int ops) {
			return run_weak_shared_assign_test<raw::weak_ptr<TestObject>,
											   raw::shared_ptr<TestObject>>(ops,
																			raw_make_shared_single);
		});
	print_table_row("Weak Shared Assign (Single)", shared_assign_single_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Weak Shared Assign (Single)", initial_active_objects_before_test);

	TestResults shared_assign_array_results = run_benchmark_scenario(
		"Weak Shared Assign (Array)", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_weak_array_shared_assign_test<std::weak_ptr<TestObject[]>,
													 std::shared_ptr<TestObject[]>>(
				ops, std_make_shared_array);
		},
		[&](int ops) {
			return run_weak_array_shared_assign_test<raw::weak_ptr<TestObject[]>,
													 raw::shared_ptr<TestObject[]>>(
				ops, raw_make_shared_array);
		});
	print_table_row("Weak Shared Assign (Array)", shared_assign_array_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Weak Shared Assign (Array)", initial_active_objects_before_test);

	TestResults reset_results = run_benchmark_scenario(
		"Weak Reset", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_weak_reset_test<std::weak_ptr<TestObject>, std::shared_ptr<TestObject>>(
				ops, std_make_shared_single);
		},
		[&](int ops) {
			return run_weak_reset_test<raw::weak_ptr<TestObject>, raw::shared_ptr<TestObject>>(
				ops, raw_make_shared_single);
		});
	print_table_row("Weak Reset", reset_results, initial_active_objects_before_test,
					s_active_test_objects);
	verify_active_objects("Weak Reset", initial_active_objects_before_test);

	TestResults swap_results = run_benchmark_scenario(
		"Weak Swap", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_weak_swap_test<std::weak_ptr<TestObject>, std::shared_ptr<TestObject>>(
				ops, std_make_shared_single);
		},
		[&](int ops) {
			return run_weak_swap_test<raw::weak_ptr<TestObject>, raw::shared_ptr<TestObject>>(
				ops, raw_make_shared_single);
		});
	print_table_row("Weak Swap", swap_results, initial_active_objects_before_test,
					s_active_test_objects);
	verify_active_objects("Weak Swap", initial_active_objects_before_test);

	TestResults use_count_results = run_benchmark_scenario(
		"Weak Use Count", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_weak_use_count_test<std::weak_ptr<TestObject>, std::shared_ptr<TestObject>>(
				ops, std_make_shared_single);
		},
		[&](int ops) {
			return run_weak_use_count_test<raw::weak_ptr<TestObject>, raw::shared_ptr<TestObject>>(
				ops, raw_make_shared_single);
		});
	print_table_row("Weak Use Count", use_count_results, initial_active_objects_before_test,
					s_active_test_objects);
	verify_active_objects("Weak Use Count", initial_active_objects_before_test);

	TestResults expired_results = run_benchmark_scenario(
		"Weak Expired", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_weak_expired_test<std::weak_ptr<TestObject>, std::shared_ptr<TestObject>>(
				ops, std_make_shared_single);
		},
		[&](int ops) {
			return run_weak_expired_test<raw::weak_ptr<TestObject>, raw::shared_ptr<TestObject>>(
				ops, raw_make_shared_single);
		});
	print_table_row("Weak Expired", expired_results, initial_active_objects_before_test,
					s_active_test_objects);
	verify_active_objects("Weak Expired", initial_active_objects_before_test);

	TestResults lock_single_results = run_benchmark_scenario(
		"Weak Lock (Single)", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_weak_lock_test<std::weak_ptr<TestObject>, std::shared_ptr<TestObject>>(
				ops, std_make_shared_single);
		},
		[&](int ops) {
			return run_weak_lock_test<raw::weak_ptr<TestObject>, raw::shared_ptr<TestObject>>(
				ops, raw_make_shared_single);
		});
	print_table_row("Weak Lock (Single)", lock_single_results, initial_active_objects_before_test,
					s_active_test_objects);
	verify_active_objects("Weak Lock (Single)", initial_active_objects_before_test);

	TestResults lock_array_results = run_benchmark_scenario(
		"Weak Lock (Array)", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_weak_array_lock_test<std::weak_ptr<TestObject[]>,
											std::shared_ptr<TestObject[]>>(ops,
																		   std_make_shared_array);
		},
		[&](int ops) {
			return run_weak_array_lock_test<raw::weak_ptr<TestObject[]>,
											raw::shared_ptr<TestObject[]>>(ops,
																		   raw_make_shared_array);
		});
	print_table_row("Weak Lock (Array)", lock_array_results, initial_active_objects_before_test,
					s_active_test_objects);
	verify_active_objects("Weak Lock (Array)", initial_active_objects_before_test);

	TestResults combined_stress_results = run_benchmark_scenario(
		"Combined Stress Test (Weak)", NUM_TRIALS, STRESS_ITERATIONS,
		[&](int ops) {
			return run_combined_stress_impl_weak(false, ops, POOL_SIZE);
		},
		[&](int ops) {
			return run_combined_stress_impl_weak(true, ops, POOL_SIZE);
		});
	print_table_row("Combined Stress Test (Weak)", combined_stress_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Combined Stress Test (Weak)", initial_active_objects_before_test);

	std::cout
		<< "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
	std::cout << "Performance comparison finished.\n";
}