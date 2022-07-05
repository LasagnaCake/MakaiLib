#ifndef GRAPHICAL_RENDERER_H
#define GRAPHICAL_RENDERER_H

#include <map>
#include <vector>
#include <stdexcept>
#include <string>

// #include <OGRE/Ogre.h>

#include <collection/vectorn.hpp>
#include <collection/entity/entity.hpp>

#define DERIVED_CLASS(NAME, BASE)\
	using	BASE :: BASE ;\
	inline	virtual string getClass() {return #NAME;}\
	inline	virtual string getBaseClass() {return #BASE;}\
	inline	static string getCoreClass() {return #NAME;}

namespace RenderData {
	namespace {
		using
		Vector::Vector2,
		Vector::Vector3,
		std::vector;
	}

	/**
	***********************************
	*                                 *
	*  Triangle Shape Data Structure  *
	*                                 *
	***********************************
	*/
	struct Triangle {
		/// The triangle's vertices.
		Vector3 verts[3] = {
			Vector3(0),
			Vector3(0),
			Vector3(0)
			};

		/// The triangle's UV coordinates;
		Vector3 uv[3] = {
			Vector3(0),
			Vector3(0),
			Vector3(0)
			};
	};

	/**
	***************************************
	*                                     *
	*  Renderable Polygon Data Structure  *
	*                                     *
	***************************************
	*/
	struct RenderPolygon {
		vector<Triangle> triangles;
	};

	std::map<size_t, vector<RenderPolygon*>> layers;
}

namespace EntityClass {
	namespace {
		using
		Vector::Vector2,
		Vector::Vector3;
	}

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
