#ifndef GRAPHICAL_RENDERER_H
#define GRAPHICAL_RENDERER_H

#include <map>
#include <vector>
#include <stdexcept>

#ifdef _MAKAI_DEBUG_
#include <iostream>
#define $debug(TEXT) std::cout << TEXT << std::endl
#else
#define $debug(TEXT)
#endif // _MAKAI_DEBUG_

// #include <OGRE/Ogre.h>
// #include <irrlicht.h>

#include "collection/vectorn.hpp"
#include "collection/entity/entity.hpp"
#include "collection/event.hpp"

#define DERIVED_CLASS(NAME, BASE)\
	inline	virtual string getClass() {return #NAME;}\
	inline	virtual string getBaseClass() {return #BASE;}\
	inline	static string getCoreClass() {return #NAME;}

namespace VecMath {
	namespace {
		using
		Vector::Vector2,
		Vector::Vector3;
	}

	/// Base transformation data structure.
	template <class T>
	struct Transform {
		Transform();
		Transform(T position, T rotation, T scale) {
			this->position	= position;
			this->rotation	= rotation;
			this->scale		= scale;
		}
		T position;
		T rotation;
		T scale;
	};

	typedef Transform<Vector2> Transform2D;
	typedef Transform<Vector3> Transform3D;

	sf::Vector2f toSfVec2(Vector2 vec) {
		return sf::Vector2f(vec.x, vec.y);
	}

	sf::Vector3f toSfVec3(Vector3 vec) {
		return sf::Vector3f(vec.x, vec.y, vec.z);
	}

	Vector2 toVec2(sf::Vector2f vec) {
		return Vector2(vec.x, vec.y);
	}

	Vector3 toVec3(sf::Vector3f vec) {
		return Vector3(vec.x, vec.y, vec.z);
	}
}

namespace RenderData {
	namespace {
		using
		Vector::Vector2,
		Vector::Vector3,
		Vector::Vector4,
		std::vector;
	}
}

namespace EntityClass {

}

#undef DERIVED_CLASS

#endif // GRAPHICAL_RENDERER_H
