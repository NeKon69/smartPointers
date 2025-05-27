//
// Created by progamers on 5/27/25.
//

#ifndef SMARTPOINTERS_RAW_NAMESPACE_FUNCTIONS_H
#define SMARTPOINTERS_RAW_NAMESPACE_FUNCTIONS_H

#endif // SMARTPOINTERS_RAW_NAMESPACE_FUNCTIONS_H

#include <shared_mutex>
#include <utility>

#include "raw_namespace_smart_ptr.h"

namespace raw {
template<typename T, typename... Args>
/**
 * @brief Creates a unique_ptr that manages a single object.
 * @param args Constructor arguments for the new object.
 */
unique_ptr<T> make_unique(Args... args) {
	return unique_ptr<T>(new T(args...));
}

/**
 * @brief Creates a unique_ptr that manages a static array.
 * @param size size of the array.
 */
template<typename T>
unique_ptr<T[]> make_unique(size_t size) {
	return unique_ptr<T[]>(new T[size]());
}

template<typename T, typename... Args>
/**
 * @brief Creates a shared_ptr that manages a single object.
 * @param args Constructor arguments for the new object.
 */
shared_ptr<T> make_shared(Args... args) {
	return unique_ptr<T>(new T(args...));
}
} // namespace raw