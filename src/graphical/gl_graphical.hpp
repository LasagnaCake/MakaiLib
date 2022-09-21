#ifndef GRAPHICAL_RENDERER_H
#define GRAPHICAL_RENDERER_H

#include "../anchors.hpp"
#include "gl_shader.hpp"

#define $$FUNC function<void()>

#define $glpointer(start, offset) (void*)((start) + (offset) * sizeof(float))
#define $gloffset(offset) (void*)((offset) * sizeof(float))

#define DERIVED_CONSTRUCTOR(NAME, BASE, CODE)\
	NAME(string name = #NAME ) : BASE(name) CODE\
	NAME(Entity* parent, string name = #NAME , bool uniqueEntity = true) : BASE(parent, name, uniqueEntity) CODE

#define DERIVED_CLASS(NAME, BASE)\
	inline	virtual string getClass() {return #NAME;}\
	inline	virtual string getBaseClass() {return #BASE;}\
	inline	static string getCoreClass() {return #NAME;}

#define glSetClearColor(COLOR) glClearColor(COLOR.x, COLOR.y, COLOR.z, COLOR.w);

#include "gl_color.hpp"

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

	#define RAW_VERTEX_SIZE 9
	#define RAW_VERTEX_BYTE_SIZE RAW_VERTEX_SIZE * sizeof(float)
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
			this-> position	= position;
			this-> uv		= uv;
			this->color		= color;
		}

		Vector3 position;
		Vector2 uv;
		Vector4 color = Color::WHITE;
	};

	RawVertex toRawVertex(Vector3 pos, Vector2 uv, Vector4 col) {
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

	RawVertex toRawVertex(Vertex vert) {
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

	void clearColorBuffer(Vector4 color) {
		glSetClearColor(color);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void clearDepthBuffer() {
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	#include "gl_texture.hpp"
}

namespace VecMath {
	inline glm::vec3 asGLMVector(Vector3 vec) {
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	inline Vector3 asVector(glm::vec3 vec) {
		return Vector3(vec.x, vec.y, vec.z);
	}

	inline Vector3 glmRotateV3(Vector3 vec, Vector3 angle) {
		glm::vec3 res = asGLMVector(vec);
		res = glm::rotateZ(res, angle.z);
		res = glm::rotateY(res, angle.y);
		res = glm::rotateX(res, angle.x);
		return asVector(res);
	}

	// I fucking hate this, but it works, so fuckit.
	inline Vector3 glmSrpTransform(Vector3 vec, Vector3 pos, Vector3 rot, Vector3 scale = Vector3(1.0)) {
		vec *= scale;
		vec = glmRotateV3(vec, rot);
		vec += pos;
		return vec;
	}

	inline Vector3 glmSrpTransform(Vector3 vec, Transform3D trans) {
		return glmSrpTransform(
			vec,
			trans.position,
			trans.rotation,
			trans.scale
		);
	}
}
#ifndef SRP_TRANSFORM
#define SRP_TRANSFORM glmSrpTransform
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
		VecMath::SRP_TRANSFORM,
		Drawer::Vertex,
		Drawer::RawVertex,
		Drawer::toRawVertex,
		Drawer::DrawFunc,
		std::function,
		std::vector,
		std::runtime_error;
	}

	/// Base triangle data structure.
	struct Triangle {
		Triangle() {
			#pragma GCC unroll 3
			for (unsigned char i = 0; i < 3; i++) {
				this->verts[i].position	= Vector3(0.0);
				this->verts[i].uv		= Vector2(0.0);
				this->verts[i].color	= Vector4(1.0);
			}
		}

		Triangle(
			Vector3 verts[3],
			Vector2 uv[3] = nullptr,
			Vector4 color[3] = nullptr
		) {
			#pragma GCC unroll 3
			for (unsigned char i = 0; i < 3; i++) {
				this->verts[i].position	= verts[i];
				this->verts[i].uv		= uv != nullptr		? uv[i]		: Vector2(0.0);
				this->verts[i].color	= color != nullptr	? color[i]	: Color::WHITE;
			}
		}

		Triangle(Vertex verts[3]) {
			#pragma GCC unroll 3
			for (unsigned char i = 0; i < 3; i++)
				this->verts[i] = verts[i];
		}

		Triangle transformed(Transform3D trans) {
			Vertex res[3];
			#pragma GCC unroll 3
			for (unsigned char i = 0; i < 3; i++) {
				res[i].position = SRP_TRANSFORM(verts[i].position, trans);
				res[i].uv		= verts[i].uv;
				res[i].color	= verts[i].color;
			}
			return Triangle(res);
		}

		Vertex verts[3];
	};

	namespace Reference {
		#include "gl_reference.hpp"
	}

	#include "gl_renderable.hpp"
}

#include "gl_framebuffer.hpp"

namespace EntityClass {
	class RenderableEntity2D: public Entity2D {
	public:

		DERIVED_CONSTRUCTOR(RenderableEntity2D, Entity2D, {})
		DERIVED_CLASS(RenderableEntity2D, Entity2D)

		virtual void onFrame(float delta) {
			mesh.transform.global.position		= Vector3(globalPosition(), zIndex);
			mesh.transform.global.rotation.z	= globalRotation();
			mesh.transform.global.scale			= Vector3(globalScale(), 1);
		}

		float zIndex = 0;

		RenderData::Renderable mesh;
	};
}

#undef $$FUNC

#undef UNROLL_LOOPS
#undef DERIVED_CLASS

#endif // GRAPHICAL_RENDERER_H
