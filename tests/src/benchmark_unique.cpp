//
// Created by progamers on 5/30/25.
//

#include "../include/benchmark_unique.h"

void calculate_stats(const std::vector<long long>& durations, long long& min_val,
					 long long& max_val, long long& avg_val) {
	if (durations.empty()) {
		min_val = max_val = avg_val = 0;
		return;
	}
	min_val		  = durations[0];
	max_val		  = durations[0];
	long long sum = 0;
	for (long long d : durations) {
		if (d < min_val)
			min_val = d;
		if (d > max_val)
			max_val = d;
		sum += d;
	}
	avg_val = sum / durations.size();
}

void print_table_header() {
	std::cout
		<< "\n----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
	std::cout << std::left << std::setw(30) << "Scenario";
	std::cout << "| " << std::right << std::setw(28) << "STD (us)";
	std::cout << " | " << std::right << std::setw(28) << "RAW (us)";
	std::cout << " | " << std::right << std::setw(34) << "Comparison";
	std::cout << " | " << std::right << std::setw(24) << "Active Objects Start/End";
	std::cout << "\n";
	std::cout << std::left << std::setw(30) << "";
	std::cout << "| " << std::right << std::setw(7) << "Min" << std::setw(9) << "Max"
			  << std::setw(12) << "Avg";
	std::cout << " | " << std::right << std::setw(7) << "Min" << std::setw(9) << "Max"
			  << std::setw(12) << "Avg";
	std::cout << " | " << std::right << std::setw(34) << "RAW vs STD Avg (%)";
	std::cout << " | " << std::right << std::setw(12) << "Pre-test" << std::setw(12) << "Post-test";
	std::cout << "\n";
	std::cout
		<< "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
}

void print_table_row(const std::string& scenario_name, const TestResults& results,
					 int initial_active_objects, int final_active_objects) {
	std::cout << std::fixed << std::setprecision(2);
	std::cout << std::left << std::setw(30) << scenario_name;

	std::cout << "| " << std::right << std::setw(7) << results.std_min_us << std::setw(9)
			  << results.std_max_us << std::setw(12) << results.std_avg_us;

	std::cout << " | " << std::right << std::setw(7) << results.raw_min_us << std::setw(9)
			  << results.raw_max_us << std::setw(12) << results.raw_avg_us;

	std::cout << " | " << std::right;
	std::cout << std::showpos << std::setw(30) << results.raw_vs_std_avg_percent;
	std::cout << std::noshowpos;

	std::cout << " | " << std::right << std::setw(12) << initial_active_objects << std::setw(12)
			  << final_active_objects << "\n";
}

TestResults run_benchmark_scenario(const std::string& scenario_name, int num_trials,
								   int operations_per_trial, std::function<long long(int)> std_func,
								   std::function<long long(int)> raw_func) {
	std::vector<long long> std_durations_us;
	std_durations_us.reserve(num_trials);
	for (int i = 0; i < num_trials; ++i) {
		std_durations_us.push_back(std_func(operations_per_trial));
	}

	std::vector<long long> raw_durations_us;
	raw_durations_us.reserve(num_trials);
	for (int i = 0; i < num_trials; ++i) {
		raw_durations_us.push_back(raw_func(operations_per_trial));
	}

	TestResults results;
	calculate_stats(std_durations_us, results.std_min_us, results.std_max_us, results.std_avg_us);
	calculate_stats(raw_durations_us, results.raw_min_us, results.raw_max_us, results.raw_avg_us);

	if (results.std_avg_us != 0) {
		results.raw_vs_std_avg_percent =
			((double)results.raw_avg_us - results.std_avg_us) / results.std_avg_us * 100.0;
	} else {
		results.raw_vs_std_avg_percent = 0.0;
	}
	(void)scenario_name;
	return results;
}

