//
// Created by progamers on 5/27/25.
//

#ifndef SMARTPOINTERS_RAW_NAMESPACE_FUNCTIONS_H
#define SMARTPOINTERS_RAW_NAMESPACE_FUNCTIONS_H

#include <shared_mutex>
#include <type_traits>
#include <utility>

#include "raw_namespace_smart_ptr.h"

namespace raw {

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
	return shared_ptr<T>(new T(std::forward<Args>(args)...));
}
} // namespace raw

#endif // SMARTPOINTERS_RAW_NAMESPACE_FUNCTIONS_H