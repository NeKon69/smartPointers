//
// Created by progamers on 5/31/25.
//

#include "../include/unit_shared.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <numeric>
#include <vector>

template<typename T>
void print_shared_ptr_state(const std::string& name, const raw::shared_ptr<T>& ptr) {
	std::cout << name << ": ";
	if (ptr) {
		std::cout << "Address=" << (void*)ptr.get() << ", Value=" << ptr->id
				  << ", UseCount=" << ptr.use_count() << ", bool=true\n";
	} else {
		std::cout << "Null pointer, UseCount=" << ptr.use_count() << ", bool=false\n";
	}
}

template<typename T>
void print_shared_array_ptr_state(const std::string& name, const raw::shared_ptr<T[]>& ptr,
								  size_t size_hint) {
	std::cout << name << ": ";
	if (ptr) {
		std::cout << "Base Address=" << (void*)ptr.get() << ", UseCount=" << ptr.use_count()
				  << ", bool=true, Elements (first " << size_hint << "): [";
		for (size_t i = 0; i < size_hint; ++i) {
			std::cout << ptr[i].id << (i == size_hint - 1 ? "" : ", ");
		}
		std::cout << "]\n";
	} else {
		std::cout << "Null array pointer, UseCount=" << ptr.use_count() << ", bool=false\n";
	}
}

void test_shared_single_object_construction() {
	std::cout << "\n--- Test: Shared Single Object Construction ---\n";
	int initial_active_objects = s_active_test_objects;

	{
		raw::shared_ptr<TestObject> sptr1;
		print_shared_ptr_state("sptr1 (default)", sptr1);
		assert(!sptr1 && sptr1.use_count() == 0);
		verify_active_objects("sptr1 default construction", initial_active_objects);
	}
	verify_active_objects("sptr1 default destruction", initial_active_objects);

	{
		raw::shared_ptr<TestObject> sptr2 = raw::make_shared<TestObject>();
		print_shared_ptr_state("sptr2 (make_shared)", sptr2);
		assert(sptr2 && sptr2->id == 0 && sptr2.use_count() == 1);
		verify_active_objects("sptr2 make_shared", initial_active_objects + 1);
	}
	verify_active_objects("sptr2 destruction", initial_active_objects);

	{
		raw::shared_ptr<TestObject> sptr3 = raw::make_shared<TestObject>(100);
		print_shared_ptr_state("sptr3 (make_shared with arg)", sptr3);
		assert(sptr3 && sptr3->id == 100 && sptr3.use_count() == 1);
		verify_active_objects("sptr3 make_shared with arg", initial_active_objects + 1);
	}
	verify_active_objects("sptr3 destruction", initial_active_objects);

	{
		TestObject*					raw_ptr = new TestObject(200);
		raw::shared_ptr<TestObject> sptr4(raw_ptr);
		print_shared_ptr_state("sptr4 (from raw ptr)", sptr4);
		assert(sptr4 && sptr4->id == 200 && sptr4.use_count() == 1);
		verify_active_objects("sptr4 from raw ptr", initial_active_objects + 1);
	}
	verify_active_objects("sptr4 destruction", initial_active_objects);

	{
		raw::shared_ptr<TestObject> sptr5(nullptr);
		print_shared_ptr_state("sptr5 (nullptr_t)", sptr5);
		assert(!sptr5 && sptr5.use_count() == 0);
		verify_active_objects("sptr5 nullptr_t construction", initial_active_objects);
	}
	verify_active_objects("sptr5 nullptr_t destruction", initial_active_objects);
}

