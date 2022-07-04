#ifndef GRAPHICAL_RENDERER_H
#define GRAPHICAL_RENDERER_H

// #include <OGRE/Ogre.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <collection/vectorn.hpp>
#include <collection/entity/entity.hpp>

#define DERIVED_CLASS(NAME, BASE)\
	using	BASE :: BASE ;\
	inline	virtual string getClass() {return #NAME;}\
	inline	virtual string getBaseClass() {return #BASE;}\
	inline	static string getCoreClass() {return #NAME;}

namespace EntityClass {
	namespace {
		using
		Vector::Vector2,
		Vector::Vector3;
	}

	struct Triangle {
		Vector3 verts[4];
	};

	struct Plane {
		Vector3 verts[4];
	};

	/**
	*********************************
	*                               *
	*  Base Graphical Object Class  *
	*                               *
	*********************************
	*/
}

#undef DERIVED_CLASS

#endif // GRAPHICAL_RENDERER_H
