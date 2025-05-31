//
// Created by progamers on 5/31/25.
//

#ifndef SMARTPOINTERS_UNIT_WEAK_H
#define SMARTPOINTERS_UNIT_WEAK_H

#include <memory>
#include <random>
#include <vector>

#include "../../include/raw_memory.h"
#include "common_test_utils.h"

template<typename T>
void print_weak_ptr_state(const std::string& name, const raw::weak_ptr<T>& ptr);

template<typename T>
void print_weak_array_ptr_state(const std::string& name, const raw::weak_ptr<T[]>& ptr,
								size_t size_hint);

void test_weak_single_object_construction();
void test_weak_single_object_copy_move_semantics();
void test_weak_single_object_manipulation();
void test_weak_array_construction();
void test_weak_array_copy_move_semantics();
void test_weak_array_manipulation();
void test_weak_interaction_with_shared();

void stress_test_weak_ptr(int iterations, int max_pointers_in_pool = 100);

void run_all_weak_tests();

#endif // SMARTPOINTERS_UNIT_WEAK_H