long long run_combined_stress_impl(bool use_raw, int iterations, int max_pointers_in_pool) {
	std::random_device				rd;
	std::mt19937					gen(rd());
	std::uniform_int_distribution<> dist_op(0, 13);
	std::uniform_int_distribution<> dist_idx(0, max_pointers_in_pool - 1);
	std::uniform_int_distribution<> dist_val(0, 9999);
	std::uniform_int_distribution<> dist_array_size_gen(1, 10);

	std::vector<std::unique_ptr<TestObject>>   std_single_ptrs;
	std::vector<std::unique_ptr<TestObject[]>> std_array_ptrs;
	std::vector<raw::unique_ptr<TestObject>>   raw_single_ptrs;
	std::vector<raw::unique_ptr<TestObject[]>> raw_array_ptrs;

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
				raw_single_ptrs[idx1] = raw::make_unique<TestObject>(dist_val(gen));
				break;
			case 1:
				raw_single_ptrs[idx1].reset();
				break;
			case 2:
				if (raw_single_ptrs[idx1]) {
					TestObject* released = raw_single_ptrs[idx1].release();
					delete released;
				}
				break;
			case 3:
				if (raw_single_ptrs[idx2]) {
					raw::unique_ptr<TestObject> tmp(std::move(raw_single_ptrs[idx2]));
					raw_single_ptrs[idx1] = std::move(tmp);
				}
				break;
			case 4:
				if (raw_single_ptrs[idx2]) {
					raw_single_ptrs[idx1] = std::move(raw_single_ptrs[idx2]);
				}
				break;
			case 5:
				raw_single_ptrs[idx1] = nullptr;
				break;
			case 6:
				raw_single_ptrs[idx1].swap(raw_single_ptrs[idx2]);
				break;
			case 7:
				raw_single_ptrs[idx1].reset(new TestObject(dist_val(gen)));
				break;
			case 8:
				if (raw_single_ptrs[idx1]) {
					volatile int id = raw_single_ptrs[idx1]->id;
					(void)id;
				}
				break;
			case 9: {
				size_t current_array_size = dist_array_size_gen(gen);
				raw_array_ptrs[idx1]	  = raw::make_unique<TestObject[]>(current_array_size);
				array_actual_sizes[idx1]  = current_array_size;
			} break;
			case 10:
				raw_array_ptrs[idx1].reset();
				array_actual_sizes[idx1] = 0;
				break;
			case 11:
				raw_array_ptrs[idx1]	 = std::move(raw_array_ptrs[idx2]);
				array_actual_sizes[idx1] = array_actual_sizes[idx2];
				array_actual_sizes[idx2] = 0;
				break;
			case 12:
				raw_array_ptrs[idx1].swap(raw_array_ptrs[idx2]);
				std::swap(array_actual_sizes[idx1], array_actual_sizes[idx2]);
				break;
			case 13:
				if (raw_array_ptrs[idx1]) {
					TestObject* released = raw_array_ptrs[idx1].release();
					delete[] released;
					array_actual_sizes[idx1] = 0;
				}
				break;
			default:
				break;
			}

			if (raw_array_ptrs[idx1] && array_actual_sizes[idx1] > 0) {
				size_t elem_idx =
					std::uniform_int_distribution<size_t>(0, array_actual_sizes[idx1] - 1)(gen);
				raw_array_ptrs[idx1][elem_idx].id = dist_val(gen);
				volatile int read_val			  = raw_array_ptrs[idx1][elem_idx].id;
				(void)read_val;
			}
		} else {
			switch (op) {
			case 0:
				std_single_ptrs[idx1] = std::make_unique<TestObject>(dist_val(gen));
				break;
			case 1:
				std_single_ptrs[idx1].reset();
				break;
			case 2:
				if (std_single_ptrs[idx1]) {
					TestObject* released = std_single_ptrs[idx1].release();
					delete released;
				}
				break;
			case 3:
				if (std_single_ptrs[idx2]) {
					std::unique_ptr<TestObject> tmp(std::move(std_single_ptrs[idx2]));
					std_single_ptrs[idx1] = std::move(tmp);
				}
				break;
			case 4:
				if (std_single_ptrs[idx2]) {
					std_single_ptrs[idx1] = std::move(std_single_ptrs[idx2]);
				}
				break;
			case 5:
				std_single_ptrs[idx1] = nullptr;
				break;
			case 6:
				std_single_ptrs[idx1].swap(std_single_ptrs[idx2]);
				break;
			case 7:
				std_single_ptrs[idx1].reset(new TestObject(dist_val(gen)));
				break;
			case 8:
				if (std_single_ptrs[idx1]) {
					volatile int id = std_single_ptrs[idx1]->id;
					(void)id;
				}
				break;
			case 9: {
				size_t current_array_size = dist_array_size_gen(gen);
				std_array_ptrs[idx1]	  = std::make_unique<TestObject[]>(current_array_size);
				array_actual_sizes[idx1]  = current_array_size;
			} break;
			case 10:
				std_array_ptrs[idx1].reset();
				array_actual_sizes[idx1] = 0;
				break;
			case 11:
				std_array_ptrs[idx1]	 = std::move(std_array_ptrs[idx2]);
				array_actual_sizes[idx1] = array_actual_sizes[idx2];
				array_actual_sizes[idx2] = 0;
				break;
			case 12:
				std_array_ptrs[idx1].swap(std_array_ptrs[idx2]);
				std::swap(array_actual_sizes[idx1], array_actual_sizes[idx2]);
				break;
			case 13:
				if (std_array_ptrs[idx1]) {
					TestObject* released = std_array_ptrs[idx1].release();
					delete[] released;
					array_actual_sizes[idx1] = 0;
				}
				break;
			default:
				break;
			}

			if (std_array_ptrs[idx1] && array_actual_sizes[idx1] > 0) {
				size_t elem_idx =
					std::uniform_int_distribution<size_t>(0, array_actual_sizes[idx1] - 1)(gen);
				std_array_ptrs[idx1][elem_idx].id = dist_val(gen);
				volatile int read_val			  = std_array_ptrs[idx1][elem_idx].id;
				(void)read_val;
			}
		}
	}

	auto end_time = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
}

