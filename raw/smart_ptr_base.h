//
// Created by progamers on 5/27/25.
//

#ifndef SMARTPOINTERS_SMART_PTR_BASE_H
#define SMARTPOINTERS_SMART_PTR_BASE_H

#include <iostream>

#include "fwd.h"

namespace raw {
template<typename T>
class smart_ptr_base {
protected:
	T* ptr = nullptr;
	friend class unique_ptr<T>;
	friend class shared_ptr<T>;

public:
	constexpr smart_ptr_base() = default;
	explicit smart_ptr_base(nullptr_t) noexcept : ptr(nullptr) {}

	explicit smart_ptr_base(T* p) noexcept {
		ptr = p;
	}

	explicit operator bool() const noexcept {
		return ptr != nullptr;
	}

	T* get() const noexcept {
		return ptr;
	}

	T& operator*() const noexcept {
		return *ptr;
	}

	T* operator->() const noexcept {
		return ptr;
	}
};

template<typename T>
class smart_ptr_base<T[]> {
protected:
	T* ptr = nullptr;
	friend class unique_ptr<T[]>;
	friend class shared_ptr<T[]>;

public:
	constexpr smart_ptr_base() = default;
	explicit smart_ptr_base(nullptr_t) noexcept : ptr(nullptr) {}

	explicit smart_ptr_base(T* p) noexcept {
		ptr = p;
	}

	explicit operator bool() const noexcept {
		return ptr != nullptr;
	}

	T* get() const noexcept {
		return ptr;
	}

	T& operator[](size_t index) const noexcept {
		return ptr[index];
	}
};
} // namespace raw

#endif // SMARTPOINTERS_SMART_PTR_BASE_H
