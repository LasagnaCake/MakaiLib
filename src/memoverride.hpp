#include <cstring>

#include "collection/errors.hpp"

void* operator new(size_t sz) {
	if (!sz) throw std::bad_alloc();
	if (void* mem = malloc(sz)) {
		memset(mem, 0, sz);
		return mem;
	}
	throw std::bad_alloc();
}

void* operator new[](size_t sz) {
	if (!sz) throw std::bad_alloc();
	if (void* mem = malloc(sz)) {
		memset(mem, 0, sz);
		return mem;
	}
	throw std::bad_alloc();
}

void operator delete(void* ptr) {
	if (!ptr) throw std::bad_alloc();
	free(ptr);
}

void operator delete[](void* ptr) {
	if (!ptr) throw std::bad_alloc();
	free(ptr);
}

void operator delete(void* ptr, size_t sz) {
	if (!ptr || !sz) throw std::bad_alloc();
	memset(ptr, 0, sz);
	free(ptr);
}

void operator delete[](void* ptr, size_t sz) {
	if (!ptr || !sz) throw std::bad_alloc();
	memset(ptr, 0, sz);
	free(ptr);
}
