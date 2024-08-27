#define GLEW_STATIC
#include <GLEW/include/GL/glew.h>
#include <GLEW/include/GL/wglew.h>
#include <GL/gl.h>

#define GET_GL_POINTER(start, offset) (void*)((start) + (offset) * sizeof(float))
#define GET_GL_OFFSET(offset) (void*)((offset) * sizeof(float))

#include "vertex.hpp"

using namespace Makai::Graph;

Vertex Vertex::from(VertexMap vm) {
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

void Vertex::setAttributes() {
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

void Vertex::enableAttributes() {
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
}

void Vertex::disableAttributes() {
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
}
