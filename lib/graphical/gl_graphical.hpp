#ifndef GRAPHICAL_RENDERER_H
#define GRAPHICAL_RENDERER_H

#include "../anchors.hpp"

#define DERIVED_CLASS(NAME, BASE)\
	inline	virtual string getClass() {return #NAME;}\
	inline	virtual string getBaseClass() {return #BASE;}\
	inline	static string getCoreClass() {return #NAME;}

//#pragma GCC push_options
//#pragma GCC optimize ("unroll-loops")

namespace RenderHelper {
	namespace {
		using
		Vector::VecV2,
		Vector::VecV3,
		Vector::VecV4,
		std::vector;
	}
}

namespace RenderData {
	namespace {
		using
		Vector::VecV2,
		Vector::VecV3,
		Vector::VecV4,
		VecMath::Transform,
		VecMath::Transform2D,
		VecMath::Transform3D,
		VecMath::srpTransform,
		std::vector;
	}

	struct Vertex {
		Vertex() {}
		Vertex(VecV3 position, VecV4 color = VecV4(1), VecV2 uv = VecV2(0)) {
			this-> position	= position;
			this-> uv		= uv;
			this->color		= color;
		}
		VecV3 position;
		VecV2 uv;
		VecV4 color;
	};

	/// Base triangle data structure.
	template<class T>
	struct Triangle {
		Triangle() {}
		Triangle(
			T verts[3],
			VecV2 uv[3] = nullptr,
			VecV4 color[3] = nullptr
		) {
			#pragma GCC unroll 3
			for (unsigned char i = 0; i < 3; i++) {
				this->verts[i]	= verts[i];
				this->uv[i]		= uv ? uv[i] : VecV2(1);
				this->color[i]	= color ? color[i] : VecV4(1);
			}
		}

		VecV4 color[3];
		T verts[3];
		VecV2 uv[3];
	};

	/// Base plane data structure. Organized in the order of: TL, TR, BL, BR.
	template<class T, typename ROT_T>
	struct Plane {
		Plane() {}
		Plane(
			Transform<T, ROT_T> transform,
			T verts[4],
			VecV2 uv[] = new VecV2[4],
			VecV4 color[] = new VecV4[4]
		) {
			this->transform = transform;
			#pragma GCC unroll 4
			for (unsigned char i = 0; i < 4; i++) {
				this->verts[i]	= verts[i];
				this->uv[i]		= uv ? uv[i] : VecV2(1);
				this->color[i]	= color ? color[i] : VecV4(1);
			}
		};
		Plane<T, ROT_T> globalized() {
			Plane<T, ROT_T> res;
			#pragma GCC unroll 4
			for (unsigned char i = 0; i < 4; i++)
				res.verts[i] = srpTransform(verts[i], transform);
			return res;
		}
		Transform<T, ROT_T> transform;
		VecV4 color[4];
		T verts[4];
		VecV2 uv[4];
	};

	typedef Triangle<VecV2>			Triangle2D;
	typedef Triangle<VecV3>			Triangle3D;

	typedef Plane<VecV2, float>		Plane2D;
	typedef Plane<VecV3, VecV3>		Plane3D;
}

namespace EntityClass {

}

#undef UNROLL_LOOPS
#undef DERIVED_CLASS

#endif // GRAPHICAL_RENDERER_H
