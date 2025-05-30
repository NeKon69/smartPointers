//
// Created by progamers on 5/27/25.
//

#ifndef SMARTPOINTERS_HELPER_H
#define SMARTPOINTERS_HELPER_H

#include <exception>
#include <utility>

#include "fwd.h"
#include "hub.h"

// Struct to emulate shared_ptr's internal structure
template<typename T>
struct combined {
	T		 ptr;
	raw::hub hub_ptr;
};

namespace raw {

template<typename T>
static void delete_single_object(void* obj_ptr) {
	delete static_cast<T*>(obj_ptr);
}

template<typename T>
static void delete_array_object(void* obj_ptr) {
	delete[] static_cast<T*>(obj_ptr);
}

template<typename T>
static void destroy_make_shared_object(void* obj_ptr) {
	static_cast<T*>(obj_ptr)->~T();
}

static void deallocate_hub_for_new_single(void* hub_ptr, void*) {
	delete static_cast<hub*>(hub_ptr);
}

static void deallocate_hub_for_new_array(void* hub_ptr, void*) {
	delete static_cast<hub*>(hub_ptr);
}

static void deallocate_make_shared_block(void*, void* base_block_ptr) {
	std::free(base_block_ptr);
}

/**
 * @brief Creates a unique_ptr that manages a static array.
 * @param size size of the array.
 */
template<typename T>
std::enable_if_t<std::is_array_v<T>, raw::unique_ptr<T>> make_unique(size_t size) {
	using element_type = std::remove_extent_t<T>;
	return raw::unique_ptr<T>(new element_type[size]());
}

template<typename T, typename... Args>
/**
 * @brief Creates a unique_ptr that manages a single object.
 * @param args Constructor arguments for the new object.
 */
std::enable_if_t<!std::is_array_v<T>, raw::unique_ptr<T>> make_unique(Args&&... args) {
	return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<typename T, typename... Args>
/**
 * @brief Creates a shared_ptr that manages a single object.
 * @param args Constructor arguments for the new object.
 */
shared_ptr<T> make_shared(Args&&... args) {
	std::byte* raw_block =
		static_cast<std::byte*>(std::aligned_alloc(alignof(combined<T>), sizeof(combined<T>)));
	if (!raw_block) {
		throw std::bad_alloc();
	}
	T*	 constructed_ptr = nullptr;
	hub* constructed_hub = nullptr;
	try {
		constructed_ptr =
			new (raw_block + offsetof(combined<T>, ptr)) T(std::forward<Args>(args)...);
		constructed_hub = new (raw_block + offsetof(combined<T>, hub_ptr))
			hub(constructed_ptr, raw_block, &destroy_make_shared_object<T>,
				deallocate_make_shared_block);
	} catch (const std::exception& e) {
		if (constructed_ptr) {
			constructed_ptr->~T();
		}
		std::free(raw_block);
		throw;
	}

	return shared_ptr<T>(constructed_ptr, constructed_hub);
}

} // namespace raw

#endif // SMARTPOINTERS_HELPER_H