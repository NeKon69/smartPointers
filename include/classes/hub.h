//
// Created by progamers on 5/27/25.
//

#ifndef SMARTPOINTERS_HUB_H
#define SMARTPOINTERS_HUB_H

#include <cstdio>

#include "raw_namespace_smart_ptr.h"

namespace raw {
template<typename T>
class hub {
private:
	size_t strong_ref_count = 0;
	size_t weak_ref_count	= 0;

public:
	hub() = default;

	void increment_strong_ref() {
		++strong_ref_count;
	}

	void decrement_strong_ref() {
		if (strong_ref_count > 0) {
			--strong_ref_count;
		}
	}

	size_t get_strong_ref_count() const {
		return strong_ref_count;
	}

	void increment_weak_ref() {
		++weak_ref_count;
	}

	void decrement_weak_ref() {
		if (weak_ref_count > 0) {
			--weak_ref_count;
		}
	}

	size_t get_weak_ref_count() const {
		return weak_ref_count;
	}
};
} // namespace raw

#endif // SMARTPOINTERS_HUB_H
