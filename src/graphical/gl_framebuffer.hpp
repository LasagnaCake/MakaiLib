#ifndef MAKAI_FRAME_BUFFERRING_H
#define MAKAI_FRAME_BUFFERRING_H

/*
STAY AWAY FROM THIS CLASS. THIS CLASS
IS NOT A CLASS OF HONOR.

TOO VOLATILE, AND TOO TRIGGER-HAPPY
TO FAIL ON YOU.

IT PLAYS BY ITS OWN RULES, IT WANTS
YOU TO LOSE YOUR MIND, LIKE IT IS A
SADISTIC GAME TO IT.

IF YOU MUST, MAY YOU FIND A GOD THAT
HAS MERCY ON YOU. BECAUSE THIS CLASS
SURE AS HELL DOESN'T.

IF ALL ELSE FAILS, CLEAR THE DEPTH
BUFFER. IT APPEASES THE BEAST
TEMPORARILY. I'M STILL NOT SURE ON
HOW THIS MAKES SENSE, BUT I'M NOT
GOING TO QUESTION IT.
*/

namespace Drawer {
	namespace {
		using
		VecMath::srpTransform;
	}
	struct FrameBufferData {
		unsigned int
			id		= 0,
			width	= 0,
			height	= 0
		;
		Texture2D
			*screen	= nullptr,
			*depth	= nullptr
		;
	};

	// Todo: Fix this
	class FrameBuffer {
	public:
		FrameBuffer() {
		}

		FrameBuffer(
			unsigned int width,
			unsigned int height
		) {
			create(width, height);
		}

		~FrameBuffer() {
			destroy();
		}

		void destroy() {
			if (!created) return;
			else created = false;
			glDeleteFramebuffers(1, &id);
			buffer.screen.destroy();
			buffer.depth.destroy();
			glDeleteBuffers(1, &vbo);
			glDeleteVertexArrays(1, &vao);
		}

		FrameBuffer* create(
			unsigned int width,
			unsigned int height
		) {
			if (created) return this;
			else created = true;
			glGenFramebuffers(1, &id);
			glBindFramebuffer(GL_FRAMEBUFFER, id);
			buffer.screen.create(width, height, GL_FLOAT, GL_RGBA, GL_LINEAR, GL_LINEAR);
			//buffer.screen.create(width, height, GL_UNSIGNED_BYTE, GL_RGBA, GL_LINEAR, GL_LINEAR);
			glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_2D,
				buffer.screen.getID(),
				0
			);
			buffer.depth.create(width, height, GL_UNSIGNED_INT_24_8, GL_DEPTH_STENCIL, GL_LINEAR, GL_LINEAR, NULL, GL_DEPTH24_STENCIL8);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
			glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				GL_DEPTH_STENCIL_ATTACHMENT,
				GL_TEXTURE_2D,
				buffer.depth.getID(),
				0
			);
			// Setup display rectangle
			rect[0] = {-1, +1, 0, 0, 1};
			rect[1] = {+1, +1, 0, 1, 1};
			rect[2] = {-1, -1, 0, 0, 0};
			rect[3] = {+1, -1, 0, 1, 0};
			// Create buffers
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			// This keeps the alpha from shitting itself
			glEnable(GL_BLEND);
			glEnable(GL_ALPHA_TEST);
			//glDisable(GL_BLEND);
			//glEnable(GL_CULL_FACE);
			glBlendFuncSeparatei(id, DEFAULT_BLEND_FUNC);
			glBlendEquationSeparatei(id, DEFAULT_BLEND_EQUATION);
			glDepthFunc(GL_LESS);
			//glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
			//glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			this->width = width;
			this->height = height;
			return this;
		}

		FrameBuffer* operator()() {
			return enable();
		}

		FrameBuffer* enable() {
			glBindFramebuffer(GL_FRAMEBUFFER, id);
			glClear(GL_DEPTH_BUFFER_BIT);
			return this;
		}

		FrameBufferData toFrameBufferData() {
			return FrameBufferData{
				id,
				width,
				height,
				&buffer.screen,
				&buffer.depth
			};
		}

		FrameBuffer* clearBuffers() {
			this->clearColorBuffer();
			this->clearDepthBuffer();
			return this;
		}

		FrameBuffer* clearColorBuffer() {
			Drawer::clearColorBuffer(material.background);
			return this;
		}

		FrameBuffer* clearDepthBuffer() {
			glClear(GL_DEPTH_BUFFER_BIT);
			return this;
		}

		FrameBuffer* render(FrameBufferData target) {
			if (!created) return this;
			// Set target buffer
			glBindFramebuffer(GL_FRAMEBUFFER, target.id);
			// Set VBO as active
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			// Copy vertices to VBO
			glBufferData(
				GL_ARRAY_BUFFER,
				RAW_VERTEX_BYTE_SIZE * 4,
				rect,
				GL_STATIC_DRAW
			);
			// Set VAO as active
			glBindVertexArray(vao);
			// Define vertex data in VBO
			Drawer::setVertexAttributes();
			// Get shader
			// Activate shader
			shader();
			// Set shader textures
			buffer.depth(30);
			buffer.screen(31);
			// Set camera's near and far plane
			shader["near"](Scene::camera.zNear);
			shader["far"](Scene::camera.zFar);
			// Set texture locations
			shader["depth"](30);
			shader["screen"](31);
			// Set transformation matrix
			shader["posMatrix"](Matrix4x4(trans));
			shader["uvMatrix"](Matrix4x4(uv));
			shader["resolution"](Vector2(width, height));
			// Set material data
			RenderData::Material::setMaterial(shader, material);
			// Enable attribute pointers
			Drawer::enableVertexAttributes();
			// Set VAO as active
			glBindVertexArray(vao);
			// Set polygon rendering mode
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDisable(GL_CULL_FACE);
			// Draw object to screen
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			// Unbind vertex array
			glBindVertexArray(0);
			// Disable attributes
			Drawer::disableVertexAttributes();
			return this;
		}

		FrameBuffer* render(FrameBuffer& targetBuffer) {
			if (!targetBuffer.exists()) return this;
			return render(targetBuffer.toFrameBufferData());
		}

		bool exists() {
			return created;
		}

		/// The framebuffer's vertex transformation.
		Transform3D trans;
		/// The framebuffer's UV transformation.
		Transform3D uv;
		/// The framebuffer's material.
		RenderData::Material::BufferMaterial material;
		/// The framebuffer's shape.
		RawVertex rect[4];
		/// The framebuffer's rendering shader.
		Shader::Shader shader;

	private:

		bool created = false;
		unsigned int id;
		unsigned int width, height;
		struct {
			Texture2D screen;
			Texture2D depth;
		} buffer;
		unsigned int vao;
		unsigned int vbo;
	};
}

#endif // MAKAI_FRAME_BUFFERRING_H