void test_shared_single_object_copy_move_semantics() {
	std::cout << "\n--- Test: Shared Single Object Copy/Move Semantics ---\n";
	int initial_active_objects = s_active_test_objects;

	raw::shared_ptr<TestObject> original = raw::make_shared<TestObject>(10);
	print_shared_ptr_state("original (init)", original);
	assert(original.use_count() == 1);
	verify_active_objects("original created", initial_active_objects + 1);

	raw::shared_ptr<TestObject> copy1 = original;
	print_shared_ptr_state("original (after copy-construct)", original);
	print_shared_ptr_state("copy1 (after copy-construct)", copy1);
	assert(original.use_count() == 2 && copy1.use_count() == 2 && original.get() == copy1.get());
	assert(original->id == 10 && copy1->id == 10);
	verify_active_objects("copy1 copy-construct", initial_active_objects + 1);

	raw::shared_ptr<TestObject> moved1 = std::move(copy1);
	print_shared_ptr_state("copy1 (after move-construct)", copy1);
	print_shared_ptr_state("moved1 (after move-construct)", moved1);
	assert(!copy1 && copy1.use_count() == 0);
	assert(original.use_count() == 2 && moved1.use_count() == 2 && original.get() == moved1.get());
	assert(moved1->id == 10);
	verify_active_objects("moved1 move-construct", initial_active_objects + 1);

	raw::shared_ptr<TestObject> target_copy_assign;
	target_copy_assign = original;
	print_shared_ptr_state("original (after copy-assign)", original);
	print_shared_ptr_state("target_copy_assign (after copy-assign)", target_copy_assign);
	assert(original.use_count() == 3 && target_copy_assign.use_count() == 3 &&
		   original.get() == target_copy_assign.get());
	assert(target_copy_assign->id == 10);
	verify_active_objects("target_copy_assign copy-assign", initial_active_objects + 1);

	raw::shared_ptr<TestObject> to_be_moved = raw::make_shared<TestObject>(20);
	print_shared_ptr_state("to_be_moved (before move-assign)", to_be_moved);
	verify_active_objects("to_be_moved created", initial_active_objects + 2);

	raw::shared_ptr<TestObject> target_move_assign = raw::make_shared<TestObject>(30);
	print_shared_ptr_state("target_move_assign (before move-assign)", target_move_assign);
	verify_active_objects("target_move_assign created", initial_active_objects + 3);

	target_move_assign = std::move(to_be_moved);
	print_shared_ptr_state("to_be_moved (after move-assign)", to_be_moved);
	print_shared_ptr_state("target_move_assign (after move-assign)", target_move_assign);
	assert(!to_be_moved && to_be_moved.use_count() == 0);
	assert(target_move_assign.use_count() == 1 && target_move_assign->id == 20);
	verify_active_objects("target_move_assign move-assign (old #30 deleted)",
						  initial_active_objects + 2);

	raw::shared_ptr<TestObject> self_move = raw::make_shared<TestObject>(40);
	print_shared_ptr_state("self_move (before self-move-assign)", self_move);
	verify_active_objects("self_move created", initial_active_objects + 3);

	self_move = std::move(self_move);
	print_shared_ptr_state("self_move (after self-move-assign)", self_move);
	assert(self_move && self_move->id == 40 && self_move.use_count() == 1);
	verify_active_objects("self_move self-move-assign", initial_active_objects + 3);

	target_copy_assign = nullptr;
	print_shared_ptr_state("target_copy_assign (after nullptr assign)", target_copy_assign);
	assert(!target_copy_assign && target_copy_assign.use_count() == 0);
	assert(original.use_count() == 2 && moved1.use_count() == 2);
	verify_active_objects("target_copy_assign nullptr assign (decremented count)",
						  initial_active_objects + 3);

	original.reset();
	print_shared_ptr_state("original (after reset)", original);
	print_shared_ptr_state("moved1 (after original reset)", moved1);
	assert(!original && original.use_count() == 0);
	assert(moved1.use_count() == 1);
	verify_active_objects("original reset", initial_active_objects + 3);

	moved1.reset();
	print_shared_ptr_state("moved1 (after reset)", moved1);
	assert(!moved1 && moved1.use_count() == 0);
	verify_active_objects("moved1 reset (object #10 deleted)", initial_active_objects + 2);

	self_move.reset();
	verify_active_objects("self_move reset (object #40 deleted)", initial_active_objects + 1);
}

