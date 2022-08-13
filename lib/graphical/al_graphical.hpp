#ifndef GRAPHICAL_RENDERER_H
#define GRAPHICAL_RENDERER_H

#include "anchors.hpp"

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

	inline ALLEGRO_COLOR toAllegroColor(VecV4 color) {
		return al_map_rgba_f(color.x, color.y, color.z, color.w);
	}

	inline ALLEGRO_COLOR toAllegroColor(VecV3 color) {
		return al_map_rgb_f(color.x, color.y, color.z);
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
		RenderHelper::toAllegroColor,
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

	ALLEGRO_VERTEX toAllegroVertex(Vertex v) {
		ALLEGRO_VERTEX res;
		res.x = v.position.x;
		res.y = v.position.y;
		res.z = v.position.z;
		res.u = v.uv.x;
		res.v = v.uv.y;
		res.color = toAllegroColor(v.color);
		return res;
	}

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
			ALLEGRO_BITMAP* texture,
			VecV2 uv[] = new VecV2[4],
			VecV4 color[] = new VecV4[4]
		) {
			this->transform = transform;
			this->texture	= texture;
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
		ALLEGRO_BITMAP* texture;
		VecV4 color[4];
		T verts[4];
		VecV2 uv[4];
	};

	typedef Triangle<VecV2>			Triangle2D;
	typedef Triangle<VecV3>			Triangle3D;

	typedef Plane<VecV2, float>		Plane2D;
	typedef Plane<VecV3, VecV3>		Plane3D;

	/// Returns a globalized triangle, based on the transforms of a given triangle
	template <class TRI, class TRANS>
	TRI transformed(TRI tri, TRANS transform) {
		#pragma GCC unroll 3
		for (unsigned char i = 0; i < 3; i++) {
			tri.verts[i] = VecMath::srpTransform(
				tri.verts[i],
				transform.position,
				transform.rotation,
				transform.scale
			);
		}
		return tri;
	}

	/// Converts a Triangle3D to a set of allegro vertexes. Must be manually deleted after use.
	ALLEGRO_VERTEX* asPrimitives(Triangle3D tri, bool useUV = true) {
		// Create vertexes
		ALLEGRO_VERTEX* verts = new ALLEGRO_VERTEX[3];
		// Assign vertex data
		#pragma GCC unroll 3
		for (unsigned char i = 0; i < 3; i++) {
			verts[i].x = tri.verts[i].x;
			verts[i].y = tri.verts[i].y;
			verts[i].z = tri.verts[i].z;
			verts[i].color = RenderHelper::toAllegroColor(tri.color[i]);
		}
		if (useUV) {
			#pragma GCC unroll 3
			for (unsigned char i = 0; i < 3; i++) {
				verts[i].u = tri.uv[i].x;
				verts[i].v = tri.uv[i].y;
			}
		}
		// Return vertex array
		return verts;
	}

	/// Converts a Plane3D to a set of allegro vertexes. Must be manually deleted after use.
	ALLEGRO_VERTEX* asPrimitives(Plane3D plane, bool useUV = true) {
		Plane3D pl = plane.globalized();
		// Create vertexes
		ALLEGRO_VERTEX* verts = new ALLEGRO_VERTEX[4];
		// Assign vertex data
		#pragma GCC unroll 4
		for (unsigned char i = 0; i < 4; i++) {
			verts[i].x = pl.verts[i].x;
			verts[i].y = pl.verts[i].y;
			verts[i].z = pl.verts[i].z;
			verts[i].color = al_map_rgba_f(
				pl.color[i].x,
				pl.color[i].y,
				pl.color[i].z,
				pl.color[i].w
			);
		}
		if (useUV) {
			#pragma GCC unroll 4
			for (unsigned char i = 0; i < 4; i++) {
				verts[i].u = pl.uv[i].x;
				verts[i].v = pl.uv[i].y;
			}
		}
		// Return vertex array
		return verts;
	}

//#pragma GCC pop_options

	/// Renders a set of vertexes as a triangle strip to the screen.
	void render3DStrip(ALLEGRO_VERTEX* verts, size_t vertCount, ALLEGRO_BITMAP* texture, bool autoDelete = true) {
		// Render triangle to screen
		$debug("Drawing shanpes...");
		al_draw_prim(
			verts,
			NULL,
			texture,
			0,
			vertCount,
			ALLEGRO_PRIM_TRIANGLE_STRIP
		);
		// If should clear vertexes, delete them
		if (autoDelete) delete[] verts;
	}
}

namespace EntityClass {

}

#undef UNROLL_LOOPS
#undef DERIVED_CLASS

#endif // GRAPHICAL_RENDERER_H
