#ifndef GRAPHICAL_RENDERER_H
#define GRAPHICAL_RENDERER_H

#include "../anchors.hpp"
#include "gl_shader.hpp"

#define $$FUNC function<void()>

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

		using namespace std;

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

		_VtxRaw _toVtxRaw(VecV3 pos, VecV2 uv, VecV4 col) {
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

	inline RawVertex toRawVertex(Vertex vert) {
		return _toVtxRaw(vert.position, vert.uv, vert.color);
	}

	typedef const function<void()> DrawFunc;

	Group::Group<DrawFunc> renderLayers;

	void renderLayer(size_t layerID) {
		for (auto rFunc: renderLayers[layerID]) {
			(*rFunc)();
		}
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
		matrix = glm::scale(
			matrix,
			asGLMVector(transform.scale)
		);
		matrix = glm::rotate(
			matrix,
			1.0f,
			asGLMVector(transform.rotation)
		);
		matrix = glm::translate(
			matrix,
			asGLMVector(transform.position)
		);
		// Return result
		return matrix;
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
			VecV2 uv[3] = nullptr,
			VecV4 color[3] = nullptr
		) {
			#pragma GCC unroll 3
			for (unsigned char i = 0; i < 3; i++) {
				this->verts[i].position		= verts[i];
				this->verts[i].uv			= uv ? uv[i] : VecV2(1);
				this->verts[i].color		= color ? color[i] : VecV4(1);
			}
		}

		Triangle(Vertex verts[3]) {
			#pragma GCC unroll 3
			for (unsigned char i = 0; i < 3; i++)
				this->verts[i] = verts[i];
		}

		Vertex verts[3];
	};

	class Renderable {
	public:
		Renderable(size_t layer = 0) {
			Drawer::renderLayers.addObject(&render, layer);
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			onCreate();
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
		vector<Triangle<Vector3>> triangles;

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
			// Call onRender function
			onRender();
			// Set VBO as active
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
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
			// Copy IVB to VBO
			glBufferData(
				GL_ARRAY_BUFFER,
				vertexCount * 5,
				verts,
				GL_STATIC_DRAW
			);
			// Delete IVB, since it is no longer necessary
			delete [] verts;
			// Define vertex data in VBO
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(
				0,
				3,
				GL_FLOAT,
				GL_FALSE,
				9 * sizeof(float),
				(void*)(0)
			);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(
				1,
				2,
				GL_FLOAT,
				GL_FALSE,
				9 * sizeof(float),
				(void*)(3 * sizeof(float))
			);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(
				2,
				4,
				GL_FLOAT,
				GL_FALSE,
				9 * sizeof(float),
				(void*)(5 * sizeof(float))
			);
			// Render object passes
			for (auto s : shaders) {
				// Enable shader
				s();
				// Set VAO as active
				glBindVertexArray(vao);
				// Set polygon mode
				glPolygonMode(params.culling, params.fill);
				// Set matrices
				s["world"](Scene::world);
				s["camera"](Scene::camera);
				s["projection"](Scene::projection);
				s["actor"](asGLMMatrix(transform));
				// Draw object
				draw();
			}
			// Unbind vertex Array
			glBindVertexArray(0);
		};

		void draw() {
			glDrawArrays(GL_TRIANGLES, 0, vertexCount * 5);
		};
	};
}

namespace EntityClass {

}

#undef $$FUNC

#undef UNROLL_LOOPS
#undef DERIVED_CLASS

#endif // GRAPHICAL_RENDERER_H