void test_shared_single_object_manipulation() {
	std::cout << "\n--- Test: Shared Single Object Manipulation ---\n";
	int initial_active_objects = s_active_test_objects;

	raw::shared_ptr<TestObject> sptr = raw::make_shared<TestObject>(10);
	print_shared_ptr_state("sptr initial", sptr);
	assert(sptr.use_count() == 1);
	verify_active_objects("sptr created", initial_active_objects + 1);

	assert(sptr.get() != nullptr);
	assert((*sptr).id == 10);
	assert(sptr->id == 10);

	assert(sptr.unique());

	sptr.reset();
	print_shared_ptr_state("sptr after reset()", sptr);
	assert(!sptr && sptr.use_count() == 0);
	verify_active_objects("sptr reset() (old #10 deleted)", initial_active_objects);

	TestObject* new_raw_ptr = new TestObject(11);
	sptr.reset(new_raw_ptr);
	print_shared_ptr_state("sptr after reset(new_ptr)", sptr);
	assert(sptr && sptr->id == 11 && sptr.use_count() == 1);
	verify_active_objects("sptr reset(new_ptr)", initial_active_objects + 1);

	raw::shared_ptr<TestObject> sptrA = raw::make_shared<TestObject>(20);
	raw::shared_ptr<TestObject> sptrB = raw::make_shared<TestObject>(21);
	print_shared_ptr_state("sptrA before swap", sptrA);
	print_shared_ptr_state("sptrB before swap", sptrB);
	assert(sptrA.use_count() == 1 && sptrB.use_count() == 1);
	verify_active_objects("sptrA, sptrB created", initial_active_objects + 3);

	sptrA.swap(sptrB);
	print_shared_ptr_state("sptrA after swap", sptrA);
	print_shared_ptr_state("sptrB after swap", sptrB);
	assert(sptrA && sptrA->id == 21 && sptrA.use_count() == 1);
	assert(sptrB && sptrB->id == 20 && sptrB.use_count() == 1);
	verify_active_objects("sptrA, sptrB after swap", initial_active_objects + 3);

	sptrA.reset();
	sptrB.reset();
	verify_active_objects("sptrA, sptrB destruction", initial_active_objects + 1);
}

void test_shared_array_construction() {
	std::cout << "\n--- Test: Shared Array Construction ---\n";
	int initial_active_objects = s_active_test_objects;

	{
		raw::shared_ptr<TestObject[]> sptr_array_1;
		print_shared_array_ptr_state("sptr_array_1 (default)", sptr_array_1, 0);
		assert(!sptr_array_1 && sptr_array_1.use_count() == 0);
		verify_active_objects("sptr_array_1 default construction", initial_active_objects);
	}
	verify_active_objects("sptr_array_1 default destruction", initial_active_objects);

	{
		raw::shared_ptr<TestObject[]> sptr_array_2 = raw::make_shared<TestObject[]>(5);
		print_shared_array_ptr_state("sptr_array_2 (make_shared<T[]>(5))", sptr_array_2, 5);
		assert(sptr_array_2 && sptr_array_2.get() != nullptr);
		assert(sptr_array_2.use_count() == 1);
		for (size_t i = 0; i < 5; ++i)
			assert(sptr_array_2[i].id == 0);
		verify_active_objects("sptr_array_2 make_shared (5 objects)", initial_active_objects + 5);

		for (size_t i = 0; i < 5; ++i) {
			sptr_array_2[i].id = (int)(i + 1);
		}
		print_shared_array_ptr_state("sptr_array_2 (modified)", sptr_array_2, 5);
		for (size_t i = 0; i < 5; ++i)
			assert(sptr_array_2[i].id == (int)(i + 1));
	}
	verify_active_objects("sptr_array_2 destruction (5 objects)", initial_active_objects);

	{
		TestObject* raw_array = new TestObject[3];
		raw_array[0].id		  = 300;
		raw_array[1].id		  = 301;
		raw_array[2].id		  = 302;
		raw::shared_ptr<TestObject[]> sptr_array_3(raw_array);
		print_shared_array_ptr_state("sptr_array_3 (from raw array ptr)", sptr_array_3, 3);
		assert(sptr_array_3 && sptr_array_3.use_count() == 1);
		assert(sptr_array_3[0].id == 300);
		verify_active_objects("sptr_array_3 from raw ptr", initial_active_objects + 3);
	}
	verify_active_objects("sptr_array_3 destruction", initial_active_objects);
}