void performance_comparison_unique_test() {
	std::cout << "\n--- Performance Comparison Test: raw::unique_ptr vs std::unique_ptr ---\n";

	const int NUM_TRIALS	= 20;
	const int OPS_PER_TRIAL = 1000000;

	print_table_header();

	int initial_active_objects_before_test = s_active_test_objects;

	TestResults make_single_results = run_benchmark_scenario(
		"Make Single Object", NUM_TRIALS, OPS_PER_TRIAL,
		[](int ops) {
			return run_single_obj_creation_test<std::unique_ptr<TestObject>>(ops, [](int val) {
				return std::make_unique<TestObject>(val);
			});
		},
		[](int ops) {
			return run_single_obj_creation_test<raw::unique_ptr<TestObject>>(ops, [](int val) {
				return raw::make_unique<TestObject>(val);
			});
		});
	print_table_row("Make Single Object", make_single_results, initial_active_objects_before_test,
					s_active_test_objects);
	verify_active_objects("Make Single Object", initial_active_objects_before_test);

	TestResults make_array_results = run_benchmark_scenario(
		"Make Array (size 1-10)", NUM_TRIALS, OPS_PER_TRIAL,
		[](int ops) {
			return run_array_creation_test<std::unique_ptr<TestObject[]>>(ops, [](size_t s) {
				return std::make_unique<TestObject[]>(s);
			});
		},
		[](int ops) {
			return run_array_creation_test<raw::unique_ptr<TestObject[]>>(ops, [](size_t s) {
				return raw::make_unique<TestObject[]>(s);
			});
		});
	print_table_row("Make Array (size 1-10)", make_array_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Make Array", initial_active_objects_before_test);

	TestResults reset_single_results = run_benchmark_scenario(
		"Reset Single Object", NUM_TRIALS, OPS_PER_TRIAL,
		[](int ops) {
			return run_reset_single_test<std::unique_ptr<TestObject>>(ops, [](int val) {
				return std::make_unique<TestObject>(val);
			});
		},
		[](int ops) {
			return run_reset_single_test<raw::unique_ptr<TestObject>>(ops, [](int val) {
				return raw::make_unique<TestObject>(val);
			});
		});
	print_table_row("Reset Single Object", reset_single_results, initial_active_objects_before_test,
					s_active_test_objects);
	verify_active_objects("Reset Single Object", initial_active_objects_before_test);

	TestResults reset_array_results = run_benchmark_scenario(
		"Reset Array", NUM_TRIALS, OPS_PER_TRIAL,
		[](int ops) {
			return run_reset_array_test<std::unique_ptr<TestObject[]>>(ops, [](size_t s) {
				return std::make_unique<TestObject[]>(s);
			});
		},
		[](int ops) {
			return run_reset_array_test<raw::unique_ptr<TestObject[]>>(ops, [](size_t s) {
				return raw::make_unique<TestObject[]>(s);
			});
		});
	print_table_row("Reset Array", reset_array_results, initial_active_objects_before_test,
					s_active_test_objects);
	verify_active_objects("Reset Array", initial_active_objects_before_test);

	TestResults move_construct_single_results = run_benchmark_scenario(
		"Move Construct Single", NUM_TRIALS, OPS_PER_TRIAL,
		[](int ops) {
			return run_move_construct_single_test<std::unique_ptr<TestObject>>(ops, [](int val) {
				return std::make_unique<TestObject>(val);
			});
		},
		[](int ops) {
			return run_move_construct_single_test<raw::unique_ptr<TestObject>>(ops, [](int val) {
				return raw::make_unique<TestObject>(val);
			});
		});
	print_table_row("Move Construct Single", move_construct_single_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Move Construct Single", initial_active_objects_before_test);

	TestResults move_assign_single_results = run_benchmark_scenario(
		"Move Assign Single", NUM_TRIALS, OPS_PER_TRIAL,
		[](int ops) {
			return run_move_assign_single_test<std::unique_ptr<TestObject>>(ops, [](int val) {
				return std::make_unique<TestObject>(val);
			});
		},
		[](int ops) {
			return run_move_assign_single_test<raw::unique_ptr<TestObject>>(ops, [](int val) {
				return raw::make_unique<TestObject>(val);
			});
		});
	print_table_row("Move Assign Single", move_assign_single_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Move Assign Single", initial_active_objects_before_test);

	TestResults move_assign_array_results = run_benchmark_scenario(
		"Move Assign Array", NUM_TRIALS, OPS_PER_TRIAL,
		[](int ops) {
			return run_move_assign_array_test<std::unique_ptr<TestObject[]>>(ops, [](size_t s) {
				return std::make_unique<TestObject[]>(s);
			});
		},
		[](int ops) {
			return run_move_assign_array_test<raw::unique_ptr<TestObject[]>>(ops, [](size_t s) {
				return raw::make_unique<TestObject[]>(s);
			});
		});
	print_table_row("Move Assign Array", move_assign_array_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Move Assign Array", initial_active_objects_before_test);

	TestResults access_single_results = run_benchmark_scenario(
		"Access Single Object", NUM_TRIALS, OPS_PER_TRIAL,
		[](int ops) {
			return run_access_single_test<std::unique_ptr<TestObject>>(ops, [](int val) {
				return std::make_unique<TestObject>(val);
			});
		},
		[](int ops) {
			return run_access_single_test<raw::unique_ptr<TestObject>>(ops, [](int val) {
				return raw::make_unique<TestObject>(val);
			});
		});
	print_table_row("Access Single Object", access_single_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Access Single Object", initial_active_objects_before_test);

	TestResults access_array_elem_results = run_benchmark_scenario(
		"Access Array Element", NUM_TRIALS, OPS_PER_TRIAL,
		[](int ops) {
			return run_access_array_element_test<std::unique_ptr<TestObject[]>>(ops, [](size_t s) {
				return std::make_unique<TestObject[]>(s);
			});
		},
		[](int ops) {
			return run_access_array_element_test<raw::unique_ptr<TestObject[]>>(ops, [](size_t s) {
				return raw::make_unique<TestObject[]>(s);
			});
		});
	print_table_row("Access Array Element", access_array_elem_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Access Array Element", initial_active_objects_before_test);

	const int STRESS_ITERATIONS = 100000;
	const int POOL_SIZE			= 100;

	TestResults combined_stress_results = run_benchmark_scenario(
		"Combined Stress Test", NUM_TRIALS, STRESS_ITERATIONS,
		[&](int ops) {
			return run_combined_stress_impl(false, ops, POOL_SIZE);
		},
		[&](int ops) {
			return run_combined_stress_impl(true, ops, POOL_SIZE);
		});
	print_table_row("Combined Stress Test", combined_stress_results,
					initial_active_objects_before_test, s_active_test_objects);
	verify_active_objects("Combined Stress Test", initial_active_objects_before_test);

	std::cout
		<< "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
	std::cout << "Performance comparison finished.\n";
}