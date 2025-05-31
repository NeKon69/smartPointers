//
// Created by progamers on 5/31/25.
//

#include "../include/unit_weak.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <numeric>
#include <vector>

template<typename T>
void print_weak_ptr_state(const std::string& name, const raw::weak_ptr<T>& ptr) {
	std::cout << name << ": ";
	if (ptr.expired()) {
		std::cout << "Expired (Target deleted), UseCount (Shared)=" << ptr.use_count()
				  << ", bool=false\n";
	} else {
		raw::shared_ptr<T> sptr = ptr.lock();
		std::cout << "Address=" << (void*)sptr.get() << ", Value=" << sptr->id
				  << ", UseCount (Shared)=" << ptr.use_count() << ", bool=true\n";
	}
}

template<typename T>
void print_weak_array_ptr_state(const std::string& name, const raw::weak_ptr<T[]>& ptr,
								size_t size_hint) {
	std::cout << name << ": ";
	if (ptr.expired()) {
		std::cout << "Expired (Target deleted), UseCount (Shared)=" << ptr.use_count()
				  << ", bool=false\n";
	} else {
		raw::shared_ptr<T[]> sptr = ptr.lock();
		std::cout << "Base Address=" << (void*)sptr.get()
				  << ", UseCount (Shared)=" << ptr.use_count() << ", bool=true";
		if (sptr && size_hint > 0) {
			std::cout << ", Elements (first " << size_hint << "): [";
			for (size_t i = 0; i < size_hint; ++i) {
				std::cout << sptr[i].id << (i == size_hint - 1 ? "" : ", ");
			}
			std::cout << "]";
		}
		std::cout << "\n";
	}
}

void test_weak_single_object_construction() {
	std::cout << "\n--- Test: Weak Single Object Construction ---\n";
	int initial_active_objects = s_active_test_objects;

	{
		raw::weak_ptr<TestObject> wptr1;
		print_weak_ptr_state("wptr1 (default)", wptr1);
		assert(wptr1.expired() && wptr1.use_count() == 0);
		verify_active_objects("wptr1 default construction", initial_active_objects);
	}
	verify_active_objects("wptr1 default destruction", initial_active_objects);

	{
		raw::weak_ptr<TestObject> wptr2(nullptr);
		print_weak_ptr_state("wptr2 (nullptr_t)", wptr2);
		assert(wptr2.expired() && wptr2.use_count() == 0);
		verify_active_objects("wptr2 nullptr_t construction", initial_active_objects);
	}
	verify_active_objects("wptr2 nullptr_t destruction", initial_active_objects);

	{
		raw::shared_ptr<TestObject> sptr = raw::make_shared<TestObject>(100);
		raw::weak_ptr<TestObject>	wptr3(sptr);
		print_weak_ptr_state("wptr3 (from shared_ptr)", wptr3);
		assert(!wptr3.expired() && wptr3.use_count() == 1);
		assert(wptr3.lock().get() == sptr.get());
		verify_active_objects("wptr3 from shared_ptr", initial_active_objects + 1);
	}
	verify_active_objects("wptr3 destruction (shared_ptr and object also destroyed)",
						  initial_active_objects);
}

