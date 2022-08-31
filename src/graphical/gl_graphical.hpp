#ifndef GRAPHICAL_RENDERER_H
#define GRAPHICAL_RENDERER_H

#include "../anchors.hpp"
#include "gl_shader.hpp"

#define $$FUNC function<void()>

#define $glpointer(start, offset) (void*)((start) + (offset) * sizeof(float))
#define $gloffset(offset) (void*)((offset) * sizeof(float))

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
	#define RAW_VERTEX_BYTE_SIZE RAW_VERTEX_SIZE * sizeof(float)
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

	typedef const function<void()> DrawFunc;

	Group::Group<DrawFunc*> layers;

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

	void setTexture2D(unsigned char index, GLuint texture) {
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, texture);
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
}

namespace VecMath {
	inline glm::vec3 asGLMVector(Vector3 vec) {
		return glm::vec3(vec.x, vec.y, vec.z);
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
		Drawer::Vertex,
		Drawer::RawVertex,
		Drawer::toRawVertex,
		Drawer::DrawFunc,
		std::function,
		std::vector;
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
				this->verts[i].position	= verts	? verts[i]	: Vector3(0.0);
				this->verts[i].uv		= uv	? uv[i]		: Vector2(0.0);
				this->verts[i].color	= color	? color[i]	: Vector4(1.0);
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
			Triangle tris[2]
		) {
			this->tl	= &(tris[0].verts[0]);
			this->tr1	= &(tris[0].verts[1]);
			this->tr2	= &(tris[1].verts[0]);
			this->bl1	= &(tris[0].verts[2]);
			this->bl2	= &(tris[1].verts[1]);
			this->br	= &(tris[1].verts[2]);
		}

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

		/// Sets the plane's origin.
		PlaneReference* setOrigin(
				Vector3 tlPos,
				Vector3 trPos,
				Vector3 blPos,
				Vector3 brPos
			) {
			origin[0] = tl->position	= tlPos;
			origin[1] = tr1->position	= trPos;
			origin[2] = tr2->position	= trPos;
			origin[3] = bl1->position	= blPos;
			origin[4] = bl2->position	= blPos;
			origin[5] = br->position	= brPos;
			return this;
		}

