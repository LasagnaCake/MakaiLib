#ifndef CTL_EX_NAMESPACE_H
#define CTL_EX_NAMESPACE_H

#include "namespace.hpp"

#define CTL_EX_NAMESPACE_BEGIN	CTL_NAMESPACE_BEGIN namespace Ex {
#define CTL_EX_NAMESPACE_END	} CTL_NAMESPACE_END

CTL_EX_NAMESPACE_BEGIN

namespace Ex = ::CTL::Ex;

namespace {
	using namespace ::CTL;
}

CTL_EX_NAMESPACE_END

#endif // CTL_EX_NAMESPACE_H
