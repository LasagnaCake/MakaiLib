#ifndef OBJECT_MAIN_HEADER_H
#define OBJECT_MAIN_HEADER_H

#include "core.hpp"
#include "2d3d.hpp"
#include "collidable.hpp"

#undef ENTITY_CLASS
#undef DERIVED_CLASS

/// Derived class construction macro.
#define ENTITY_CLASS(NAME, BASE) \
	using Entities:: BASE :: BASE ;\
	using Entities:: BASE ::~ BASE ;\
	inline virtual std::string getClass() {return #NAME;}\
	inline virtual std::string getBaseClass() {return #BASE;}\
	inline static std::string getCoreClass() {return BASE::getCoreClass();}

namespace Entities {
}

#endif // OBJECT_MAIN_HEADER_H