void test_shared_array_copy_move_semantics() {
	std::cout << "\n--- Test: Shared Array Copy/Move Semantics ---\n";
	int initial_active_objects = s_active_test_objects;

	raw::shared_ptr<TestObject[]> original_array = raw::make_shared<TestObject[]>(3);
	for (size_t i = 0; i < 3; ++i)
		original_array[i].id = (int)(i + 10);
	print_shared_array_ptr_state("original_array (init)", original_array, 3);
	assert(original_array.use_count() == 1);
	verify_active_objects("original_array created (3 objects)", initial_active_objects + 3);

	raw::shared_ptr<TestObject[]> copy1_array = original_array;
	print_shared_array_ptr_state("original_array (after copy-construct)", original_array, 3);
	print_shared_array_ptr_state("copy1_array (after copy-construct)", copy1_array, 3);
	assert(original_array.use_count() == 2 && copy1_array.use_count() == 2 &&
		   original_array.get() == copy1_array.get());
	assert(copy1_array[0].id == 10);
	verify_active_objects("copy1_array copy-construct", initial_active_objects + 3);

	raw::shared_ptr<TestObject[]> moved1_array = std::move(copy1_array);
	print_shared_array_ptr_state("copy1_array (after move-construct)", copy1_array, 0);
	print_shared_array_ptr_state("moved1_array (after move-construct)", moved1_array, 3);
	assert(!copy1_array && copy1_array.use_count() == 0);
	assert(original_array.use_count() == 2 && moved1_array.use_count() == 2 &&
		   original_array.get() == moved1_array.get());
	assert(moved1_array[0].id == 10);
	verify_active_objects("moved1_array move-construct", initial_active_objects + 3);

	raw::shared_ptr<TestObject[]> target_copy_assign_array;
	target_copy_assign_array = original_array;
	print_shared_array_ptr_state("original_array (after copy-assign)", original_array, 3);
	print_shared_array_ptr_state("target_copy_assign_array (after copy-assign)",
								 target_copy_assign_array, 3);
	assert(original_array.use_count() == 3 && target_copy_assign_array.use_count() == 3 &&
		   original_array.get() == target_copy_assign_array.get());
	assert(target_copy_assign_array[0].id == 10);
	verify_active_objects("target_copy_assign_array copy-assign", initial_active_objects + 3);

	raw::shared_ptr<TestObject[]> to_be_moved_array = raw::make_shared<TestObject[]>(2);
	for (size_t i = 0; i < 2; ++i)
		to_be_moved_array[i].id = (int)(i + 20);
	print_shared_array_ptr_state("to_be_moved_array (before move-assign)", to_be_moved_array, 2);
	verify_active_objects("to_be_moved_array created", initial_active_objects + 3 + 2);

	raw::shared_ptr<TestObject[]> target_move_assign_array = raw::make_shared<TestObject[]>(1);
	target_move_assign_array[0].id						   = 30;
	print_shared_array_ptr_state("target_move_assign_array (before move-assign)",
								 target_move_assign_array, 1);
	verify_active_objects("target_move_assign_array created", initial_active_objects + 3 + 2 + 1);

	target_move_assign_array = std::move(to_be_moved_array);
	print_shared_array_ptr_state("to_be_moved_array (after move-assign)", to_be_moved_array, 0);
	print_shared_array_ptr_state("target_move_assign_array (after move-assign)",
								 target_move_assign_array, 2);
	assert(!to_be_moved_array && to_be_moved_array.use_count() == 0);
	assert(target_move_assign_array.use_count() == 1 && target_move_assign_array[0].id == 20);
	verify_active_objects("target_move_assign_array move-assign (old #30 deleted)",
						  initial_active_objects + 3 + 2);

	raw::shared_ptr<TestObject[]> self_move_array = raw::make_shared<TestObject[]>(1);
	self_move_array[0].id						  = 40;
	print_shared_array_ptr_state("self_move_array (before self-move-assign)", self_move_array, 1);
	verify_active_objects("self_move_array created", initial_active_objects + 3 + 2 + 1);

	self_move_array = std::move(self_move_array);
	print_shared_array_ptr_state("self_move_array (after self-move-assign)", self_move_array, 1);
	assert(self_move_array && self_move_array[0].id == 40 && self_move_array.use_count() == 1);
	verify_active_objects("self_move_array self-move-assign", initial_active_objects + 3 + 2 + 1);

	target_copy_assign_array = nullptr;
	print_shared_array_ptr_state("target_copy_assign_array (after nullptr assign)",
								 target_copy_assign_array, 0);
	assert(!target_copy_assign_array && target_copy_assign_array.use_count() == 0);
	assert(original_array.use_count() == 2 && moved1_array.use_count() == 2);
	verify_active_objects("target_copy_assign_array nullptr assign (decremented count)",
						  initial_active_objects + 3 + 3);

	original_array.reset();
	moved1_array.reset();
	verify_active_objects("original and moved arrays reset", initial_active_objects + 2 + 1);

	target_move_assign_array.reset();
	self_move_array.reset();
	verify_active_objects("all arrays reset", initial_active_objects);
}

