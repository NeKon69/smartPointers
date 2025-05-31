//
// Created by progamers on 5/30/25.
//

#ifndef SMARTPOINTERS_UNIT_UNIQUE_H
#define SMARTPOINTERS_UNIT_UNIQUE_H

#pragma once

#include <random>
#include <vector>

#include "../../include/raw_memory.h"
#include "common_test_utils.h"

template<typename T>
void print_ptr_state(const std::string& name, const raw::unique_ptr<T>& ptr);

template<typename T>
void print_array_ptr_state(const std::string& name, const raw::unique_ptr<T[]>& ptr, size_t size);

void test_single_object_construction();
void test_single_object_move_semantics();
void test_single_object_manipulation();
void test_array_construction();
void test_array_move_semantics();
void test_array_manipulation();

void stress_test_unique_ptr(int iterations, int max_pointers_in_pool = 100);

void run_all_unique_tests();

#endif // SMARTPOINTERS_UNIT_UNIQUE_H