void test_weak_single_object_copy_move_semantics() {
	std::cout << "\n--- Test: Weak Single Object Copy/Move Semantics ---\n";
	int initial_active_objects = s_active_test_objects;

	raw::shared_ptr<TestObject> sptr_orig = raw::make_shared<TestObject>(10);
	raw::weak_ptr<TestObject>	wptr_orig(sptr_orig);
	print_weak_ptr_state("wptr_orig (init)", wptr_orig);
	assert(!wptr_orig.expired() && wptr_orig.use_count() == 1);
	verify_active_objects("wptr_orig from sptr_orig", initial_active_objects + 1);

	raw::weak_ptr<TestObject> wptr_copy1 = wptr_orig;
	print_weak_ptr_state("wptr_copy1 (copy-construct)", wptr_copy1);
	assert(!wptr_copy1.expired() && wptr_copy1.use_count() == 1);
	assert(wptr_copy1.lock().get() == sptr_orig.get());
	verify_active_objects("wptr_copy1 copy-construct", initial_active_objects + 1);

	raw::weak_ptr<TestObject> wptr_moved1 = std::move(wptr_copy1);
	print_weak_ptr_state("wptr_copy1 (after move-construct)", wptr_copy1);
	print_weak_ptr_state("wptr_moved1 (move-construct)", wptr_moved1);
	assert(wptr_copy1.expired() && wptr_copy1.use_count() == 0);
	assert(!wptr_moved1.expired() && wptr_moved1.use_count() == 1);
	assert(wptr_moved1.lock().get() == sptr_orig.get());
	verify_active_objects("wptr_moved1 move-construct", initial_active_objects + 1);

	raw::shared_ptr<TestObject> sptr_target = raw::make_shared<TestObject>(20);
	raw::weak_ptr<TestObject>	wptr_target_copy_assign(sptr_target);
	print_weak_ptr_state("wptr_target_copy_assign (before copy-assign)", wptr_target_copy_assign);
	verify_active_objects("wptr_target_copy_assign created", initial_active_objects + 2);

	wptr_target_copy_assign = wptr_orig;
	print_weak_ptr_state("wptr_target_copy_assign (after copy-assign)", wptr_target_copy_assign);
	print_weak_ptr_state("wptr_orig (after copy-assign)", wptr_orig);
	assert(!wptr_target_copy_assign.expired() && wptr_target_copy_assign.use_count() == 1);
	assert(wptr_target_copy_assign.lock().get() == sptr_orig.get());
	verify_active_objects("wptr_target_copy_assign copy-assign (old #20 is still alive)",
						  initial_active_objects + 2);

	raw::shared_ptr<TestObject> sptr_to_be_moved = raw::make_shared<TestObject>(30);
	raw::weak_ptr<TestObject>	wptr_to_be_moved(sptr_to_be_moved);
	print_weak_ptr_state("wptr_to_be_moved (before move-assign)", wptr_to_be_moved);
	verify_active_objects("wptr_to_be_moved created", initial_active_objects + 3);

	raw::shared_ptr<TestObject> sptr_target_move = raw::make_shared<TestObject>(40);
	raw::weak_ptr<TestObject>	wptr_target_move_assign(sptr_target_move);
	print_weak_ptr_state("wptr_target_move_assign (before move-assign)", wptr_target_move_assign);
	verify_active_objects("wptr_target_move_assign created", initial_active_objects + 4);

	wptr_target_move_assign = std::move(wptr_to_be_moved);
	print_weak_ptr_state("wptr_to_be_moved (after move-assign)", wptr_to_be_moved);
	print_weak_ptr_state("wptr_target_move_assign (after move-assign)", wptr_target_move_assign);
	assert(wptr_to_be_moved.expired() && wptr_to_be_moved.use_count() == 0);
	assert(!wptr_target_move_assign.expired() && wptr_target_move_assign.use_count() == 1);
	assert(wptr_target_move_assign.lock().get() == sptr_to_be_moved.get());
	verify_active_objects("wptr_target_move_assign move-assign (old #40 is still alive)",
						  initial_active_objects + 4);

	raw::weak_ptr<TestObject> self_move = wptr_orig;
	print_weak_ptr_state("self_move (before self-move-assign)", self_move);
	assert(!self_move.expired() && self_move.use_count() == 1);
	verify_active_objects("self_move created", initial_active_objects + 4);

	self_move = std::move(self_move);
	print_weak_ptr_state("self_move (after self-move-assign)", self_move);
	assert(!self_move.expired() && self_move.use_count() == 1);
	verify_active_objects("self_move self-move-assign", initial_active_objects + 4);

	wptr_orig.reset();
	wptr_moved1.reset();
	wptr_target_copy_assign.reset();
	wptr_target_move_assign.reset();
	self_move.reset();
	verify_active_objects(
		"all weak_ptrs reset (sptr_orig, sptr_target, sptr_to_be_moved, sptr_target_move still exist)",
		initial_active_objects + 4);

	sptr_orig.reset();
	sptr_to_be_moved.reset();
	verify_active_objects("sptr_orig and sptr_to_be_moved reset (objects 10 and 30 deleted)",
						  initial_active_objects + 2);
}

