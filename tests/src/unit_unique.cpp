//
// Created by progamers on 5/30/25.
//

#include "../include/unit_unique.h"

#include <chrono>

template<typename T>
void print_ptr_state(const std::string& name, const raw::unique_ptr<T>& ptr) {
	std::cout << name << ": ";
	if (ptr) {
		std::cout << "Address=" << (void*)ptr.get() << ", Value=" << ptr->id << ", bool=true\n";
	} else {
		std::cout << "Null pointer, bool=false\n";
	}
}

template<typename T>
void print_array_ptr_state(const std::string& name, const raw::unique_ptr<T[]>& ptr, size_t size) {
	std::cout << name << ": ";
	if (ptr) {
		std::cout << "Base Address=" << (void*)ptr.get() << ", bool=true, Elements: [";
		for (size_t i = 0; i < size; ++i) {
			std::cout << ptr[i].id << (i == size - 1 ? "" : ", ");
		}
		std::cout << "]\n";
	} else {
		std::cout << "Null array pointer, bool=false\n";
	}
}

void test_single_object_construction() {
	std::cout << "\n--- Test: Single Object Construction ---\n";
	int initial_active_objects = s_active_test_objects;

	{
		raw::unique_ptr<TestObject> uptr1;
		print_ptr_state("uptr1 (default)", uptr1);
		verify_active_objects("uptr1 default construction", initial_active_objects);
	}
	verify_active_objects("uptr1 default destruction", initial_active_objects);

	{
		raw::unique_ptr<TestObject> uptr2 = raw::make_unique<TestObject>();
		print_ptr_state("uptr2 (make_unique)", uptr2);
		assert(uptr2 && uptr2->id == 0);
		verify_active_objects("uptr2 make_unique", initial_active_objects + 1);
	}
	verify_active_objects("uptr2 destruction", initial_active_objects);

	{
		raw::unique_ptr<TestObject> uptr3 = raw::make_unique<TestObject>(100);
		print_ptr_state("uptr3 (make_unique with arg)", uptr3);
		assert(uptr3 && uptr3->id == 100);
		verify_active_objects("uptr3 make_unique with arg", initial_active_objects + 1);
	}
	verify_active_objects("uptr3 destruction", initial_active_objects);

	{
		TestObject*					raw_ptr = new TestObject(200);
		raw::unique_ptr<TestObject> uptr4(raw_ptr);
		print_ptr_state("uptr4 (from raw ptr)", uptr4);
		assert(uptr4 && uptr4->id == 200);
		verify_active_objects("uptr4 from raw ptr", initial_active_objects + 1);
	}
	verify_active_objects("uptr4 destruction", initial_active_objects);
}

void test_single_object_move_semantics() {
	std::cout << "\n--- Test: Single Object Move Semantics ---\n";
	int initial_active_objects = s_active_test_objects;

	raw::unique_ptr<TestObject> original_uptr = raw::make_unique<TestObject>(1);
	print_ptr_state("original_uptr before move", original_uptr);
	verify_active_objects("original_uptr created", initial_active_objects + 1);

	raw::unique_ptr<TestObject> moved_uptr(std::move(original_uptr));
	print_ptr_state("original_uptr after move-construct", original_uptr);
	print_ptr_state("moved_uptr after move-construct", moved_uptr);
	assert(!original_uptr && moved_uptr && moved_uptr->id == 1);
	verify_active_objects("moved_uptr move-construct", initial_active_objects + 1);

	raw::unique_ptr<TestObject> another_uptr = raw::make_unique<TestObject>(2);
	print_ptr_state("another_uptr before move-assign", another_uptr);
	verify_active_objects("another_uptr created", initial_active_objects + 2);

	moved_uptr = std::move(another_uptr);
	print_ptr_state("moved_uptr after move-assign", moved_uptr);
	print_ptr_state("another_uptr after move-assign", another_uptr);
	assert(!another_uptr && moved_uptr && moved_uptr->id == 2);
	verify_active_objects("moved_uptr move-assign (old #1 deleted)", initial_active_objects + 1);

	moved_uptr = std::move(moved_uptr);
	print_ptr_state("moved_uptr after self-move-assign", moved_uptr);
	assert(moved_uptr && moved_uptr->id == 2);
	verify_active_objects("moved_uptr self-move-assign", initial_active_objects + 1);

	moved_uptr = nullptr;
	print_ptr_state("moved_uptr after nullptr assign", moved_uptr);
	assert(!moved_uptr);
	verify_active_objects("moved_uptr nullptr assign (old #2 deleted)", initial_active_objects);
}

