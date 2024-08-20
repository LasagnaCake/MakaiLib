#define GLEW_STATIC
#include <GLEW/include/GL/glew.h>
#include <GLEW/include/GL/wglew.h>
#include <GL/gl.h>

#define GET_GL_POINTER(start, offset) (void*)((start) + (offset) * sizeof(float))
#define GET_GL_OFFSET(offset) (void*)((offset) * sizeof(float))

#include "vertex.hpp"

using namespace Makai::Graph;

static Vertex Vertex::from(VertexMap vm) {
	return Vertex {
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

constexpr static Vertex Vertex::from(
	Vector3 const& position	= 0,
	Vector2 const& uv		= 0,
	Vector4 const& color	= 1,
	Vector3 const& normal	= 0
) {
	Vertex res;
	res.x	= position.x;
	res.y	= position.y;
	res.z	= position.z;
	res.u	= uv.x;
	res.v	= uv.y;
	res.r	= color.x;
	res.g	= color.y;
	res.b	= color.z;
	res.a	= color.w;
	res.nx	= normal.x;
	res.ny	= normal.y;
	res.nz	= normal.z;
	return res;
}

static void Vertex::setAttributes() {
	// Position
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex),
		GET_GL_OFFSET(0)
	);
	// UV
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex),
		GET_GL_OFFSET(3)
	);
	// Color
	glVertexAttribPointer(
		2,
		4,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex),
		GET_GL_OFFSET(5)
	);
	// Normal
	glVertexAttribPointer(
		3,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex),
		GET_GL_OFFSET(8)
	);
}

static void Vertex::enableAttributes() {
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
}

static void Vertex::disableAttributes() {
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
}

constexpr static void Vertex::setPosition(RawVertex& v, Vector3 const& pos) {
	v.x = pos.x;
	v.y = pos.y;
	v.z = pos.z;
}

constexpr static void Vertex::setUV(RawVertex& v, Vector2 const& uv) {
	v.u = uv.x;
	v.v = uv.y;
}

constexpr static void Vertex::setColor(RawVertex& v, Vector4 const& color) {
	v.r = color.x;
	v.g = color.y;
	v.b = color.z;
	v.a = color.w;
}

constexpr static void Vertex::setNormal(RawVertex& v, Vector3 const& n) {
	v.nx = n.x;
	v.ny = n.y;
	v.nz = n.z;
}

// Vertex Getters

constexpr static Vector3 Vertex::getPosition(RawVertex const& v) {
	return Vector3(v.x, v.y, v.z);
}

constexpr static Vector2 Vertex::getUV(RawVertex const& v) {
	return Vector2(v.u, v.v);
}

constexpr static Vector4 Vertex::getColor(RawVertex const& v) {
	return Vector4(v.r, v.g, v.b, v.a);
}

constexpr static Vector3 Vertex::getNormal(RawVertex const& v) {
	return Vector3(v.nx, v.ny, v.nz);
}