void test_shared_array_manipulation() {
	std::cout << "\n--- Test: Shared Array Manipulation ---\n";
	int initial_active_objects = s_active_test_objects;

	raw::shared_ptr<TestObject[]> sptr = raw::make_shared<TestObject[]>(4);
	for (size_t i = 0; i < 4; ++i)
		sptr[i].id = (int)(i + 100);
	print_shared_array_ptr_state("sptr initial", sptr, 4);
	assert(sptr.use_count() == 1);
	verify_active_objects("sptr created (4 objects)", initial_active_objects + 4);
	print_shared_array_ptr_state("sptr before reset()", sptr, 0);
	sptr.reset();
	print_shared_array_ptr_state("sptr after reset()", sptr, 0);
	assert(!sptr && sptr.use_count() == 0);
	verify_active_objects("sptr reset() (old 4 deleted)", initial_active_objects);

	TestObject* new_raw_array = new TestObject[2];
	new_raw_array[0].id		  = 500;
	new_raw_array[1].id		  = 501;
	sptr.reset(new_raw_array);
	print_shared_array_ptr_state("sptr after reset(new_ptr)", sptr, 2);
	assert(sptr && sptr[0].id == 500 && sptr[1].id == 501 && sptr.use_count() == 1);
	verify_active_objects("sptr reset(new_ptr) (2 objects)", initial_active_objects + 2);

	raw::shared_ptr<TestObject[]> sptrA = raw::make_shared<TestObject[]>(1);
	raw::shared_ptr<TestObject[]> sptrB = raw::make_shared<TestObject[]>(2);
	sptrA[0].id							= 1;
	sptrB[0].id							= 2;
	sptrB[1].id							= 3;
	print_shared_array_ptr_state("sptrA before swap", sptrA, 1);
	print_shared_array_ptr_state("sptrB before swap", sptrB, 2);
	assert(sptrA.use_count() == 1 && sptrB.use_count() == 1);
	verify_active_objects("sptrA (1), sptrB (2) created", initial_active_objects + 2 + 1 + 2);

	sptrA.swap(sptrB);
	print_shared_array_ptr_state("sptrA after swap", sptrA, 2);
	print_shared_array_ptr_state("sptrB after swap", sptrB, 1);
	assert(sptrA && sptrA[0].id == 2 && sptrA[1].id == 3 && sptrA.use_count() == 1);
	assert(sptrB && sptrB[0].id == 1 && sptrB.use_count() == 1);
	verify_active_objects("sptrA, sptrB after swap (objects still alive)",
						  initial_active_objects + 5);

	sptrA.reset();
	sptrB.reset();
	verify_active_objects("sptrA, sptrB destruction", initial_active_objects + 2);
}