void test_weak_single_object_manipulation() {
	std::cout << "\n--- Test: Weak Single Object Manipulation ---\n";
	int initial_active_objects = s_active_test_objects;

	raw::shared_ptr<TestObject> sptr1 = raw::make_shared<TestObject>(1);
	raw::weak_ptr<TestObject>	wptr1(sptr1);
	print_weak_ptr_state("wptr1 initial", wptr1);
	assert(!wptr1.expired() && wptr1.use_count() == 1);
	verify_active_objects("sptr1, wptr1 created", initial_active_objects + 1);

	wptr1.reset();
	print_weak_ptr_state("wptr1 after reset()", wptr1);
	assert(wptr1.expired() && wptr1.use_count() == 0);
	assert(sptr1.use_count() == 1);
	verify_active_objects("wptr1 reset (sptr1 still exists)", initial_active_objects + 1);

	wptr1 = sptr1;
	print_weak_ptr_state("wptr1 after re-assignment from sptr1", wptr1);
	assert(!wptr1.expired() && wptr1.use_count() == 1);
	verify_active_objects("wptr1 reassigned", initial_active_objects + 1);

	raw::shared_ptr<TestObject> sptr2 = raw::make_shared<TestObject>(2);
	raw::weak_ptr<TestObject>	wptr2(sptr2);
	print_weak_ptr_state("wptr2 initial", wptr2);
	assert(!wptr2.expired() && wptr2.use_count() == 1);
	verify_active_objects("sptr2, wptr2 created", initial_active_objects + 2);

	wptr1.swap(wptr2);
	print_weak_ptr_state("wptr1 after swap", wptr1);
	print_weak_ptr_state("wptr2 after swap", wptr2);
	assert(!wptr1.expired() && wptr1.use_count() == 1 && wptr1.lock()->id == 2);
	assert(!wptr2.expired() && wptr2.use_count() == 1 && wptr2.lock()->id == 1);
	verify_active_objects("wptr1, wptr2 after swap", initial_active_objects + 2);

	raw::shared_ptr<TestObject> locked_ptr = wptr1.lock();
	print_weak_ptr_state("wptr1 (after lock)", wptr1);
	assert(locked_ptr && locked_ptr->id == 2 && locked_ptr.use_count() == 2);
	verify_active_objects("locked_ptr created", initial_active_objects + 2);

	locked_ptr.reset();
	print_weak_ptr_state("wptr1 (after locked_ptr reset)", wptr1);
	assert(wptr1.use_count() == 1);
	verify_active_objects("locked_ptr reset", initial_active_objects + 2);

	sptr1.reset();
	print_weak_ptr_state("wptr2 (after sptr1 reset)", wptr2);
	assert(wptr2.expired() && wptr2.use_count() == 0);
	verify_active_objects("sptr1 reset (object #1 deleted)", initial_active_objects + 1);

	sptr2.reset();
	print_weak_ptr_state("wptr1 (after sptr2 reset)", wptr1);
	assert(wptr1.expired() && wptr1.use_count() == 0);
	verify_active_objects("sptr2 reset (object #2 deleted)", initial_active_objects);

	wptr1.reset();
	wptr2.reset();
	verify_active_objects("final cleanup", initial_active_objects);
}

void test_weak_array_construction() {
	std::cout << "\n--- Test: Weak Array Construction ---\n";
	int initial_active_objects = s_active_test_objects;

	{
		raw::weak_ptr<TestObject[]> wptr_array_1;
		print_weak_array_ptr_state("wptr_array_1 (default)", wptr_array_1, 0);
		assert(wptr_array_1.expired() && wptr_array_1.use_count() == 0);
		verify_active_objects("wptr_array_1 default construction", initial_active_objects);
	}
	verify_active_objects("wptr_array_1 default destruction", initial_active_objects);

	{
		raw::shared_ptr<TestObject[]> sptr_array = raw::make_shared<TestObject[]>(3);
		for (size_t i = 0; i < 3; ++i)
			sptr_array[i].id = (int)(i + 10);
		raw::weak_ptr<TestObject[]> wptr_array_2(sptr_array);
		print_weak_array_ptr_state("wptr_array_2 (from shared_ptr<T[]>(3))", wptr_array_2, 3);
		assert(!wptr_array_2.expired() && wptr_array_2.use_count() == 1);
		assert(wptr_array_2.lock()[0].id == 10);
		verify_active_objects("wptr_array_2 from shared_ptr (3 objects)",
							  initial_active_objects + 3);
	}
	verify_active_objects("wptr_array_2 destruction (shared_ptr and objects also destroyed)",
						  initial_active_objects);
}

