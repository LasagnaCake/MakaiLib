#ifndef GRAPHICAL_RENDERER_H
#define GRAPHICAL_RENDERER_H

#include "../anchors.hpp"

#define DERIVED_CLASS(NAME, BASE)\
	inline	virtual string getClass() {return #NAME;}\
	inline	virtual string getBaseClass() {return #BASE;}\
	inline	static string getCoreClass() {return #NAME;}

//#pragma GCC push_options
//#pragma GCC optimize ("unroll-loops")

namespace Drawer {
	namespace {
		//GLuint defBackBuffer

		using
		Vector::VecV2,
		Vector::VecV3,
		Vector::VecV4,
		std::vector;

		struct _VtxRaw {
			float x, y, z, u, v, r, g, b, a;
		};

		struct _Vtx {
			_Vtx() {}
			_Vtx(VecV3 position, VecV4 color = VecV4(1), VecV2 uv = VecV2(0)) {
				this-> position	= position;
				this-> uv		= uv;
				this->color		= color;
			}
			VecV3 position;
			VecV2 uv;
			VecV4 color;
		};

		_Vtx* _toVtx(VecV3* pos, VecV2* uv, VecV4* col, size_t numVerts) {
			_Vtx* res = new _Vtx[numVerts];
			for(size_t i = 0; i < numVerts; i++) {
				res[i].position	= pos[i];
				if (uv) res[i].uv		= uv[i];
				if (col) res[i].color	= col[i];
				else	res[i].color	= VecV4(1);
			}
			return res;
		}

		_VtxRaw* _toVtxRaw(_Vtx* verts, size_t numVerts) {
			_VtxRaw* res = new _VtxRaw[numVerts];
			for(size_t i = 0; i < numVerts; i++) {
				res[i].x = verts[i].position.x;
				res[i].y = verts[i].position.y;
				res[i].z = verts[i].position.z;
				res[i].u = verts[i].uv.x;
				res[i].v = verts[i].uv.y;
				res[i].r = verts[i].color.x;
				res[i].g = verts[i].color.y;
				res[i].b = verts[i].color.z;
				res[i].a = verts[i].color.w;
			}
			return res;
		}

		_VtxRaw* _toVtxRaw(VecV3* pos, VecV2* uv, VecV4* col, size_t numVerts) {
			_VtxRaw* res = new _VtxRaw[numVerts];
			for(size_t i = 0; i < numVerts; i++) {
				res[i].x = pos[i].x;
				res[i].y = pos[i].y;
				res[i].z = pos[i].z;
				res[i].u = uv[i].x;
				res[i].v = uv[i].y;
				res[i].r = col[i].x;
				res[i].g = col[i].y;
				res[i].b = col[i].z;
				res[i].a = col[i].w;
			}
			return res;
		}

		typedef GLuint _GLTex;
		typedef GLuint _GLBuf;

		void _renderRaw(
			_VtxRaw* verts,
			_GLTex image,
			_GLBuf buffer,
			size_t start,
			size_t count,
			int glType
		) {
			if(buffer)
				glBindBuffer(GL_ARRAY_BUFFER, buffer);

			glDrawArrays(glType, start, count);

			if(buffer)
				glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}

	typedef _Vtx	Vertex;
	typedef _VtxRaw	RawVertex;
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
