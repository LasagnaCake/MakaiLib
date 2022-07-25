#ifndef GRAPHICAL_RENDERER_H
#define GRAPHICAL_RENDERER_H

#include "anchors.hpp"

#define DERIVED_CLASS(NAME, BASE)\
	inline	virtual string getClass() {return #NAME;}\
	inline	virtual string getBaseClass() {return #BASE;}\
	inline	static string getCoreClass() {return #NAME;}

namespace VecMath {
	namespace {
		using
		Vector::VecV2,
		Vector::VecV3;
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

	typedef Transform<VecV2> Transform2D;
	typedef Transform<VecV3> Transform3D;

	sf::Vector2f toSfVec2(VecV2 vec) {
		return sf::Vector2f(vec.x, vec.y);
	}

	sf::Vector3f toSfVec3(VecV3 vec) {
		return sf::Vector3f(vec.x, vec.y, vec.z);
	}

	VecV2 toVec2(sf::Vector2f vec) {
		return VecV2(vec.x, vec.y);
	}

	VecV3 toVec3(sf::Vector3f vec) {
		return VecV3(vec.x, vec.y, vec.z);
	}
}

namespace RenderData {
	namespace {
		using
		Vector::VecV2,
		Vector::VecV3,
		Vector::VecV4,
		std::vector;
	}
}

namespace EntityClass {

}

#undef DERIVED_CLASS

#endif // GRAPHICAL_RENDERER_H