void test_shared_from_unique() {
	std::cout << "\n--- Test: Shared from Unique ---\n";
	int initial_active_objects = s_active_test_objects;

	raw::unique_ptr<TestObject> uptr_single = raw::make_unique<TestObject>(1000);
	assert(uptr_single.get() != nullptr && uptr_single->id == 1000);
	verify_active_objects("uptr_single created", initial_active_objects + 1);

	raw::shared_ptr<TestObject> sptr_from_uptr(std::move(uptr_single));
	assert(!uptr_single && sptr_from_uptr && sptr_from_uptr->id == 1000 &&
		   sptr_from_uptr.use_count() == 1);
	verify_active_objects("sptr_from_uptr from unique", initial_active_objects + 1);

	raw::unique_ptr<TestObject> uptr_single_2 = raw::make_unique<TestObject>(1001);
	assert(uptr_single_2.get() != nullptr && uptr_single_2->id == 1001);
	raw::shared_ptr<TestObject> sptr_target_assign;
	sptr_target_assign = std::move(uptr_single_2);
	assert(!uptr_single_2 && sptr_target_assign && sptr_target_assign->id == 1001 &&
		   sptr_target_assign.use_count() == 1);
	verify_active_objects("sptr_target_assign from unique", initial_active_objects + 2);

	raw::unique_ptr<TestObject> uptr_single_10 = raw::make_unique<TestObject>(1001);
	assert(uptr_single_10.get() != nullptr && uptr_single_10->id == 1001);
	raw::shared_ptr<TestObject[]> sptr_target_assign2;
	sptr_target_assign = std::move(uptr_single_10);

	raw::unique_ptr<TestObject[]> uptr_array = raw::make_unique<TestObject[]>(2);
	uptr_array[0].id						 = 2000;
	uptr_array[1].id						 = 2001;
	assert(uptr_array.get() != nullptr && uptr_array[0].id == 2000);
	verify_active_objects("uptr_array created", initial_active_objects + 2 + 2);

	raw::shared_ptr<TestObject[]> sptr_from_uptr_array(std::move(uptr_array));
	assert(!uptr_array && sptr_from_uptr_array && sptr_from_uptr_array[0].id == 2000 &&
		   sptr_from_uptr_array.use_count() == 1);
	verify_active_objects("sptr_from_uptr_array from unique", initial_active_objects + 2 + 2);

	sptr_from_uptr.reset();
	sptr_target_assign.reset();
	sptr_from_uptr_array.reset();
	verify_active_objects("Shared from unique cleanup", initial_active_objects);
}

