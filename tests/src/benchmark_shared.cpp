//
// Created by progamers on 5/31/25.
//

#include "../include/benchmark_shared.h"

#include <iostream>

long long run_combined_stress_impl_shared(bool use_raw, int iterations, int max_pointers_in_pool) {
	std::random_device				rd;
	std::mt19937					gen(rd());
	std::uniform_int_distribution<> dist_op(0, 16);
	std::uniform_int_distribution<> dist_idx(0, max_pointers_in_pool - 1);
	std::uniform_int_distribution<> dist_val(0, 9999);
	std::uniform_int_distribution<> dist_array_size_gen(1, 10);
	std::uniform_int_distribution<> dist_extra_op_chance(0, 9);

	std::vector<std::shared_ptr<TestObject>>   std_single_ptrs;
	std::vector<std::shared_ptr<TestObject[]>> std_array_ptrs;
	std::vector<raw::shared_ptr<TestObject>>   raw_single_ptrs;
	std::vector<raw::shared_ptr<TestObject[]>> raw_array_ptrs;

	std::vector<size_t> array_actual_sizes(max_pointers_in_pool, 0);

	if (use_raw) {
		raw_single_ptrs.resize(max_pointers_in_pool);
		raw_array_ptrs.resize(max_pointers_in_pool);
	} else {
		std_single_ptrs.resize(max_pointers_in_pool);
		std_array_ptrs.resize(max_pointers_in_pool);
	}

	auto start_time = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < iterations; ++i) {
		int op	 = dist_op(gen);
		int idx1 = dist_idx(gen);
		int idx2 = dist_idx(gen);

		if (use_raw) {
			switch (op) {
			case 0:
				raw_single_ptrs[idx1] = raw::make_shared<TestObject>(dist_val(gen));
				if (raw_single_ptrs[idx1] && dist_extra_op_chance(gen) < 1) {
					volatile size_t count = raw_single_ptrs[idx1].use_count();
					(void)count;
				}
				break;
			case 1:
				raw_single_ptrs[idx1].reset();
				if (dist_extra_op_chance(gen) < 1) {
					volatile bool is_null = (raw_single_ptrs[idx1].get() == nullptr);
					(void)is_null;
				}
				break;
			case 2:
				if (raw_single_ptrs[idx2]) {
					raw_single_ptrs[idx1] = raw_single_ptrs[idx2];
					if (dist_extra_op_chance(gen) < 1) {
						volatile size_t count = raw_single_ptrs[idx1].use_count();
						(void)count;
					}
				} else {
					raw_single_ptrs[idx1].reset();
				}
				break;
			case 3:
				if (raw_single_ptrs[idx2]) {
					raw_single_ptrs[idx1] = std::move(raw_single_ptrs[idx2]);
					if (dist_extra_op_chance(gen) < 1) {
						volatile bool was_moved_from = (raw_single_ptrs[idx2].get() == nullptr);
						(void)was_moved_from;
					}
				} else {
					raw_single_ptrs[idx1].reset();
				}
				break;
			case 4:
				raw_single_ptrs[idx1] = nullptr;
				break;
			case 5:
				raw_single_ptrs[idx1].swap(raw_single_ptrs[idx2]);
				break;
			case 6:
				raw_single_ptrs[idx1].reset(new TestObject(dist_val(gen)));
				break;
			case 7:
				if (raw_single_ptrs[idx1]) {
					volatile int id = raw_single_ptrs[idx1]->id;
					(void)id;
				}
				break;
			case 8:
				if (raw_single_ptrs[idx1]) {
					volatile TestObject* p = raw_single_ptrs[idx1].get();
					(void)p;
				}
				break;
			case 9: {
				size_t current_array_size = dist_array_size_gen(gen);
				raw_array_ptrs[idx1]	  = raw::make_shared<TestObject[]>(current_array_size);
				array_actual_sizes[idx1]  = current_array_size;
				if (raw_array_ptrs[idx1] && current_array_size > 0 &&
					dist_extra_op_chance(gen) < 1) {
					volatile int dummy_val = raw_array_ptrs[idx1][0].id;
					(void)dummy_val;
				}
			} break;
			case 10:
				raw_array_ptrs[idx1].reset();
				array_actual_sizes[idx1] = 0;
				if (dist_extra_op_chance(gen) < 1) {
					volatile bool is_null = (raw_array_ptrs[idx1].get() == nullptr);
					(void)is_null;
				}
				break;
			case 11:
				if (raw_array_ptrs[idx2]) {
					raw_array_ptrs[idx1]	 = raw_array_ptrs[idx2];
					array_actual_sizes[idx1] = array_actual_sizes[idx2];
					if (dist_extra_op_chance(gen) < 1) {
						volatile size_t count = raw_array_ptrs[idx1].use_count();
						(void)count;
					}
				} else {
					raw_array_ptrs[idx1].reset();
					array_actual_sizes[idx1] = 0;
				}
				break;
			case 12:
				raw_array_ptrs[idx1]	 = std::move(raw_array_ptrs[idx2]);
				array_actual_sizes[idx1] = array_actual_sizes[idx2];
				array_actual_sizes[idx2] = 0;
				if (dist_extra_op_chance(gen) < 1) {
					volatile bool was_moved_from = (raw_array_ptrs[idx2].get() == nullptr);
					(void)was_moved_from;
				}
				break;
			case 13:
				raw_array_ptrs[idx1].swap(raw_array_ptrs[idx2]);
				std::swap(array_actual_sizes[idx1], array_actual_sizes[idx2]);
				break;
			case 14:
				if (raw_single_ptrs[idx1]) {
					volatile size_t count = raw_single_ptrs[idx1].use_count();
					(void)count;
				}
				break;
			case 15:
				if (raw_single_ptrs[idx1]) {
					volatile bool is_unique = raw_single_ptrs[idx1].unique();
					(void)is_unique;
				}
				break;
			case 16: {
				int access_idx = dist_idx(gen);
				if (raw_array_ptrs[access_idx] && array_actual_sizes[access_idx] > 0) {
					size_t elem_idx = std::uniform_int_distribution<size_t>(
						0, array_actual_sizes[access_idx] - 1)(gen);
					raw_array_ptrs[access_idx][elem_idx].id = dist_val(gen);
					volatile int read_val = raw_array_ptrs[access_idx][elem_idx].id;
					(void)read_val;
				}
			} break;
			default:
				break;
			}
		} else {
			switch (op) {
			case 0:
				std_single_ptrs[idx1] = std::make_shared<TestObject>(dist_val(gen));
				if (std_single_ptrs[idx1] && dist_extra_op_chance(gen) < 1) {
					volatile size_t count = std_single_ptrs[idx1].use_count();
					(void)count;
				}
				break;
			case 1:
				std_single_ptrs[idx1].reset();
				if (dist_extra_op_chance(gen) < 1) {
					volatile bool is_null = (std_single_ptrs[idx1].get() == nullptr);
					(void)is_null;
				}
				break;
			case 2:
				if (std_single_ptrs[idx2]) {
					std_single_ptrs[idx1] = std_single_ptrs[idx2];
					if (dist_extra_op_chance(gen) < 1) {
						volatile size_t count = std_single_ptrs[idx1].use_count();
						(void)count;
					}
				} else {
					std_single_ptrs[idx1].reset();
				}
				break;
			case 3:
				if (std_single_ptrs[idx2]) {
					std_single_ptrs[idx1] = std::move(std_single_ptrs[idx2]);
					if (dist_extra_op_chance(gen) < 1) {
						volatile bool was_moved_from = (std_single_ptrs[idx2].get() == nullptr);
						(void)was_moved_from;
					}
				} else {
					std_single_ptrs[idx1].reset();
				}
				break;
			case 4:
				std_single_ptrs[idx1] = nullptr;
				break;
			case 5:
				std_single_ptrs[idx1].swap(std_single_ptrs[idx2]);
				break;
			case 6:
				std_single_ptrs[idx1].reset(new TestObject(dist_val(gen)));
				break;
			case 7:
				if (std_single_ptrs[idx1]) {
					volatile int id = std_single_ptrs[idx1]->id;
					(void)id;
				}
				break;
			case 8:
				if (std_single_ptrs[idx1]) {
					volatile TestObject* p = std_single_ptrs[idx1].get();
					(void)p;
				}
				break;
			case 9: {
				size_t current_array_size = dist_array_size_gen(gen);
				std_array_ptrs[idx1]	  = std::make_shared<TestObject[]>(current_array_size);
				array_actual_sizes[idx1]  = current_array_size;
				if (std_array_ptrs[idx1] && current_array_size > 0 &&
					dist_extra_op_chance(gen) < 1) {
					volatile int dummy_val = std_array_ptrs[idx1][0].id;
					(void)dummy_val;
				}
			} break;
			case 10:
				std_array_ptrs[idx1].reset();
				array_actual_sizes[idx1] = 0;
				if (dist_extra_op_chance(gen) < 1) {
					volatile bool is_null = (std_array_ptrs[idx1].get() == nullptr);
					(void)is_null;
				}
				break;
			case 11:
				if (std_array_ptrs[idx2]) {
					std_array_ptrs[idx1]	 = std_array_ptrs[idx2];
					array_actual_sizes[idx1] = array_actual_sizes[idx2];
					if (dist_extra_op_chance(gen) < 1) {
						volatile size_t count = std_array_ptrs[idx1].use_count();
						(void)count;
					}
				} else {
					std_array_ptrs[idx1].reset();
					array_actual_sizes[idx1] = 0;
				}
				break;
			case 12:
				std_array_ptrs[idx1]	 = std::move(std_array_ptrs[idx2]);
				array_actual_sizes[idx1] = array_actual_sizes[idx2];
				array_actual_sizes[idx2] = 0;
				if (dist_extra_op_chance(gen) < 1) {
					volatile bool was_moved_from = (std_array_ptrs[idx2].get() == nullptr);
					(void)was_moved_from;
				}
				break;
			case 13:
				std_array_ptrs[idx1].swap(std_array_ptrs[idx2]);
				std::swap(array_actual_sizes[idx1], array_actual_sizes[idx2]);
				break;
			case 14:
				if (std_single_ptrs[idx1]) {
					volatile size_t count = std_single_ptrs[idx1].use_count();
					(void)count;
				}
				break;
			case 15:
				if (std_single_ptrs[idx1]) {
					volatile bool is_unique = std_single_ptrs[idx1].unique();
					(void)is_unique;
				}
				break;
			case 16: {
				int access_idx = dist_idx(gen);
				if (std_array_ptrs[access_idx] && array_actual_sizes[access_idx] > 0) {
					size_t elem_idx = std::uniform_int_distribution<size_t>(
						0, array_actual_sizes[access_idx] - 1)(gen);
					std_array_ptrs[access_idx][elem_idx].id = dist_val(gen);
					volatile int read_val = std_array_ptrs[access_idx][elem_idx].id;
					(void)read_val;
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

void performance_comparison_shared_test() {
	std::cout << "\n--- Performance Comparison Test: raw::shared_ptr vs std::shared_ptr ---\n";

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

	TestResults make_single_results = run_benchmark_scenario(
		"Make Single Object (make_shared)", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_shared_single_obj_creation_test<std::shared_ptr<TestObject>>(
				ops, std_make_shared_single);
		},
		[&](int ops) {
			return run_shared_single_obj_creation_test<raw::shared_ptr<TestObject>>(
				ops, raw_make_shared_single);
		});
	print_table_row("Make Single Object (make_shared)", make_single_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Make Single Object", initial_active_objects_before_test);

	TestResults make_array_results = run_benchmark_scenario(
		"Make Array (make_shared, size 1-10)", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_shared_array_creation_test<std::shared_ptr<TestObject[]>>(
				ops, std_make_shared_array);
		},
		[&](int ops) {
			return run_shared_array_creation_test<raw::shared_ptr<TestObject[]>>(
				ops, raw_make_shared_array);
		});
	print_table_row("Make Array (make_shared, size 1-10)", make_array_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Make Array", initial_active_objects_before_test);

	TestResults reset_single_results = run_benchmark_scenario(
		"Reset Single Object", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_shared_reset_single_test<std::shared_ptr<TestObject>>(
				ops, std_make_shared_single);
		},
		[&](int ops) {
			return run_shared_reset_single_test<raw::shared_ptr<TestObject>>(
				ops, raw_make_shared_single);
		});
	print_table_row("Reset Single Object", reset_single_results, initial_active_objects_before_test,
					s_active_test_objects);
	verify_active_objects("Reset Single Object", initial_active_objects_before_test);

	TestResults reset_array_results = run_benchmark_scenario(
		"Reset Array", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_shared_reset_array_test<std::shared_ptr<TestObject[]>>(
				ops, std_make_shared_array);
		},
		[&](int ops) {
			return run_shared_reset_array_test<raw::shared_ptr<TestObject[]>>(
				ops, raw_make_shared_array);
		});
	print_table_row("Reset Array", reset_array_results, initial_active_objects_before_test,
					s_active_test_objects);
	verify_active_objects("Reset Array", initial_active_objects_before_test);

	TestResults move_construct_single_results = run_benchmark_scenario(
		"Move Construct Single", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_shared_move_construct_single_test<std::shared_ptr<TestObject>>(
				ops, std_make_shared_single);
		},
		[&](int ops) {
			return run_shared_move_construct_single_test<raw::shared_ptr<TestObject>>(
				ops, raw_make_shared_single);
		});
	print_table_row("Move Construct Single", move_construct_single_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Move Construct Single", initial_active_objects_before_test);

	TestResults move_assign_single_results = run_benchmark_scenario(
		"Move Assign Single", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_shared_move_assign_single_test<std::shared_ptr<TestObject>>(
				ops, std_make_shared_single);
		},
		[&](int ops) {
			return run_shared_move_assign_single_test<raw::shared_ptr<TestObject>>(
				ops, raw_make_shared_single);
		});
	print_table_row("Move Assign Single", move_assign_single_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Move Assign Single", initial_active_objects_before_test);

	TestResults move_assign_array_results = run_benchmark_scenario(
		"Move Assign Array", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_shared_move_assign_array_test<std::shared_ptr<TestObject[]>>(
				ops, std_make_shared_array);
		},
		[&](int ops) {
			return run_shared_move_assign_array_test<raw::shared_ptr<TestObject[]>>(
				ops, raw_make_shared_array);
		});
	print_table_row("Move Assign Array", move_assign_array_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Move Assign Array", initial_active_objects_before_test);

	TestResults access_single_results = run_benchmark_scenario(
		"Access Single Object", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_shared_access_single_test<std::shared_ptr<TestObject>>(
				ops, std_make_shared_single);
		},
		[&](int ops) {
			return run_shared_access_single_test<raw::shared_ptr<TestObject>>(
				ops, raw_make_shared_single);
		});
	print_table_row("Access Single Object", access_single_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Access Single Object", initial_active_objects_before_test);

	TestResults access_array_elem_results = run_benchmark_scenario(
		"Access Array Element", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_shared_access_array_element_test<std::shared_ptr<TestObject[]>>(
				ops, std_make_shared_array);
		},
		[&](int ops) {
			return run_shared_access_array_element_test<raw::shared_ptr<TestObject[]>>(
				ops, raw_make_shared_array);
		});
	print_table_row("Access Array Element", access_array_elem_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Access Array Element", initial_active_objects_before_test);

	TestResults copy_construct_single_results = run_benchmark_scenario(
		"Copy Construct Single", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_shared_copy_construct_single_test<std::shared_ptr<TestObject>>(
				ops, std_make_shared_single);
		},
		[&](int ops) {
			return run_shared_copy_construct_single_test<raw::shared_ptr<TestObject>>(
				ops, raw_make_shared_single);
		});
	print_table_row("Copy Construct Single", copy_construct_single_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Copy Construct Single", initial_active_objects_before_test);

	TestResults copy_assign_single_results = run_benchmark_scenario(
		"Copy Assign Single", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_shared_copy_assign_single_test<std::shared_ptr<TestObject>>(
				ops, std_make_shared_single);
		},
		[&](int ops) {
			return run_shared_copy_assign_single_test<raw::shared_ptr<TestObject>>(
				ops, raw_make_shared_single);
		});
	print_table_row("Copy Assign Single", copy_assign_single_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Copy Assign Single", initial_active_objects_before_test);

	TestResults copy_assign_array_results = run_benchmark_scenario(
		"Copy Assign Array", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_shared_copy_assign_array_test<std::shared_ptr<TestObject[]>>(
				ops, std_make_shared_array);
		},
		[&](int ops) {
			return run_shared_copy_assign_array_test<raw::shared_ptr<TestObject[]>>(
				ops, raw_make_shared_array);
		});
	print_table_row("Copy Assign Array", copy_assign_array_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Copy Assign Array", initial_active_objects_before_test);

	TestResults use_count_results = run_benchmark_scenario(
		"Use Count", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_shared_use_count_test<std::shared_ptr<TestObject>>(ops,
																		  std_make_shared_single);
		},
		[&](int ops) {
			return run_shared_use_count_test<raw::shared_ptr<TestObject>>(ops,
																		  raw_make_shared_single);
		});
	print_table_row("Use Count", use_count_results, initial_active_objects_before_test,
					s_active_test_objects);
	verify_active_objects("Use Count", initial_active_objects_before_test);

	TestResults unique_results = run_benchmark_scenario(
		"Unique Check", NUM_TRIALS, OPS_PER_TRIAL,
		[&](int ops) {
			return run_shared_unique_test<std::shared_ptr<TestObject>>(ops, std_make_shared_single);
		},
		[&](int ops) {
			return run_shared_unique_test<raw::shared_ptr<TestObject>>(ops, raw_make_shared_single);
		});
	print_table_row("Unique Check", unique_results, initial_active_objects_before_test,
					s_active_test_objects);
	verify_active_objects("Unique Check", initial_active_objects_before_test);

	TestResults combined_stress_results = run_benchmark_scenario(
		"Combined Stress Test (Shared)", NUM_TRIALS, STRESS_ITERATIONS,
		[&](int ops) {
			return run_combined_stress_impl_shared(false, ops, POOL_SIZE);
		},
		[&](int ops) {
			return run_combined_stress_impl_shared(true, ops, POOL_SIZE);
		});
	print_table_row("Combined Stress Test (Shared)", combined_stress_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Combined Stress Test (Shared)", initial_active_objects_before_test);

	std::cout
		<< "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
	std::cout << "Performance comparison finished.\n";
}