		/// Transforms the plane by a given transform, and sets it as its origin.
		PlaneReference* setOrigin(Transform3D trans) {
			origin[0] = tl->position	= srpTransform(tl->position, trans);
			origin[1] = tr1->position	= srpTransform(tr1->position, trans);
			origin[2] = tr2->position	= srpTransform(tr2->position, trans);
			origin[3] = bl1->position	= srpTransform(bl1->position, trans);
			origin[4] = bl2->position	= srpTransform(bl2->position, trans);
			origin[5] = br->position	= srpTransform(br->position, trans);
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

		/// Transforms the plane by a given transform.
		PlaneReference* transform(Transform3D trans) {
			tl->position	= srpTransform(tl->position, trans);
			tr1->position	= srpTransform(tr1->position, trans);
			tr2->position	= srpTransform(tr2->position, trans);
			bl1->position	= srpTransform(bl1->position, trans);
			bl2->position	= srpTransform(bl2->position, trans);
			br->position	= srpTransform(br->position, trans);
			return this;
		}

		/// Sets the plane to its original state (last state set with setPosition).
		PlaneReference* reset() {
			tl->position	= origin[0];
			tr1->position	= origin[1];
			tr2->position	= origin[2];
			bl1->position	= origin[3];
			bl2->position	= origin[4];
			br->position	= origin[5];
			return this;
		}

	private:
		Vector3 origin[6];

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

		void setManual() {
			if(!manualMode)
				Drawer::layers.removeFromAll(&render);
			manualMode = true;
		}

		void setAuto(size_t renderLayer) {
			if(manualMode)
				Drawer::layers.addObject(&render, renderLayer);
			manualMode = false;
		}

		PlaneReference* createPlaneReference() {
			Triangle* tris = new Triangle[2];
			// Add triangles
			triangles.push_back(&tris[0]);
			triangles.push_back(&tris[1]);
			// Get index of last plane
			size_t last = triangles.size();
			// Create reference
			PlaneReference* plane = new PlaneReference(tris);
			// Setup plane
			plane->setOrigin(
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
			plane->setColor();
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
				vertexCount * RAW_VERTEX_BYTE_SIZE,
				verts,
				GL_STATIC_DRAW
			);
			// Delete IVB, since it is no longer necessary
			delete [] verts;
			// Set VAO as active
			glBindVertexArray(vao);
			// Define vertex data in VBO
			Drawer::setVertexAttributes();
			// Set VAO as active
			glBindVertexArray(vao);
			// Enable attribute pointers
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			// Set polygon rendering mode
			glPolygonMode(params.culling, params.fill);
			// Draw object to screen
			draw();
			// Disable attributes
			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(0);
			// Unbind vertex array
			glBindVertexArray(0);
		};

		Shader::ShaderList shaders;

		vector<Triangle*> triangles;

		struct {
			bool active		= true;
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
			create(width, height);
		}

		~FrameBuffer() {
			glDeleteFramebuffers(1, &id);
			glDeleteTextures(1, &buffer.color);
			glDeleteTextures(1, &buffer.depth);
			glDeleteBuffers(1, &vbo);
			glDeleteVertexArrays(1, &vao);
		}

		void create(
			unsigned int width,
			unsigned int height
		) {
			if (created) return;
			else created = true;
			glGenFramebuffers(1, &id);
			glGenTextures(1, &buffer.color);
			glBindTexture(GL_TEXTURE_2D, buffer.color);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				width,
				height,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				NULL
			);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_2D,
				buffer.color,
				0
			);
			glGenRenderbuffers(1, &buffer.depth);
			glBindRenderbuffer(GL_RENDERBUFFER, buffer.depth);
			glRenderbufferStorage(
				GL_RENDERBUFFER,
				GL_DEPTH24_STENCIL8,
				width,
				height
			);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(
				GL_FRAMEBUFFER,
				GL_DEPTH_STENCIL_ATTACHMENT,
				GL_RENDERBUFFER,
				buffer.depth
			);
			/*
			glGenTextures(1, &buffer.depth);
			glBindTexture(GL_TEXTURE_2D, buffer.depth);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_DEPTH24_STENCIL8,
				width,
				height,
				0,
				GL_DEPTH_STENCIL,
				GL_UNSIGNED_INT_24_8,
				NULL
			);
			glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				GL_DEPTH_STENCIL_ATTACHMENT,
				GL_TEXTURE_2D,
				buffer.depth,
				0
			);*/
			// Setup display rectangle
			rect[0].position	= Vector3(-1, +1, 0);
			rect[0].uv			= Vector2(0, 0);
			rect[1].position	= Vector3(+1, +1, 0);
			rect[1].uv			= Vector2(1, 0);
			rect[2].position	= Vector3(-1, -1, 0);
			rect[2].uv			= Vector2(0, 1);
			rect[3].position	= Vector3(+1, -1, 0);
			rect[3].uv			= Vector2(1, 1);
			// Create buffers
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
		}

		void operator()() {
			glBindFramebuffer(GL_FRAMEBUFFER, id);
		}

		void renderToBuffer(unsigned int targetBuffer = 0) {
			// Create Intermediary Vertex Buffer (IVB) to be displayed on screen
			RawVertex verts[4];
			verts[0] = toRawVertex(rect[0]);
			verts[1] = toRawVertex(rect[1]);
			verts[2] = toRawVertex(rect[2]);
			verts[3] = toRawVertex(rect[3]);
			// Set VBO as active
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			// Copy IVB to VBO
			glBufferData(
				GL_ARRAY_BUFFER,
				RAW_VERTEX_SIZE * 4 * sizeof(float),
				verts,
				GL_STATIC_DRAW
			);
			// Set VAO as active
			glBindVertexArray(vao);
			// Define vertex data in VBO
			Drawer::setVertexAttributes();
			// Get shader
			auto& comp = (*compose);
			// Bind target frame buffer
			glBindFramebuffer(GL_FRAMEBUFFER, targetBuffer);
			// Clear target frame buffer
			glClearColor(color.x, color.y, color.z, color.w);
			glClear(GL_COLOR_BUFFER_BIT);
			// Activate compose shader
			comp();
			Drawer::setTexture2D(0, buffer.depth);
			comp["screen"](0);
			// Draw screen
			draw();
		}

		unsigned int getID() {
			return id;
		}

		Vector4 color = Vector4(0, 0, 0, 1);
		Vertex rect[4];
		Shader::Shader* compose;

	private:
		void draw() {
			// Enable attribute pointers
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			// Set VAO as active
			glBindVertexArray(vao);
			// Set polygon rendering mode
			glPolygonMode(GL_FRONT, GL_LINE);
			// Draw object to screen
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			// Unbind vertex array
			glBindVertexArray(0);
			// Disable attributes
			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(0);
		}

		bool created = false;
		unsigned int id;
		struct {
			unsigned int color;
			unsigned int depth;
		} buffer;
		unsigned int vao;
		unsigned int vbo;
	};
}

namespace EntityClass {

}

#undef $$FUNC

#undef UNROLL_LOOPS
#undef DERIVED_CLASS

#endif // GRAPHICAL_RENDERER_H
