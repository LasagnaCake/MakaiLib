#ifndef GRAPHICAL_RENDERER_H
#define GRAPHICAL_RENDERER_H

#include "../anchors.hpp"
#include "gl_shader.hpp"

#define $$FUNC function<void()>

#define $glpointer(start, offset) (void*)((start) + (offset) * sizeof(float))
#define $gloffset(offset) (void*)((offset) * sizeof(float))

#define glSetClearColor(COLOR) glClearColor(COLOR.x, COLOR.y, COLOR.z, COLOR.w);

#include "gl_color.hpp"

#define DEFAULT_BLEND_FUNC GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA
#define DEFAULT_BLEND_EQUATION GL_FUNC_ADD, GL_FUNC_ADD

namespace Drawer {
	namespace {
		//GLuint defBackBuffer

		using
		Vector::Vector2,
		Vector::Vector3,
		Vector::Vector4,
		VecMath::Transform3D,
		VecMath::Transform2D,
		VecMath::srpTransform,
		std::vector;

		using namespace std;
	}

	#define RAW_VERTEX_SIZE (sizeof(RawVertex) / sizeof(float))
	#define RAW_VERTEX_BYTE_SIZE sizeof(RawVertex)
	struct RawVertex {
		float
			x = 0,
			y = 0,
			z = 0,
			u = 0,
			v = 0,
			r = 1,
			g = 1,
			b = 1,
			a = 1;
	};

	struct Vertex {
		Vertex() {}

		Vertex(Vector3 position, Vector4 color = Vector4(1), Vector2 uv = Vector2(0)) {
			this-> position		= position;
			this-> uv			= uv;
			this-> color		= color;
		}

		Vertex(const Vertex& v) {
			this-> position		= v.position;
			this-> uv			= v.uv;
			this-> color		= v.color;
		}

		virtual ~Vertex() {}

		Vector3 position;
		Vector2 uv;
		Vector4 color = Vector4(1);
	};

	RawVertex toRawVertex(Vector3 pos, Vector2 uv, Vector4 col = Vector4(1)) {
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
		return res;
	}

	RawVertex toRawVertex(Vertex& vert) {
		RawVertex res;
		res.x = vert.position.x;
		res.y = vert.position.y;
		res.z = vert.position.z;
		res.u = vert.uv.x;
		res.v = vert.uv.y;
		res.r = vert.color.x;
		res.g = vert.color.y;
		res.b = vert.color.z;
		res.a = vert.color.w;
		return res;
	}

	void copyToRawVertex(RawVertex* target, Vertex vert) {
		target->x = vert.position.x;
		target->y = vert.position.y;
		target->z = vert.position.z;
		target->u = vert.uv.x;
		target->v = vert.uv.y;
		target->r = vert.color.x;
		target->g = vert.color.y;
		target->b = vert.color.z;
		target->a = vert.color.w;
	}

	typedef const function<void()> DrawFunc;

	Group::Group<DrawFunc*> layers;

	void renderLayer(size_t layerID) {
		for (auto rFunc: layers[layerID]) {
			(*rFunc)();
		}
	}

	void renderAllLayers() {
		vector<size_t> rLayers = layers.getAllGroups();
		if (rLayers.size())
			for (auto layer : rLayers) {
				glClear(GL_DEPTH_BUFFER_BIT);
				renderLayer(layer);
			}
	}