void test_weak_array_copy_move_semantics() {
	std::cout << "\n--- Test: Weak Array Copy/Move Semantics ---\n";
	int initial_active_objects = s_active_test_objects;

	raw::shared_ptr<TestObject[]> sptr_orig_array = raw::make_shared<TestObject[]>(2);
	sptr_orig_array[0].id						  = 100;
	sptr_orig_array[1].id						  = 101;
	raw::weak_ptr<TestObject[]> wptr_orig_array(sptr_orig_array);
	print_weak_array_ptr_state("wptr_orig_array (init)", wptr_orig_array, 2);
	assert(!wptr_orig_array.expired() && wptr_orig_array.use_count() == 1);
	verify_active_objects("wptr_orig_array created (2 objects)", initial_active_objects + 2);

	raw::weak_ptr<TestObject[]> wptr_copy1_array = wptr_orig_array;
	print_weak_array_ptr_state("wptr_copy1_array (copy-construct)", wptr_copy1_array, 2);
	assert(!wptr_copy1_array.expired() && wptr_copy1_array.use_count() == 1);
	assert(wptr_copy1_array.lock()[0].id == 100);
	verify_active_objects("wptr_copy1_array copy-construct", initial_active_objects + 2);

	raw::weak_ptr<TestObject[]> wptr_moved1_array = std::move(wptr_copy1_array);
	print_weak_array_ptr_state("wptr_copy1_array (after move-construct)", wptr_copy1_array, 0);
	print_weak_array_ptr_state("wptr_moved1_array (move-construct)", wptr_moved1_array, 2);
	assert(wptr_copy1_array.expired() && wptr_copy1_array.use_count() == 0);
	assert(!wptr_moved1_array.expired() && wptr_moved1_array.use_count() == 1);
	assert(wptr_moved1_array.lock()[0].id == 100);
	verify_active_objects("wptr_moved1_array move-construct", initial_active_objects + 2);

	raw::shared_ptr<TestObject[]> sptr_target_array = raw::make_shared<TestObject[]>(1);
	sptr_target_array[0].id							= 200;
	raw::weak_ptr<TestObject[]> wptr_target_copy_assign_array(sptr_target_array);
	print_weak_array_ptr_state("wptr_target_copy_assign_array (before copy-assign)",
							   wptr_target_copy_assign_array, 1);
	verify_active_objects("wptr_target_copy_assign_array created", initial_active_objects + 3);

	wptr_target_copy_assign_array = wptr_orig_array;
	print_weak_array_ptr_state("wptr_target_copy_assign_array (after copy-assign)",
							   wptr_target_copy_assign_array, 2);
	assert(!wptr_target_copy_assign_array.expired() &&
		   wptr_target_copy_assign_array.use_count() == 1);
	assert(wptr_target_copy_assign_array.lock()[0].id == 100);
	verify_active_objects("wptr_target_copy_assign_array copy-assign (old #200 is still alive)",
						  initial_active_objects + 3);

	raw::weak_ptr<TestObject[]> self_move_array = wptr_orig_array;
	print_weak_array_ptr_state("self_move_array (before self-move-assign)", self_move_array, 2);
	assert(!self_move_array.expired() && self_move_array.use_count() == 1);
	verify_active_objects("self_move_array created", initial_active_objects + 3);

	self_move_array = std::move(self_move_array);
	print_weak_array_ptr_state("self_move_array (after self-move-assign)", self_move_array, 2);
	assert(!self_move_array.expired() && self_move_array.use_count() == 1);
	verify_active_objects("self_move_array self-move-assign", initial_active_objects + 3);

	wptr_orig_array.reset();
	wptr_moved1_array.reset();
	wptr_target_copy_assign_array.reset();
	self_move_array.reset();
	verify_active_objects(
		"all weak_ptrs array reset (sptr_orig_array and sptr_target_array still exist)",
		initial_active_objects + 3);

	sptr_orig_array.reset();
	verify_active_objects("sptr_orig_array reset (objects 100,101 deleted)",
						  initial_active_objects + 1);
}

