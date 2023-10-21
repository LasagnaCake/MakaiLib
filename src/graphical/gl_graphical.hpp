#ifndef GRAPHICAL_RENDERER_H
#define GRAPHICAL_RENDERER_H

#include "../anchors.hpp"
#include "gl_shader.hpp"

#define GET_GL_POINTER(start, offset) (void*)((start) + (offset) * sizeof(float))
#define GET_GL_OFFSET(offset) (void*)((offset) * sizeof(float))

#define glSetClearColor(COLOR) glClearColor(COLOR.x, COLOR.y, COLOR.z, COLOR.w);

#include "gl_color.hpp"

#ifndef DEFAULT_BLEND_FUNC
//#define DEFAULT_BLEND_FUNC GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO
//#define DEFAULT_BLEND_FUNC GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA
//#define DEFAULT_BLEND_FUNC GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA
//#define DEFAULT_BLEND_FUNC GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA
//#define DEFAULT_BLEND_FUNC GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA
//#define DEFAULT_BLEND_FUNC GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE
//#define DEFAULT_BLEND_FUNC GL_ONE, GL_ONE, GL_ONE, GL_ONE
#define DEFAULT_BLEND_FUNC GL_SRC_ALPHA, GL_SRC_ALPHA, GL_DST_ALPHA, GL_DST_ALPHA
//#define DEFAULT_BLEND_FUNC GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE
//#define DEFAULT_BLEND_FUNC GL_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE, GL_ONE
#endif // DEFAULT_BLEND_FUNC

#ifndef DEFAULT_BLEND_EQUATION
//#define DEFAULT_BLEND_EQUATION GL_FUNC_ADD, GL_FUNC_REVERSE_SUBTRACT
#define DEFAULT_BLEND_EQUATION GL_FUNC_ADD, GL_FUNC_ADD
#endif // DEFAULT_BLEND_EQUATION

#ifndef GRAPHICAL_PARALLEL_THREAD_COUNT
#define GRAPHICAL_PARALLEL_THREAD_COUNT PARALLEL_THREAD_COUNT
#endif // GRAPHICAL_PARALLEL_THREAD_COUNT

#ifndef GRAPHICAL_PARALLEL_FOR
#define GRAPHICAL_PARALLEL_FOR PRAGMA_PARALLEL_FOR(GRAPHICAL_PARALLEL_THREAD_COUNT)
#endif // GRAPHICAL_PARALLEL_FOR

namespace Drawer {
	namespace {
		//GLuint defBackBuffer

		using
		VecMath::Transform3D,
		VecMath::Transform2D,
		VecMath::srpTransform,
		std::vector;

		using namespace std;
	}

	#define RAW_VERTEX_SIZE (sizeof(RawVertex) / sizeof(float))
	#define RAW_VERTEX_BYTE_SIZE sizeof(RawVertex)
	#define RAW_VERTEX_COMPONENTS "x,y,z,u,v,r,g,b,a,nx,ny,nz"
	struct RawVertex {
		float
			x	= 0,
			y	= 0,
			z	= 0,
			u	= 0,
			v	= 0,
			r	= 1,
			g	= 1,
			b	= 1,
			a	= 1,
			nx	= 0,
			ny	= 0,
			nz	= 1;
	};

	typedef std::map<string, float> VertexMap;

	const VertexMap baseVertexMap = {
		{"x",0},
		{"y",0},
		{"z",0},
		{"u",0},
		{"v",0},
		{"r",1},
		{"g",1},
		{"b",1},
		{"a",1},
		{"nx",0},
		{"ny",0},
		{"nz",1}
	};

	RawVertex toRawVertex(VertexMap vm = baseVertexMap) {
		return RawVertex {
			vm["x"],
			vm["y"],
			vm["z"],
			vm["u"],
			vm["v"],
			vm["r"],
			vm["g"],
			vm["b"],
			vm["a"],
			vm["nx"],
			vm["ny"],
			vm["nz"]
		};
	}

	RawVertex toRawVertex(Vector3 pos, Vector2 uv, Vector4 col = Vector4(1), Vector3 norm = Vector3(0)) {
		RawVertex res;
		res.x = pos.x;
		res.y = pos.y;
		res.z = pos.z;
		res.u = uv.x;
		res.v = uv.y;
		res.r = col.x;
		res.g = col.y;
		res.b = col.z;
		res.a = col.w;
		res.nx = norm.x;
		res.ny = norm.y;
		res.nz = norm.z;
		return res;
	}

	typedef function<void()> const DrawFunc;

	Group::Group<DrawFunc*> layers;

	void renderLayer(size_t layerID) {
		for (auto rFunc: layers[layerID]) {
			(*rFunc)();
		}
	}

