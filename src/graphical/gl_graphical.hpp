#ifndef GRAPHICAL_RENDERER_H
#define GRAPHICAL_RENDERER_H

#include "../anchors.hpp"
#include "gl_shader.hpp"

#define $$FUNC function<void()>

#define $stride(start, offset) (void*)((start) + (offset) * sizeof(float))

#define DERIVED_CLASS(NAME, BASE)\
	inline	virtual string getClass() {return #NAME;}\
	inline	virtual string getBaseClass() {return #BASE;}\
	inline	static string getCoreClass() {return #NAME;}

namespace Drawer {
	namespace {
		//GLuint defBackBuffer

		using
		Vector::Vector2,
		Vector::Vector3,
		Vector::Vector4,
		std::vector;

		using namespace std;

		struct _VtxRaw {
			float x = 0;
			float y = 0;
			float z = 0;
			float u = 0;
			float v = 0;
			float r = 1;
			float g = 1;
			float b = 1;
			float a = 1;
		};

		struct _Vtx {
			_Vtx() {}
			_Vtx(Vector3 position, Vector4 color = Vector4(1), Vector2 uv = Vector2(0)) {
				this-> position	= position;
				this-> uv		= uv;
				this->color		= color;
			}
			Vector3 position;
			Vector2 uv;
			Vector4 color;
		};

		_Vtx* _toVtx(Vector3* pos, Vector2* uv, Vector4* col, size_t numVerts) {
			_Vtx* res = new _Vtx[numVerts];
			for(size_t i = 0; i < numVerts; i++) {
				res[i].position	= pos[i];
				if (uv) res[i].uv		= uv[i];
				if (col) res[i].color	= col[i];
				else	res[i].color	= Vector4(1);
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

		_VtxRaw* _toVtxRaw(Vector3* pos, Vector2* uv, Vector4* col, size_t numVerts) {
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

		_VtxRaw _toVtxRaw(Vector3 pos, Vector2 uv, Vector4 col) {
			_VtxRaw res;
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
	}

	typedef _Vtx	Vertex;
	typedef _VtxRaw	RawVertex;

	inline RawVertex toRawVertex(Vertex vert) {
		return _toVtxRaw(vert.position, vert.uv, vert.color);
	}

	typedef const function<void()> DrawFunc;

	Group::Group<DrawFunc> layers;

	void renderLayer(size_t layerID) {
		glEnable(GL_DEPTH_TEST);
		for (auto rFunc: layers[layerID]) {
			(*rFunc)();
		}
		glDisable(GL_DEPTH_TEST);
	}

	void renderAllLayers() {
		vector<size_t> rLayers = layers.getAllGroups();
		if (rLayers.size())
			for (auto layer : rLayers)
				renderLayer(layer);
	}
}

namespace VecMath {
	inline glm::vec3 asGLMVector(Vector3 vec) {
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	glm::mat4 asGLMMatrix(Transform3D transform) {
		// Create base matrix
		glm::mat4 matrix = glm::mat4(1.0f);
		// SRP Transform it
		matrix = glm::translate(
			matrix,
			asGLMVector(transform.position)
		);
		matrix = glm::rotate(
			matrix,
			1.0f,
			asGLMVector(transform.rotation)
		);
		matrix = glm::scale(
			matrix,
			asGLMVector(transform.scale)
		);
		// Return result
		return matrix;
	}
}

namespace RenderData {
	namespace {
		using
		Vector::Vector2,
		Vector::Vector3,
		Vector::Vector4,
		VecMath::Transform,
		VecMath::Transform2D,
		VecMath::Transform3D,
		VecMath::srpTransform,
		VecMath::asGLMMatrix,
		Drawer::Vertex,
		Drawer::toRawVertex,
		Drawer::DrawFunc,
		std::function,
		std::vector;
	}

	/// Base triangle data structure.
	template<class T>
	struct Triangle {
		Triangle() {}

		Triangle(
			T verts[3],
			Vector2 uv[3] = nullptr,
			Vector4 color[3] = nullptr
		) {
			#pragma GCC unroll 3
			for (unsigned char i = 0; i < 3; i++) {
				this->verts[i].position		= Vector3(verts[i]);
				this->verts[i].uv			= uv ? uv[i] : Vector2(1);
				this->verts[i].color		= color ? color[i] : Vector4(1);
			}
		}

		Triangle(Vertex verts[3]) {
			#pragma GCC unroll 3
			for (unsigned char i = 0; i < 3; i++)
				this->verts[i] = verts[i];
		}

		Vertex verts[3];
	};

	typedef Triangle<Vector2> Triangle2D;
	typedef Triangle<Vector3> Triangle3D;

	class Renderable {
	public:
		Renderable(size_t layer = 0) {
			Drawer::layers.addObject(&render, layer);
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			transform = Transform3D(
				Vector3(0.0f),
				Vector3(0.0f),
				Vector3(1.0f)
			);
			onCreate();
		}

		Renderable(vector<Triangle3D> triangles, size_t layer = 0)
		: Renderable(layer) {
			this->triangles = triangles;
		}

		~Renderable() {
			onDelete();
			glDeleteBuffers(1, &vbo);
			glDeleteVertexArrays(1, &vao);
		}

		/// Called on creation.
		virtual void onCreate()	{}
		/// Called before rendering to screen.
		virtual void onRender()	{}
		/// Called on deletion.
		virtual void onDelete()	{}

		Shader::ShaderList shaders;

		vector<Triangle3D> triangles;

		struct {
			GLuint culling	= GL_FRONT_AND_BACK;
			GLuint fill		= GL_FILL;
		} params;

		Transform3D transform;
	private:
		/// The object's Vertex Array Object (VAO).
		GLuint vao;

		/// The object's Vertex Buffer Object (VBO).
		GLuint vbo;

		/// The amount of vertices this object has.
		size_t vertexCount;

		/// Renders the object to the screen.
		DrawFunc render = $func() {
			// If no triangles exist, return
			if (!triangles.size()) return;
			// Call onRender function
			onRender();
			// Get vertex count
			vertexCount = triangles.size() * 3;
			// Create Intermediary Vertex Buffer (IVB) to be displayed on screen
			Drawer::RawVertex* verts = new Drawer::RawVertex[vertexCount];
			// Copy data to IVB
			size_t i = 0;
			for (auto t: triangles) {
				verts[i]	= toRawVertex(t.verts[0]);
				verts[i+1]	= toRawVertex(t.verts[1]);
				verts[i+2]	= toRawVertex(t.verts[2]);
				i += 3;
			}
			// Set VAO as active
			glBindVertexArray(vao);
			// Set VBO as active
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			// Copy IVB to VBO
			glBufferData(
				GL_ARRAY_BUFFER,
				vertexCount * 9,
				verts,
				GL_STATIC_DRAW
			);
			// Delete IVB, since it is no longer necessary
			delete [] verts;
			// Enable attribute pointers
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			// Define vertex data in VBO
			glVertexAttribPointer(
				0,
				3,
				GL_FLOAT,
				GL_FALSE,
				9 * sizeof(float),
				$stride(0, 0)
			);
			glVertexAttribPointer(
				1,
				2,
				GL_FLOAT,
				GL_FALSE,
				9 * sizeof(float),
				$stride(0, 3)
			);
			glVertexAttribPointer(
				2,
				4,
				GL_FLOAT,
				GL_FALSE,
				9 * sizeof(float),
				$stride(0, 5)
			);
			// Set VAO as active
			glBindVertexArray(vao);
			// Set polygon rendering mode
			glPolygonMode(params.culling, params.fill);
			// Draw object to screen
			draw();
			// Unbind vertex array
			glBindVertexArray(0);
			// Disable attributes
			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(0);
		};

		void draw() {
			// Render with basic shader
			Shader::defaultShader();
			Shader::defaultShader["world"](Scene::world);
			Shader::defaultShader["camera"](Scene::camera);
			Shader::defaultShader["projection"](Scene::projection);
			Shader::defaultShader["actor"](asGLMMatrix(transform));
			glDrawArrays(GL_TRIANGLES, 0, vertexCount);
			// Render object passes, if any
			if(shaders.size())
			for (auto s : shaders) {
				auto shader = (*s);
				// Enable shader
				shader();
				// Set matrices
				shader["world"](Scene::world);
				shader["camera"](Scene::camera);
				shader["projection"](Scene::projection);
				shader["actor"](asGLMMatrix(transform));
				// Draw object
				glDrawArrays(GL_TRIANGLES, 0, vertexCount);
			}
		};
	};
}

namespace EntityClass {

}

#undef $$FUNC

#undef UNROLL_LOOPS
#undef DERIVED_CLASS

#endif // GRAPHICAL_RENDERER_H