	void setVertexAttributes() {
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			RAW_VERTEX_BYTE_SIZE,
			$gloffset(0)
		);
		glVertexAttribPointer(
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			RAW_VERTEX_BYTE_SIZE,
			$gloffset(3)
		);
		glVertexAttribPointer(
			2,
			4,
			GL_FLOAT,
			GL_FALSE,
			RAW_VERTEX_BYTE_SIZE,
			$gloffset(5)
		);
	}

	inline void clearColorBuffer(Vector4 color) {
		glSetClearColor(color);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	inline void clearDepthBuffer() {
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	inline void vertexSetColor(RawVertex& v, Vector4 color) {
		v.r = color.x;
		v.g = color.y;
		v.b = color.z;
		v.a = color.w;
	}

	inline void vertexSetPosition(RawVertex& v, Vector3 pos) {
		v.x = pos.x;
		v.y = pos.y;
		v.z = pos.z;
	}

	inline void vertexSetUV(RawVertex& v, Vector2 uv) {
		v.u = uv.x;
		v.v = uv.y;
	}

	inline Vector4 vertexGetColor(RawVertex& v) {
		return Vector4(v.r, v.g, v.b, v.a);
	}

	inline Vector3 vertexGetPosition(RawVertex& v) {
		return Vector3(v.x, v.y, v.z);
	}

	inline Vector2 vertexGetUV(RawVertex& v) {
		return Vector2(v.u, v.v);
	}

	#include "gl_texture.hpp"
}

namespace VecMath {
	using Drawer::RawVertex;

	inline glm::vec3 asGLMVector(Vector3& vec) {
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	inline Vector3 asVector(glm::vec3& vec) {
		return Vector3(vec.x, vec.y, vec.z);
	}

	inline glm::vec4 asGLMVector(Vector4& vec) {
		return glm::vec4(vec.x, vec.y, vec.z, vec.w);
	}

	inline Vector4 asVector(glm::vec4& vec) {
		return Vector4(vec.x, vec.y, vec.z, vec.w);
	}

	inline Vector3 asVector3(glm::vec4& vec) {
		return Vector3(vec.x, vec.y, vec.z);
	}

	inline Vector3 glmRotateV3(Vector3& vec, Vector3 angle) {
		glm::vec3 res = asGLMVector(vec);
		res = glm::rotateZ(res, angle.z);
		res = glm::rotateY(res, angle.y);
		res = glm::rotateX(res, angle.x);
		return asVector(res);
	}

	inline glm::mat4 rotationMatrix(Vector3& rotation) {
		return glm::eulerAngleYXZ(
			rotation.y,
			rotation.x,
			rotation.z
		);
	}

	glm::mat4 asGLMMatrix(Transform3D& trans) {
		glm::mat4 res(1.0f);
		res = glm::translate(res, asGLMVector(trans.position));
		res = glm::scale(res, asGLMVector(trans.scale));
		res *= rotationMatrix(trans.rotation);
		return res;
	}

	Vector3 srpTransform(Vector3& vec, glm::mat4& matrix) {
		glm::vec4 res = glm::vec4(vec.x, vec.y, vec.z, 1.0f) * matrix;
		res += glm::vec4(glm::vec3(matrix[3]), 0.0);
		return Vector3(res.x, res.y, res.z);
	}

	inline Vector3 glmSrpTransform(Vector3& vec, Transform3D& trans) {
		auto tmat = asGLMMatrix(trans);
		return srpTransform(vec, tmat);
	}

	/*RawVertex srpTransform(RawVertex& vtx, glm::mat4& matrix) {
		glm::vec4 res = glm::vec4(vtx.x, vtx.y, vtx.z, 1.0f) * matrix;
		res += glm::vec4(glm::vec3(matrix[3]), 0.0);
		return RawVertex{res.x, res.y, res.z, vtx.u, vtx.v, vtx.r, vtx.g, vtx.b, vtx.a};
	}
	*/

	void srpTransform(RawVertex& vtx, glm::mat4& matrix) {
		glm::vec4 res = glm::vec4(vtx.x, vtx.y, vtx.z, 1.0f) * matrix;
		res += glm::vec4(glm::vec3(matrix[3]), 0.0);
		Drawer::vertexSetPosition(vtx, Vector3(res.x, res.y, res.z));
	}

	inline void glmSrpTransform(RawVertex& vtx, Transform3D& trans) {
		auto tmat = asGLMMatrix(trans);
		srpTransform(vtx, tmat);
	}
}
#ifndef $srpTransform
#define $srpTransform srpTransform
#endif // SRP_TRANSFORM

namespace RenderData {
	namespace {
		using
		Vector::Vector2,
		Vector::Vector3,
		Vector::Vector4,
		VecMath::Transform,
		VecMath::Transform2D,
		VecMath::Transform3D,
		VecMath::$srpTransform,
		VecMath::asGLMVector,
		VecMath::asVector3,
		VecMath::asGLMMatrix,
		Drawer::Vertex,
		Drawer::RawVertex,
		Drawer::toRawVertex,
		Drawer::DrawFunc,
		Drawer::Texture2D,
		Drawer::vertexGetPosition,
		std::function,
		std::vector,
		std::string,
		std::runtime_error;
	}

	/// Base triangle data structure.
	struct Triangle {
		RawVertex verts[3];
	};

	class Renderable;

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
}

#include "gl_framebuffer.hpp"

#define $ref	RenderData::Reference::
#define $rdt	RenderData::
#define $drw	Drawer::

#undef $$FUNC

#endif // GRAPHICAL_RENDERER_H
