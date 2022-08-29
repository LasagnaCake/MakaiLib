#ifndef GRAPHICAL_RENDERER_H
#define GRAPHICAL_RENDERER_H

#include "../anchors.hpp"
#include "gl_shader.hpp"

#define $$FUNC function<void()>

#define $stride(start, offset) (void*)((start) + (offset) * sizeof(float))
#define $abstride(offset) (void*)((offset) * sizeof(float))

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
		VecMath::Transform3D,
		VecMath::srpTransform,
		std::vector;

		using namespace std;
	}

	#define RAW_VERTEX_SIZE 9
	struct RawVertex {
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

	struct Vertex {
		Vertex() {}
		Vertex(Vector3 position, Vector4 color = Vector4(1), Vector2 uv = Vector2(0)) {
			this-> position	= position;
			this-> uv		= uv;
			this->color		= color;
		}

		Vector3 position;
		Vector2 uv;
		Vector4 color;
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

	inline RawVertex toRawVertex(Vertex vert) {
		return toRawVertex(vert.position, vert.uv, vert.color);
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
		Drawer::RawVertex,
		Drawer::toRawVertex,
		Drawer::DrawFunc,
		std::function,
		std::vector;
	}

	/// Base triangle data structure.
	struct Triangle {
		Triangle() {}

		Triangle(
			Vector3 verts[3],
			Vector2 uv[3] = nullptr,
			Vector4 color[3] = nullptr
		) {
			#pragma GCC unroll 3
			for (unsigned char i = 0; i < 3; i++) {
				this->verts[i].position		= verts	? verts[i]	: Vector2(0.0);
				this->verts[i].uv			= uv	? uv[i]		: Vector2(0.0);
				this->verts[i].color		= color	? color[i]	: Vector4(1.0);
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
			for (unsigned char i = 0; i < 3; i++)
				res[i].position = srpTransform(verts[i].position, trans);
			return Triangle(res);
		}

		Vertex verts[3];
	};

	class PlaneReference {
	public:
		PlaneReference(
			Vertex* tl,
			Vertex* tr1,
			Vertex* tr2,
			Vertex* bl1,
			Vertex* bl2,
			Vertex* br
		) {
			this->tl	= tl;
			this->tr1	= tr1;
			this->tr2	= tr2;
			this->bl1	= bl1;
			this->bl2	= bl2;
			this->br	= br;
		}

		PlaneReference* setPosition(
				Vector3 tlPos,
				Vector3 trPos,
				Vector3 blPos,
				Vector3 brPos
			) {
			tl->position	= tlPos;
			tr1->position	= trPos;
			tr2->position	= trPos;
			bl1->position	= blPos;
			bl2->position	= blPos;
			br->position	= brPos;
			return this;
		}

		PlaneReference* setUV(
				Vector2 tlUV,
				Vector2 trUV,
				Vector2 blUV,
				Vector2 brUV
			) {
			tl->uv	= tlUV;
			tr1->uv	= trUV;
			tr2->uv	= trUV;
			bl1->uv	= blUV;
			bl2->uv	= blUV;
			br->uv	= brUV;
			return this;
		}

		PlaneReference* setColor(
				Vector4 tlCol,
				Vector4 trCol,
				Vector4 blCol,
				Vector4 brCol
			) {
			tl->color	= tlCol;
			tr1->color	= trCol;
			tr2->color	= trCol;
			bl1->color	= blCol;
			bl2->color	= blCol;
			br->color	= brCol;
			return this;
		}

		PlaneReference* setColor(
				Vector4 col = Vector4(1.0)
			) {
			tl->color	= col;
			tr1->color	= col;
			tr2->color	= col;
			bl1->color	= col;
			bl2->color	= col;
			br->color	= col;
			return this;
		}

		PlaneReference* transform(Transform3D trans) {
			tl->position	= srpTransform(tl->position, trans);
			tr1->position	= srpTransform(tr1->position, trans);
			tr2->position	= srpTransform(tr2->position, trans);
			bl1->position	= srpTransform(bl1->position, trans);
			bl2->position	= srpTransform(bl2->position, trans);
			br->position	= srpTransform(br->position, trans);
			return this;
		}

	private:

		Vertex* tl;
		Vertex* tr1;
		Vertex* tr2;
		Vertex* bl1;
		Vertex* bl2;
		Vertex* br;
	};

	class Renderable {
	public:
		Renderable(size_t layer = 0, bool manual = false) {
			if(!manual)
				Drawer::layers.addObject(&render, layer);
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			onCreate();
			manualMode = manual;
		}

		Renderable(vector<Triangle*> triangles, size_t layer = 0, bool manual = false)
		: Renderable(layer, manual) {
			this->triangles = triangles;
		}

		~Renderable() {
			onDelete();
			glDeleteBuffers(1, &vbo);
			glDeleteVertexArrays(1, &vao);
			for (auto t: triangles)
				delete t;
			for (auto pr: references.plane)
				delete pr;
			if(!manualMode)
				Drawer::layers.removeFromAll(&render);
		}

		/// Called on creation.
		virtual void onCreate()	{}
		/// Called before rendering to screen.
		virtual void onRender()	{}
		/// Called on deletion.
		virtual void onDelete()	{}

		PlaneReference* createPlaneReference() {
			// Add triangles
			triangles.push_back(new Triangle());
			triangles.push_back(new Triangle());
			// Get index of last plane
			size_t last = triangles.size();
			// Create reference
			PlaneReference* plane = new PlaneReference(
				&(triangles[last-1]->verts[0]),
				&(triangles[last-1]->verts[1]),
				&(triangles[last-2]->verts[0]),
				&(triangles[last-1]->verts[2]),
				&(triangles[last-2]->verts[1]),
				&(triangles[last-2]->verts[2])
			);
			// Setup plane
			plane->setPosition(
				Vector3(-1.0, +1.0, 0.0),
				Vector3(+1.0, +1.0, 0.0),
				Vector3(-1.0, -1.0, 0.0),
				Vector3(+1.0, -1.0, 0.0)
			);
			plane->setUV(
				Vector2(-1.0, +1.0),
				Vector2(+1.0, +1.0),
				Vector2(-1.0, -1.0),
				Vector2(+1.0, -1.0)
			);
			// Add to reference list
			references.plane.push_back(plane);
			// return plane
			return plane;
		}

		/// Renders the object to the screen.
		DrawFunc render = $func() {
			// If not active, return
			if (!params.active) return;
			// If no triangles exist, return
			if (!triangles.size()) return;
			// Call onRender function
			onRender();
			// Pad array
			Triangle padding;
			triangles.push_back(&padding);
			triangles.push_back(&padding);
			// Get vertex count
			vertexCount = triangles.size() * 3;
			// Create Intermediary Vertex Buffer (IVB) to be displayed on screen
			RawVertex* verts = new RawVertex[(vertexCount)];
			// Copy data to IVB
			size_t i = 0;
			for (auto t: triangles) {
				auto tri = (*t)
					.transformed(transform.local)
					.transformed(transform.global);
				verts[i]	= toRawVertex(tri.verts[0]);
				verts[i+1]	= toRawVertex(tri.verts[1]);
				verts[i+2]	= toRawVertex(tri.verts[2]);
				i += 3;
			}
			// Set VBO as active
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			// Copy IVB to VBO
			glBufferData(
				GL_ARRAY_BUFFER,
				vertexCount * RAW_VERTEX_SIZE * 2,
				verts,
				GL_STATIC_DRAW
			);
			// Delete IVB, since it is no longer necessary
			delete [] verts;
			// Set VAO as active
			glBindVertexArray(vao);
			// Define vertex data in VBO
			glVertexAttribPointer(
				0,
				3,
				GL_FLOAT,
				GL_FALSE,
				RAW_VERTEX_SIZE * sizeof(float),
				$abstride(0)
			);
			glVertexAttribPointer(
				1,
				2,
				GL_FLOAT,
				GL_FALSE,
				RAW_VERTEX_SIZE * sizeof(float),
				$abstride(3)
			);
			glVertexAttribPointer(
				2,
				4,
				GL_FLOAT,
				GL_FALSE,
				RAW_VERTEX_SIZE * sizeof(float),
				$abstride(5)
			);
			// Enable attribute pointers
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
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
			// Unpad array
			triangles.pop_back();
			triangles.pop_back();
		};

		Shader::ShaderList shaders;

		vector<Triangle*> triangles;

		struct {
			bool active = true;
			GLuint culling	= GL_FRONT_AND_BACK;
			GLuint fill		= GL_FILL;
		} params;

		struct {
			Transform3D global;
			Transform3D local;
		} transform;
	private:
		/// List of references linked to this object.
		struct {
			vector<PlaneReference*> plane;
		} references;

		/// Whether manually rendering or not.
		bool manualMode = false;

		/// The object's Vertex Array Object (VAO).
		GLuint vao;

		/// The object's Vertex Buffer Object (VBO).
		GLuint vbo;

		/// The amount of vertices this object has.
		size_t vertexCount;

		void draw() {
			// Render with basic shader
			Shader::defaultShader();
			glm::mat4 camera = Scene::camera.matrix();
			glm::mat4 projection = Scene::camera.perspective();
			Shader::defaultShader["world"](Scene::world);
			Shader::defaultShader["camera"](camera);
			Shader::defaultShader["projection"](projection);
			//Shader::defaultShader["actor"](asGLMMatrix(transform));
			glDrawArrays(GL_TRIANGLES, 0, vertexCount);
			// Render object passes, if any
			if(shaders.size())
			for (auto s : shaders) {
				auto shader = (*s);
				// Enable shader
				shader();
				// Draw object
				glDrawArrays(GL_TRIANGLES, 0, vertexCount);
			}
		};
	};
}

namespace Drawer {
	class FrameBuffer {
	public:
		FrameBuffer() {}

		FrameBuffer(
			unsigned int width,
			unsigned int height
		): FrameBuffer() {
			glGenFramebuffers(1, &id);
			target = new RenderData::Renderable(0, true);
			rect = target->createPlaneReference();
			glGenTextures(1, &texture.color);
			glBindTexture(GL_TEXTURE_2D, texture.color);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA16F,
				width,
				height,
				0,
				GL_RGBA16F,
				GL_FLOAT,
				NULL
			);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glGenTextures(1, &texture.depth);
			glBindTexture(GL_TEXTURE_2D, texture.depth);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_DEPTH24_STENCIL8,
				width,
				height,
				0,
				GL_UNSIGNED_INT_24_8,
				GL_FLOAT,
				NULL
			);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.color, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture.depth, 0);
		}
		~FrameBuffer() {
			glDeleteFramebuffers(1, &id);
			delete target;
		}

		void operator()() {
			glBindFramebuffer(GL_FRAMEBUFFER, id);
		}

		void renderToBuffer(unsigned int buffer = 0) {
			glBindFramebuffer(GL_FRAMEBUFFER, buffer); // back to default
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glBindTexture(GL_TEXTURE_2D, texture.color);
			(*compose)();
			target->render();
		}

		unsigned int getID() {
			return id;
		}

		Shader::Shader* compose;
		RenderData::PlaneReference* rect;

	private:
		unsigned int id;
		RenderData::Renderable* target;
		struct {
			unsigned int color;
			unsigned int depth;
		} texture;
	};
}

namespace EntityClass {

}

#undef $$FUNC

#undef UNROLL_LOOPS
#undef DERIVED_CLASS

#endif // GRAPHICAL_RENDERER_H