	void setVertexAttributes() {
		// Position
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			RAW_VERTEX_BYTE_SIZE,
			GET_GL_OFFSET(0)
		);
		// UV
		glVertexAttribPointer(
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			RAW_VERTEX_BYTE_SIZE,
			GET_GL_OFFSET(3)
		);
		// Color
		glVertexAttribPointer(
			2,
			4,
			GL_FLOAT,
			GL_FALSE,
			RAW_VERTEX_BYTE_SIZE,
			GET_GL_OFFSET(5)
		);
		// Normal
		glVertexAttribPointer(
			3,
			3,
			GL_FLOAT,
			GL_FALSE,
			RAW_VERTEX_BYTE_SIZE,
			GET_GL_OFFSET(8)
		);
	}

	inline void enableVertexAttributes() {
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
	}

	inline void disableVertexAttributes() {
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
	}

	inline void clearColorBuffer(Vector4 color) {
		glSetClearColor(color);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	inline void clearDepthBuffer() {
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	// Vertex Setters

	inline void vertexSetPosition(RawVertex& v, Vector3 pos) {
		v.x = pos.x;
		v.y = pos.y;
		v.z = pos.z;
	}

	inline void vertexSetUV(RawVertex& v, Vector2 uv) {
		v.u = uv.x;
		v.v = uv.y;
	}

	inline void vertexSetColor(RawVertex& v, Vector4 color) {
		v.r = color.x;
		v.g = color.y;
		v.b = color.z;
		v.a = color.w;
	}

	inline void vertexSetNormal(RawVertex& v, Vector3 n) {
		v.nx = n.x;
		v.ny = n.y;
		v.nz = n.z;
	}

	// Vertex Getters

	inline Vector3 vertexGetPosition(RawVertex& v) {
		return Vector3(v.x, v.y, v.z);
	}

	inline Vector2 vertexGetUV(RawVertex& v) {
		return Vector2(v.u, v.v);
	}

	inline Vector4 vertexGetColor(RawVertex& v) {
		return Vector4(v.r, v.g, v.b, v.a);
	}

	inline Vector3 vertexGetNormal(RawVertex& v) {
		return Vector3(v.nx, v.ny, v.nz);
	}

	#include "gl_texture.hpp"
}

namespace VecMath {
	using Drawer::RawVertex;

	void srpTransform(RawVertex& vtx, Matrix4x4 const& tmat) {
		// Position
		Vector3 res = (tmat * Vector4(vtx.x, vtx.y, vtx.z, 1.0f)).toVector3();
		Drawer::vertexSetPosition(vtx, res);
		// Normal
		res = (tmat * Vector4(vtx.nx, vtx.ny, vtx.nz, 1.0f)).toVector3();
		Drawer::vertexSetNormal(vtx, res);
	}

	inline void srpTransform(RawVertex& vtx, Transform3D const& trans) {
		srpTransform(vtx, Matrix4x4(trans));
	}
}

namespace RenderData {
	namespace {
		using
		VecMath::Transform,
		VecMath::Transform2D,
		VecMath::Transform3D,
		VecMath::srpTransform,
		Drawer::RawVertex,
		Drawer::toRawVertex,
		Drawer::DrawFunc,
		Drawer::Texture2D,
		Drawer::vertexGetPosition,
		std::function,
		std::vector,
		std::string;
	}

	/// Base triangle data structure.
	struct Triangle {
		RawVertex verts[3];
	};

	class Renderable;

	#define _ENCDEC_CASE(T, F) if (encoding == T) return F(data)
	List<ubyte> decodeData(String const& data, String const& encoding) {
		_ENCDEC_CASE	("base32",	cppcodec::base32_rfc4648::decode);
		_ENCDEC_CASE	("base64",	cppcodec::base64_rfc4648::decode);
		throw Error::InvalidValue(
			"Invalid encoding: " + encoding,
			__FILE__,
			toString(__LINE__),
			"decodeData"
		);
	}

	String encodeData(List<ubyte> const& data, String const& encoding) {
		_ENCDEC_CASE	("base32",	cppcodec::base32_rfc4648::encode);
		_ENCDEC_CASE	("base64",	cppcodec::base64_rfc4648::encode);
		throw Error::InvalidValue(
			"Invalid encoding: " + encoding,
			__FILE__,
			toString(__LINE__),
			"decodeData"
		);
	}
	#undef _ENCDEC_CASE

	namespace Material {
		#include "gl_material.hpp"
	}

	namespace Base {
		#include "gl_drawable.hpp"
	}

	namespace Reference {
		#include "gl_reference.hpp"
	}

	#include "gl_renderable.hpp"

	namespace Text {
		#include "gl_text.hpp"
	}

	namespace Bar {
		#include "gl_progressbar.hpp"
	}

	#include "gl_scene.hpp"
}

#include "gl_framebuffer.hpp"

#endif // GRAPHICAL_RENDERER_H