void stress_test_shared_ptr(int iterations, int max_pointers_in_pool) {
	std::cout << "\n--- Stress Test: Shared Ptr (" << iterations << " iterations) ---\n";
	std::random_device				rd;
	std::mt19937					gen(rd());
	std::uniform_int_distribution<> dist_op(0, 10);
	std::uniform_int_distribution<> dist_idx(0, max_pointers_in_pool - 1);
	std::uniform_int_distribution<> dist_val(0, 9999);
	std::uniform_int_distribution<> dist_array_size_gen(1, 10);

	std::vector<raw::shared_ptr<TestObject>>   single_ptrs(max_pointers_in_pool);
	std::vector<raw::shared_ptr<TestObject[]>> array_ptrs(max_pointers_in_pool);
	std::vector<size_t>						   array_actual_sizes(max_pointers_in_pool, 0);

	auto start_time = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < iterations; ++i) {
		int op	 = dist_op(gen);
		int idx1 = dist_idx(gen);
		int idx2 = dist_idx(gen);

		try {
			switch (op) {
			case 0:
				single_ptrs[idx1] = raw::make_shared<TestObject>(dist_val(gen));
				break;
			case 1:
				single_ptrs[idx1].reset();
				break;
			case 2:
				if (single_ptrs[idx2]) {
					single_ptrs[idx1] = single_ptrs[idx2];
				} else {
					single_ptrs[idx1].reset();
				}
				break;
			case 3: {
				raw::shared_ptr<TestObject> temp = raw::make_shared<TestObject>(dist_val(gen));
				single_ptrs[idx1]				 = std::move(temp);
			} break;
			case 4:
				single_ptrs[idx1].swap(single_ptrs[idx2]);
				break;
			case 5:
				if (single_ptrs[idx1]) {
					volatile int id = single_ptrs[idx1]->id;
					(void)id;
					volatile size_t count = single_ptrs[idx1].use_count();
					(void)count;
				}
				break;
			case 6: {
				size_t new_size			 = dist_array_size_gen(gen);
				array_ptrs[idx1]		 = raw::make_shared<TestObject[]>(new_size);
				array_actual_sizes[idx1] = new_size;
				if (array_ptrs[idx1]) {
					for (size_t k = 0; k < new_size; ++k) {
						array_ptrs[idx1][k].id = dist_val(gen);
					}
				}
			} break;
			case 7:
				array_ptrs[idx1].reset();
				array_actual_sizes[idx1] = 0;
				break;
			case 8:
				if (array_ptrs[idx2]) {
					array_ptrs[idx1]		 = array_ptrs[idx2];
					array_actual_sizes[idx1] = array_actual_sizes[idx2];
				} else {
					array_ptrs[idx1].reset();
					array_actual_sizes[idx1] = 0;
				}
				break;
			case 9: {
				size_t						  temp_size = dist_array_size_gen(gen);
				raw::shared_ptr<TestObject[]> temp		= raw::make_shared<TestObject[]>(temp_size);
				array_ptrs[idx1]						= std::move(temp);
				array_actual_sizes[idx1]				= temp_size;
			} break;
			case 10:
				if (array_ptrs[idx1] && array_actual_sizes[idx1] > 0) {
					size_t elem_idx =
						std::uniform_int_distribution<size_t>(0, array_actual_sizes[idx1] - 1)(gen);
					volatile int val = array_ptrs[idx1][elem_idx].id;
					(void)val;
					array_ptrs[idx1][elem_idx].id = dist_val(gen);
					volatile size_t count		  = array_ptrs[idx1].use_count();
					(void)count;
				}
				break;
			}

			if (i % 10000 == 0) {
				assert(s_active_test_objects >= 0);
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

	for (int i = 0; i < max_pointers_in_pool; ++i) {
		single_ptrs[i].reset();
		array_ptrs[i].reset();
	}
	verify_active_objects("Stress test cleanup", 0);
}

void run_all_shared_tests() {
	std::cout << "\nStarting shared_ptr tests...\n";
	int initial_active_objects = s_active_test_objects;

	test_shared_single_object_construction();
	verify_active_objects("After test_shared_single_object_construction", initial_active_objects);

	test_shared_single_object_copy_move_semantics();
	verify_active_objects("After test_shared_single_object_copy_move_semantics",
						  initial_active_objects);

	test_shared_single_object_manipulation();
	verify_active_objects("After test_shared_single_object_manipulation", initial_active_objects);

	test_shared_array_construction();
	verify_active_objects("After test_shared_array_construction", initial_active_objects);

	test_shared_array_copy_move_semantics();
	verify_active_objects("After test_shared_array_copy_move_semantics", initial_active_objects);

	test_shared_array_manipulation();
	verify_active_objects("After test_shared_array_manipulation", initial_active_objects);

	test_shared_from_unique();
	verify_active_objects("After test_shared_from_unique", initial_active_objects);

	stress_test_shared_ptr(100000, 100);
	verify_active_objects("After stress_test_shared_ptr", initial_active_objects);

	std::cout << "\nAll shared_ptr tests PASSED!.\n";
	verify_active_objects("Final check after all shared_ptr unit tests", 0);
}