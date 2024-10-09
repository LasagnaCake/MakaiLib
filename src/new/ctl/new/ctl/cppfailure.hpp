#ifndef CTL_CPPFAILURE_H
#define CTL_CPPFAILURE_H

#include "namespace.hpp"
#include "ctypes.hpp"
#include <stdexcept>

CTL_NAMESPACE_BEGIN

struct Crash {};

template<usize I>
struct DebugCrash: Crash {};

struct Failure {
	virtual char const* what() const noexcept {return "Something happened!";}
};

struct CatastrophicFailure: Failure, Crash {
	virtual char const* what() const noexcept {return "Something REALLY bad happened!";}
};

struct AllocationFailure: CatastrophicFailure {
	virtual char const* what() const noexcept {return "Memory allocation failed!";}
};

struct MaximumSizeFailure: CatastrophicFailure {
	virtual char const* what() const noexcept {return "Maximum size reached!";}
};

struct ConstructionFailure: CatastrophicFailure {
	virtual char const* what() const noexcept {return "Failed to construct type!";}
};

void panic() {
	throw Crash();
}

CTL_NAMESPACE_END

#endif // CTL_CPPFAILURE_H