void test_single_object_manipulation() {
	std::cout << "\n--- Test: Single Object Manipulation ---\n";
	int initial_active_objects = s_active_test_objects;

	raw::unique_ptr<TestObject> uptr = raw::make_unique<TestObject>(10);
	print_ptr_state("uptr initial", uptr);
	verify_active_objects("uptr created", initial_active_objects + 1);

	uptr.reset();
	print_ptr_state("uptr after reset()", uptr);
	assert(!uptr);
	verify_active_objects("uptr reset() (old #10 deleted)", initial_active_objects);

	uptr.reset(new TestObject(11));
	print_ptr_state("uptr after reset(new_ptr)", uptr);
	assert(uptr && uptr->id == 11);
	verify_active_objects("uptr reset(new_ptr)", initial_active_objects + 1);

	TestObject* current_raw_ptr = uptr.get();
	uptr.reset(current_raw_ptr);
	print_ptr_state("uptr after reset(same_ptr)", uptr);
	assert(uptr && uptr->id == 11 && uptr.get() == current_raw_ptr);
	verify_active_objects("uptr reset(same_ptr)", initial_active_objects + 1);

	TestObject* released_ptr = uptr.release();
	print_ptr_state("uptr after release()", uptr);
	assert(!uptr && released_ptr && released_ptr->id == 11);
	verify_active_objects("uptr release() (object not deleted)", initial_active_objects + 1);

	delete released_ptr;
	verify_active_objects("Manually deleted released_ptr", initial_active_objects);

	raw::unique_ptr<TestObject> uptrA = raw::make_unique<TestObject>(20);
	raw::unique_ptr<TestObject> uptrB = raw::make_unique<TestObject>(21);
	print_ptr_state("uptrA before swap", uptrA);
	print_ptr_state("uptrB before swap", uptrB);
	verify_active_objects("uptrA, uptrB created", initial_active_objects + 2);

	uptrA.swap(uptrB);
	print_ptr_state("uptrA after swap", uptrA);
	print_ptr_state("uptrB after swap", uptrB);
	assert(uptrA && uptrA->id == 21);
	assert(uptrB && uptrB->id == 20);
	verify_active_objects("uptrA, uptrB after swap", initial_active_objects + 2);
}

void test_array_construction() {
	std::cout << "\n--- Test: Array Construction ---\n";
	int initial_active_objects = s_active_test_objects;

	{
		raw::unique_ptr<TestObject[]> uptr_array_1;
		print_array_ptr_state("uptr_array_1 (default)", uptr_array_1, 0);
		verify_active_objects("uptr_array_1 default construction", initial_active_objects);
	}
	verify_active_objects("uptr_array_1 default destruction", initial_active_objects);

	{
		raw::unique_ptr<TestObject[]> uptr_array_2 = raw::make_unique<TestObject[]>(5);
		print_array_ptr_state("uptr_array_2 (make_unique<T[]>(5))", uptr_array_2, 5);
		assert(uptr_array_2 && uptr_array_2.get() != nullptr);
		for (size_t i = 0; i < 5; ++i)
			assert(uptr_array_2[i].id == 0);
		verify_active_objects("uptr_array_2 make_unique (5 objects)", initial_active_objects + 5);

		for (size_t i = 0; i < 5; ++i) {
			uptr_array_2[i].id = (int)(i + 1);
		}
		print_array_ptr_state("uptr_array_2 (modified)", uptr_array_2, 5);
		for (size_t i = 0; i < 5; ++i)
			assert(uptr_array_2[i].id == (int)(i + 1));
	}
	verify_active_objects("uptr_array_2 destruction (5 objects)", initial_active_objects);
}

