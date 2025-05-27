//
// Created by progamers on 5/27/25.
//

#ifndef SMARTPOINTERS_SMART_PTR_BASE_H
#define SMARTPOINTERS_SMART_PTR_BASE_H

#include <cstdlib>
#include <exception>
#include <iostream>

#include "raw_namespace_smart_ptr.h"

namespace raw {
template<typename T>
class smart_ptr_base {
protected:
	T* ptr = nullptr;
	friend class unique_ptr<T>;
	friend class shared_ptr<T>;

public:
	constexpr smart_ptr_base() noexcept {}
	virtual ~smart_ptr_base() noexcept {
		if constexpr (std::is_array_v<T>) {
			std::cout << "Deleting array" << std::endl;
			delete[] ptr;
		} else {
			std::cout << "Deleting single object" << std::endl;
			delete ptr;
		}
	}

	explicit smart_ptr_base(T* p) {
		if (!p) {
			throw std::bad_alloc();
		}
		ptr = p;
	}

	explicit smart_ptr_base(T&& p) {
		if (!p) {
			throw std::bad_alloc();
		}
	}

	operator bool() const noexcept {
		return ptr != nullptr;
	}

	T* get() const noexcept {
		return ptr;
	}

	T& operator*() const noexcept {
		if (!ptr) {
			throw std::runtime_error("Dereferencing null pointer");
		}
		return *ptr;
	}

	T* operator->() const noexcept {
		if (!ptr) {
			throw std::runtime_error("Dereferencing null pointer");
		}
		return ptr;
	}
};
} // namespace raw

#endif // SMARTPOINTERS_SMART_PTR_BASE_H
