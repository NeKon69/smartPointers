//
// Created by progamers on 5/27/25.
//

#ifndef SMARTPOINTERS_HUB_H
#define SMARTPOINTERS_HUB_H

#include <atomic>
#include <cstdio>

#include "fwd.h"

namespace raw {
class hub {
public:
	std::atomic<size_t> use_count;
	std::atomic<size_t> weak_count;

	void*	   managed_object_ptr;
	std::byte* allocated_base_block;

	void (*destroy_obj_func)(void*);
	void (*deallocate_mem_func)(void*, void*);

	// Конструктор hub'а
	hub(void* obj_ptr, std::byte* base_block, void (*destroyer)(void*),
		void (*deallocator)(void*, void*)) noexcept
		: use_count(1),
		  weak_count(0),
		  managed_object_ptr(obj_ptr),
		  allocated_base_block(base_block),
		  destroy_obj_func(destroyer),
		  deallocate_mem_func(deallocator) {}
	~hub() = default;

	void increment_use_count() noexcept {
		use_count++;
	}
	void decrement_use_count() noexcept {
		use_count--;
		if (use_count == 0) {
			if (destroy_obj_func) {
				destroy_obj_func(managed_object_ptr);
				managed_object_ptr = nullptr;
			}
			if (weak_count == 0) {
				if (deallocate_mem_func) {
					deallocate_mem_func(this, allocated_base_block);
				}
			}
		}
	}

	void increment_weak_count() noexcept {
		weak_count++;
	}
	void decrement_weak_count() noexcept {
		weak_count--;
		if (use_count == 0 && weak_count == 0) {
			if (deallocate_mem_func) {
				deallocate_mem_func(this, allocated_base_block);
			}
		}
	}
};

} // namespace raw

#endif // SMARTPOINTERS_HUB_H