void test_array_move_semantics() {
	std::cout << "\n--- Test: Array Move Semantics ---\n";
	int initial_active_objects = s_active_test_objects;

	raw::unique_ptr<TestObject[]> original_array = raw::make_unique<TestObject[]>(3);
	for (size_t i = 0; i < 3; ++i)
		original_array[i].id = (int)(i + 10);
	print_array_ptr_state("original_array before move", original_array, 3);
	verify_active_objects("original_array created (3 objects)", initial_active_objects + 3);

	raw::unique_ptr<TestObject[]> moved_array = std::move(original_array);
	print_array_ptr_state("original_array after move-construct", original_array, 0);
	print_array_ptr_state("moved_array after move-construct", moved_array, 3);
	assert(!original_array && moved_array && moved_array[0].id == 10);
	verify_active_objects("moved_array move-construct (3 objects)", initial_active_objects + 3);

	raw::unique_ptr<TestObject[]> another_array = raw::make_unique<TestObject[]>(2);
	for (size_t i = 0; i < 2; ++i)
		another_array[i].id = (int)(i + 20);
	print_array_ptr_state("another_array before move-assign", another_array, 2);
	verify_active_objects("another_array created (2 objects)", initial_active_objects + 3 + 2);

	moved_array = std::move(another_array);
	print_array_ptr_state("moved_array after move-assign", moved_array, 2);
	print_array_ptr_state("another_array after move-assign", another_array, 0);
	assert(!another_array && moved_array && moved_array[0].id == 20 && moved_array[1].id == 21);
	verify_active_objects("moved_array move-assign (old 3 deleted)", initial_active_objects + 2);

	moved_array = nullptr;
	print_array_ptr_state("moved_array after nullptr assign", moved_array, 0);
	assert(!moved_array);
	verify_active_objects("moved_array nullptr assign (old 2 deleted)", initial_active_objects);
}

void test_array_manipulation() {
	std::cout << "\n--- Test: Array Manipulation ---\n";
	int initial_active_objects = s_active_test_objects;

	raw::unique_ptr<TestObject[]> uptr = raw::make_unique<TestObject[]>(4);
	for (size_t i = 0; i < 4; ++i)
		uptr[i].id = (int)(i + 100);
	print_array_ptr_state("uptr initial", uptr, 4);
	verify_active_objects("uptr created (4 objects)", initial_active_objects + 4);

	uptr.reset();
	print_array_ptr_state("uptr after reset()", uptr, 0);
	assert(!uptr);
	verify_active_objects("uptr reset() (old 4 deleted)", initial_active_objects);

	TestObject* new_raw_array = new TestObject[2];
	new_raw_array[0].id		  = 500;
	new_raw_array[1].id		  = 501;
	uptr.reset(new_raw_array);
	print_array_ptr_state("uptr after reset(new_ptr)", uptr, 2);
	assert(uptr && uptr[0].id == 500 && uptr[1].id == 501);
	verify_active_objects("uptr reset(new_ptr) (2 objects)", initial_active_objects + 2);

	TestObject* released_array = uptr.release();
	print_array_ptr_state("uptr after release()", uptr, 0);
	assert(!uptr && released_array && released_array[0].id == 500);
	verify_active_objects("uptr release() (array not deleted)", initial_active_objects + 2);

	delete[] released_array;
	verify_active_objects("Manually deleted released_array", initial_active_objects);

	raw::unique_ptr<TestObject[]> uptrA = raw::make_unique<TestObject[]>(1);
	raw::unique_ptr<TestObject[]> uptrB = raw::make_unique<TestObject[]>(2);
	uptrA[0].id							= 1;
	uptrB[0].id							= 2;
	uptrB[1].id							= 3;
	print_array_ptr_state("uptrA before swap", uptrA, 1);
	print_array_ptr_state("uptrB before swap", uptrB, 2);
	verify_active_objects("uptrA (1), uptrB (2) created", initial_active_objects + 1 + 2);

	uptrA.swap(uptrB);
	print_array_ptr_state("uptrA after swap", uptrA, 2);
	print_array_ptr_state("uptrB after swap", uptrB, 1);
	assert(uptrA && uptrA[0].id == 2 && uptrA[1].id == 3);
	assert(uptrB && uptrB[0].id == 1);
	verify_active_objects("uptrA, uptrB after swap (objects still alive)",
						  initial_active_objects + 3);
}