void test_weak_array_manipulation() {
	std::cout << "\n--- Test: Weak Array Manipulation ---\n";
	int initial_active_objects = s_active_test_objects;

	raw::shared_ptr<TestObject[]> sptr_array_1 = raw::make_shared<TestObject[]>(2);
	sptr_array_1[0].id						   = 1000;
	sptr_array_1[1].id						   = 1001;
	raw::weak_ptr<TestObject[]> wptr_array_1(sptr_array_1);
	print_weak_array_ptr_state("wptr_array_1 initial", wptr_array_1, 2);
	assert(!wptr_array_1.expired() && wptr_array_1.use_count() == 1);
	verify_active_objects("sptr_array_1, wptr_array_1 created", initial_active_objects + 2);

	wptr_array_1.reset();
	print_weak_array_ptr_state("wptr_array_1 after reset()", wptr_array_1, 0);
	assert(wptr_array_1.expired() && wptr_array_1.use_count() == 0);
	assert(sptr_array_1.use_count() == 1);
	verify_active_objects("wptr_array_1 reset (sptr_array_1 still exists)",
						  initial_active_objects + 2);

	wptr_array_1 = sptr_array_1;
	print_weak_array_ptr_state("wptr_array_1 after re-assignment from sptr_array_1", wptr_array_1,
							   2);
	assert(!wptr_array_1.expired() && wptr_array_1.use_count() == 1);
	verify_active_objects("wptr_array_1 reassigned", initial_active_objects + 2);

	raw::shared_ptr<TestObject[]> sptr_array_2 = raw::make_shared<TestObject[]>(1);
	sptr_array_2[0].id						   = 2000;
	raw::weak_ptr<TestObject[]> wptr_array_2(sptr_array_2);
	print_weak_array_ptr_state("wptr_array_2 initial", wptr_array_2, 1);
	assert(!wptr_array_2.expired() && wptr_array_2.use_count() == 1);
	verify_active_objects("sptr_array_2, wptr_array_2 created", initial_active_objects + 3);

	wptr_array_1.swap(wptr_array_2);
	print_weak_array_ptr_state("wptr_array_1 after swap", wptr_array_1, 1);
	print_weak_array_ptr_state("wptr_array_2 after swap", wptr_array_2, 2);
	assert(!wptr_array_1.expired() && wptr_array_1.use_count() == 1 &&
		   wptr_array_1.lock()[0].id == 2000);
	assert(!wptr_array_2.expired() && wptr_array_2.use_count() == 1 &&
		   wptr_array_2.lock()[0].id == 1000);
	verify_active_objects("wptr_array_1, wptr_array_2 after swap", initial_active_objects + 3);

	raw::shared_ptr<TestObject[]> locked_array_ptr = wptr_array_1.lock();
	print_weak_array_ptr_state("wptr_array_1 (after lock)", wptr_array_1, 1);
	assert(locked_array_ptr && locked_array_ptr[0].id == 2000 && locked_array_ptr.use_count() == 2);
	verify_active_objects("locked_array_ptr created", initial_active_objects + 3);

	locked_array_ptr.reset();
	print_weak_array_ptr_state("wptr_array_1 (after locked_array_ptr reset)", wptr_array_1, 1);
	assert(wptr_array_1.use_count() == 1);
	verify_active_objects("locked_array_ptr reset", initial_active_objects + 3);

	sptr_array_1.reset();
	print_weak_array_ptr_state("wptr_array_2 (after sptr_array_1 reset)", wptr_array_2, 0);
	assert(wptr_array_2.expired() && wptr_array_2.use_count() == 0);
	verify_active_objects("sptr_array_1 reset (objects #1000,1001 deleted)",
						  initial_active_objects + 1);

	sptr_array_2.reset();
	print_weak_array_ptr_state("wptr_array_1 (after sptr_array_2 reset)", wptr_array_1, 0);
	assert(wptr_array_1.expired() && wptr_array_1.use_count() == 0);
	verify_active_objects("sptr_array_2 reset (object #2000 deleted)", initial_active_objects);

	wptr_array_1.reset();
	wptr_array_2.reset();
	verify_active_objects("final cleanup", initial_active_objects);
}

