#ifndef CTL_CPPFAILURE_H
#define CTL_CPPFAILURE_H

#include "namespace.hpp"

CTL_NAMESPACE_BEGIN

struct Failure {
	constexpr virtual char const* what() const noexcept {return "Something happened!";}
};

struct CatastrophicFailure: Failure {
	constexpr virtual char const* what() const noexcept {return "Something REALLY bad happened!";}
};

struct AllocationFailure:	CatastrophicFailure {
	constexpr virtual char const* what() const noexcept {return "Memory allocation failed!";}
};
struct MaximumSizeFailure:	CatastrophicFailure {
	constexpr virtual char const* what() const noexcept {return "Maximum size reached!";}
};

CTL_NAMESPACE_END

#endif // CTL_CPPFAILURE_H