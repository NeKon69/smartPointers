//
// Created by progamers on 5/30/25.
//

#ifndef SMARTPOINTERS_SHARED_PTR_H
#define SMARTPOINTERS_SHARED_PTR_H

#include "helper.h"
#include "hub.h"
#include "smart_ptr_base.h"

namespace raw {
template<typename T>
class shared_ptr : public smart_ptr_base<T> {
protected:
	hub* hub_ptr = nullptr;

public:
	// Inherit constructors
	using smart_ptr_base<T>::smart_ptr_base;

	shared_ptr() noexcept = default;

	shared_ptr(nullptr_t) noexcept : smart_ptr_base<T>(nullptr), hub_ptr(nullptr) {}

	shared_ptr(T* p) noexcept {
		this->ptr = p;
		hub_ptr	  = new hub(this->ptr, nullptr, &raw::delete_single_object<T>,
							&raw::deallocate_hub_for_new_single);
		hub_ptr->increment_use_count();
	}

	shared_ptr(T* p, hub* h) noexcept {
		this->ptr = p;
		hub_ptr	  = h;
		hub_ptr->increment_use_count();
	}

	shared_ptr(const shared_ptr& other) noexcept {
		this->ptr = other.ptr;
		hub_ptr	  = other.hub_ptr;
		hub_ptr->increment_use_count();
	}

	shared_ptr(shared_ptr&& other) noexcept {
		this->ptr	  = other.ptr;
		hub_ptr		  = other.hub_ptr;
		other.ptr	  = nullptr;
		other.hub_ptr = nullptr;
	}
};
} // namespace raw

#endif // SMARTPOINTERS_SHARED_PTR_H