void test_weak_interaction_with_shared() {
	std::cout << "\n--- Test: Weak Interaction with Shared ---\n";
	int initial_active_objects = s_active_test_objects;

	raw::weak_ptr<TestObject> wptr_A;
	{
		raw::shared_ptr<TestObject> sptr_A = raw::make_shared<TestObject>(10);
		wptr_A							   = sptr_A;
		print_weak_ptr_state("wptr_A (from sptr_A)", wptr_A);
		assert(!wptr_A.expired() && wptr_A.use_count() == 1);
		verify_active_objects("sptr_A, wptr_A created", initial_active_objects + 1);

		raw::shared_ptr<TestObject> sptr_B = sptr_A;
		print_weak_ptr_state("wptr_A (sptr_B copies sptr_A)", wptr_A);
		assert(!wptr_A.expired() && wptr_A.use_count() == 2);
		verify_active_objects("sptr_B created", initial_active_objects + 1);

		raw::weak_ptr<TestObject> wptr_C(sptr_B);
		print_weak_ptr_state("wptr_C (from sptr_B)", wptr_C);
		assert(!wptr_C.expired() && wptr_C.use_count() == 2);
		verify_active_objects("wptr_C created", initial_active_objects + 1);

		sptr_B.reset();
		print_weak_ptr_state("wptr_A (sptr_B reset)", wptr_A);
		print_weak_ptr_state("wptr_C (sptr_B reset)", wptr_C);
		assert(!wptr_A.expired() && wptr_A.use_count() == 1);
		assert(!wptr_C.expired() && wptr_C.use_count() == 1);
		verify_active_objects("sptr_B reset", initial_active_objects + 1);
	}
	print_weak_ptr_state("wptr_A (sptr_A out of scope)", wptr_A);
	assert(wptr_A.expired() && wptr_A.use_count() == 0);
	verify_active_objects("sptr_A out of scope (object #10 deleted)", initial_active_objects);

	raw::shared_ptr<TestObject> locked_expired_ptr = wptr_A.lock();
	assert(!locked_expired_ptr);
	verify_active_objects("locked_expired_ptr", initial_active_objects);

	raw::shared_ptr<TestObject[]> sptr_array_A = raw::make_shared<TestObject[]>(2);
	sptr_array_A[0].id						   = 100;
	sptr_array_A[1].id						   = 101;
	raw::weak_ptr<TestObject[]> wptr_array_A(sptr_array_A);
	print_weak_array_ptr_state("wptr_array_A (from sptr_array_A)", wptr_array_A, 2);
	assert(!wptr_array_A.expired() && wptr_array_A.use_count() == 1);
	verify_active_objects("sptr_array_A, wptr_array_A created", initial_active_objects + 2);

	raw::shared_ptr<TestObject[]> sptr_array_B = wptr_array_A.lock();
	print_weak_array_ptr_state("wptr_array_A (after lock)", wptr_array_A, 2);
	assert(sptr_array_B && sptr_array_B[0].id == 100 && sptr_array_B.use_count() == 2);
	assert(!wptr_array_A.expired() && wptr_array_A.use_count() == 2);
	verify_active_objects("sptr_array_B from lock", initial_active_objects + 2);

	sptr_array_A.reset();
	print_weak_array_ptr_state("wptr_array_A (sptr_array_A reset)", wptr_array_A, 2);
	assert(!wptr_array_A.expired() && wptr_array_A.use_count() == 1);
	verify_active_objects("sptr_array_A reset", initial_active_objects + 2);

	sptr_array_B.reset();
	print_weak_array_ptr_state("wptr_array_A (sptr_array_B reset)", wptr_array_A, 0);
	assert(wptr_array_A.expired() && wptr_array_A.use_count() == 0);
	verify_active_objects("sptr_array_B reset (objects #100,101 deleted)", initial_active_objects);
}

