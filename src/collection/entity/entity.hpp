#ifndef OBJECT_MAIN_HEADER_H
#define OBJECT_MAIN_HEADER_H

#include "core.hpp"
#include "2d3d.hpp"
#include "collision.hpp"

#undef ENTITY_CLASS
#undef DERIVED_CLASS

/// Derived class construction macro.
#define ENTITY_CLASS(NAME, BASE) \
	using EntityClass:: BASE :: BASE ;\
	using EntityClass:: BASE ::~ BASE ;\
	inline virtual string getClass() {return #NAME;}\
	inline virtual string getBaseClass() {return #BASE;}\
	inline static string getCoreClass() {return BASE::getCoreClass();}

namespace EntityClass {
}

#endif // OBJECT_MAIN_HEADER_H
