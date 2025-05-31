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

	void (*destroy_obj_func)(void*, size_t);
	void (*deallocate_mem_func)(void*, void*);

	size_t obj_size;

	// Конструктор hub'а
	hub(void* obj_ptr, std::byte*				  base_block, void (*destroyer)(void*, size_t),
		void (*deallocator)(void*, void*), size_t size = 0) noexcept
		: use_count(1),
		  weak_count(0),
		  managed_object_ptr(obj_ptr),
		  allocated_base_block(base_block),
		  destroy_obj_func(destroyer),
		  deallocate_mem_func(deallocator),
		  obj_size(size) {}
	~hub() = default;

	inline void increment_use_count() noexcept {
		use_count.fetch_add(1, std::memory_order_relaxed);
	}
	inline void decrement_use_count() noexcept {
		if (use_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
			if (destroy_obj_func) {
				destroy_obj_func(managed_object_ptr, obj_size);
				managed_object_ptr = nullptr;
			}
			if (weak_count.load(std::memory_order_acquire) == 0) {
				if (deallocate_mem_func) {
					deallocate_mem_func(this, allocated_base_block);
				}
			}
		}
	}

	inline void increment_weak_count() noexcept {
		weak_count.fetch_add(1, std::memory_order_relaxed);
	}
	inline void decrement_weak_count() noexcept {
		if (weak_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
			if (deallocate_mem_func) {
				deallocate_mem_func(this, allocated_base_block);
			}
		}
	}

	inline void set_managed_object_ptr(void* obj_ptr) noexcept {
		managed_object_ptr = obj_ptr;
	}
};

} // namespace raw

#endif // SMARTPOINTERS_HUB_H