void stress_test_weak_ptr(int iterations, int max_pointers_in_pool) {
	std::cout << "\n--- Stress Test: Weak Ptr (" << iterations << " iterations) ---\n";
	std::random_device				rd;
	std::mt19937					gen(rd());
	std::uniform_int_distribution<> dist_op(0, 7);
	std::uniform_int_distribution<> dist_idx(0, max_pointers_in_pool - 1);
	std::uniform_int_distribution<> dist_val(0, 9999);
	std::uniform_int_distribution<> dist_array_size_gen(1, 10);

	std::vector<raw::shared_ptr<TestObject>>   s_pool_single(max_pointers_in_pool);
	std::vector<raw::weak_ptr<TestObject>>	   w_pool_single(max_pointers_in_pool);
	std::vector<raw::shared_ptr<TestObject[]>> s_pool_array(max_pointers_in_pool);
	std::vector<raw::weak_ptr<TestObject[]>>   w_pool_array(max_pointers_in_pool);
	std::vector<size_t>						   array_actual_sizes(max_pointers_in_pool, 0);

	auto start_time = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < iterations; ++i) {
		int op	 = dist_op(gen);
		int idx1 = dist_idx(gen);
		int idx2 = dist_idx(gen);

		try {
			switch (op) {
			case 0:
				s_pool_single[idx1] = raw::make_shared<TestObject>(dist_val(gen));
				w_pool_single[idx1] = s_pool_single[idx1];
				break;
			case 1:
				s_pool_single[idx1].reset();
				break;
			case 2:
				w_pool_single[idx1] = w_pool_single[idx2];
				break;
			case 3:
				w_pool_single[idx1] = std::move(w_pool_single[idx2]);
				break;
			case 4: {
				raw::shared_ptr<TestObject> locked_ptr = w_pool_single[idx1].lock();
				if (locked_ptr) {
					volatile int id_val = locked_ptr->id;
					(void)id_val;
				}
				break;
			}
			case 5: {
				size_t new_size			 = dist_array_size_gen(gen);
				s_pool_array[idx1]		 = raw::make_shared<TestObject[]>(new_size);
				array_actual_sizes[idx1] = new_size;
				if (s_pool_array[idx1]) {
					for (size_t k = 0; k < new_size; ++k) {
						s_pool_array[idx1][k].id = dist_val(gen);
					}
				}
				w_pool_array[idx1] = s_pool_array[idx1];
				break;
			}
			case 6:
				s_pool_array[idx1].reset();
				array_actual_sizes[idx1] = 0;
				break;
			case 7: {
				raw::shared_ptr<TestObject[]> locked_array_ptr = w_pool_array[idx1].lock();
				if (locked_array_ptr && array_actual_sizes[idx1] > 0) {
					size_t elem_idx =
						std::uniform_int_distribution<size_t>(0, array_actual_sizes[idx1] - 1)(gen);
					volatile int val = locked_array_ptr[elem_idx].id;
					(void)val;
				}
				break;
			}
			default:
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
		s_pool_single[i].reset();
		w_pool_single[i].reset();
		s_pool_array[i].reset();
		w_pool_array[i].reset();
	}
	verify_active_objects("Stress test cleanup", 0);
}

void run_all_weak_tests() {
	std::cout << "\nStarting weak_ptr tests...\n";
	int initial_active_objects = s_active_test_objects;

	test_weak_single_object_construction();
	verify_active_objects("After test_weak_single_object_construction", initial_active_objects);

	test_weak_single_object_copy_move_semantics();
	verify_active_objects("After test_weak_single_object_copy_move_semantics",
						  initial_active_objects);

	test_weak_single_object_manipulation();
	verify_active_objects("After test_weak_single_object_manipulation", initial_active_objects);

	test_weak_array_construction();
	verify_active_objects("After test_weak_array_construction", initial_active_objects);

	test_weak_array_copy_move_semantics();
	verify_active_objects("After test_weak_array_copy_move_semantics", initial_active_objects);

	test_weak_array_manipulation();
	verify_active_objects("After test_weak_array_manipulation", initial_active_objects);

	test_weak_interaction_with_shared();
	verify_active_objects("After test_weak_interaction_with_shared", initial_active_objects);

	stress_test_weak_ptr(100000, 100);
	verify_active_objects("After stress_test_weak_ptr", initial_active_objects);

	std::cout << "\nAll weak_ptr tests PASSED!.\n";
	verify_active_objects("Final check after all weak_ptr unit tests", 0);
}