void stress_test_unique_ptr(int iterations, int max_pointers_in_pool) {
	std::cout << "\n--- Stress Test: Unique Ptr (" << iterations << " iterations) ---\n";
	std::random_device				rd;
	std::mt19937					gen(rd());
	std::uniform_int_distribution<> dist_op(0, 5);
	std::uniform_int_distribution<> dist_idx(0, max_pointers_in_pool - 1);
	std::uniform_int_distribution<> dist_val(0, 9999);
	std::uniform_int_distribution<> dist_array_size_gen(1, 10);

	std::vector<raw::unique_ptr<TestObject>>   single_ptrs(max_pointers_in_pool);
	std::vector<raw::unique_ptr<TestObject[]>> array_ptrs(max_pointers_in_pool);
	std::vector<size_t>						   array_actual_sizes(max_pointers_in_pool, 0);

	auto start_time = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < iterations; ++i) {
		int op	 = dist_op(gen);
		int idx1 = dist_idx(gen);

		try {
			switch (op) {
			case 0:
				single_ptrs[idx1] = raw::make_unique<TestObject>(dist_val(gen));
				break;
			case 1:
				single_ptrs[idx1].reset();
				break;
			case 2:
				if (single_ptrs[idx1]) {
					TestObject* released = single_ptrs[idx1].release();
					delete released;
				}
				break;
			case 3: {
				size_t new_size			 = dist_array_size_gen(gen);
				array_ptrs[idx1]		 = raw::make_unique<TestObject[]>(new_size);
				array_actual_sizes[idx1] = new_size;
				if (array_ptrs[idx1]) {
					for (size_t k = 0; k < new_size; ++k) {
						array_ptrs[idx1][k].id = dist_val(gen);
					}
				}
			} break;
			case 4: {
				int idx2_swap = dist_idx(gen);
				array_ptrs[idx1].swap(array_ptrs[idx2_swap]);
				std::swap(array_actual_sizes[idx1], array_actual_sizes[idx2_swap]);
			} break;
			case 5:
				if (array_ptrs[idx1] && array_actual_sizes[idx1] > 0) {
					size_t elem_idx =
						std::uniform_int_distribution<size_t>(0, array_actual_sizes[idx1] - 1)(gen);
					array_ptrs[idx1][elem_idx].id = dist_val(gen);
					volatile int read_val		  = array_ptrs[idx1][elem_idx].id;
					(void)read_val;
				}
				break;
			}

			if (i % 1000 == 0) {
				assert(s_active_test_objects >= 0 &&
					   s_active_test_objects <= max_pointers_in_pool * (1 + 10));
			}

		} catch (const std::exception& e) {
			std::cerr << "Exception in stress test at iteration " << i << ": " << e.what()
					  << std::endl;
			exit(1);
		}
	}

	auto						  end_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end_time - start_time;
	std::cout << "Stress test completed in " << duration.count() << " seconds.\n";

	single_ptrs.clear();
	array_ptrs.clear();

	verify_active_objects("Stress test cleanup", 0);
}

void run_all_unique_tests() {
	std::cout << "Starting unique_ptr tests...\n";
	test_single_object_construction();
	test_single_object_move_semantics();
	test_single_object_manipulation();

	test_array_construction();
	test_array_move_semantics();
	test_array_manipulation();

	stress_test_unique_ptr(100000);
	std::cout << "\nAll unique_ptr tests PASSED!.\n";

	verify_active_objects("Final check after all unit tests", 0);
}