#ifndef MAKAI_FRAME_BUFFERRING_H
#define MAKAI_FRAME_BUFFERRING_H

namespace Drawer {
	// Todo: Fix this
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
			destroy();
		}

		void destroy() {
			if (!created) return;
			else created = false;
			glDeleteFramebuffers(1, &id);
			glDeleteTextures(1, &buffer.screen);
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
			glBindFramebuffer(GL_FRAMEBUFFER, id);
			buffer.screen = Drawer::createTexture2D(width, height, GL_UNSIGNED_INT);
			glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_2D,
				buffer.screen,
				0
			);
			buffer.depth = Drawer::createTexture2D(width, height, GL_UNSIGNED_INT, GL_DEPTH_COMPONENT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
			glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				GL_DEPTH_ATTACHMENT,
				GL_TEXTURE_2D,
				buffer.depth,
				0
			);
			// Setup display rectangle
			rect[0].position	= Vector3(-1, +1, 0);
			rect[0].uv			= Vector2(0, 1);
			rect[0].color		= Vector4(1);
			rect[1].position	= Vector3(+1, +1, 0);
			rect[1].uv			= Vector2(1, 1);
			rect[0].color		= Vector4(1);
			rect[2].position	= Vector3(-1, -1, 0);
			rect[2].uv			= Vector2(0, 0);
			rect[0].color		= Vector4(1);
			rect[3].position	= Vector3(+1, -1, 0);
			rect[3].uv			= Vector2(1, 0);
			rect[0].color		= Vector4(1);
			// Create buffers
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void operator()() {
			glBindFramebuffer(GL_FRAMEBUFFER, id);
		}

		void operator[](unsigned int targetBuffer) {
			render(targetBuffer);
		}

		void render(unsigned int targetBuffer = 0) {
			// Set target buffer
			glBindFramebuffer(GL_FRAMEBUFFER, targetBuffer);
			// Clear target frame buffer
			glClearColor(color.x, color.y, color.z, color.w);
			glClear(GL_COLOR_BUFFER_BIT);
			// Create Intermediary Vertex Buffer (IVB) to be displayed on screen
			RawVertex verts[4];
			for (unsigned char i = 0; i < 4; i++) {
				Vertex v = rect[i];
				v.position = SRP_TRANSFORM(v.position.xy(), transform);
				v.uv += uvShift;
				v.color *= tint;
				verts[i] = toRawVertex(v);
			}
			// Set VBO as active
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			// Copy IVB to VBO
			glBufferData(
				GL_ARRAY_BUFFER,
				RAW_VERTEX_BYTE_SIZE * 4,
				verts,
				GL_STATIC_DRAW
			);
			// Set VAO as active
			glBindVertexArray(vao);
			// Define vertex data in VBO
			Drawer::setVertexAttributes();
			// Get shader
			auto& comp = (*shader);
			// Activate shader shader
			comp();
			Drawer::setTexture2D(30, buffer.depth);
			Drawer::setTexture2D(31, buffer.screen);
			comp["near"](Scene::camera.zNear);
			comp["far"](Scene::camera.zFar);
			comp["depth"](30);
			comp["screen"](31);
			// Draw screen
			draw();
		}

		unsigned int getID() {
			return id;
		}

		/// The framebuffer's transformation.
		Transform3D transform;
		/// The framebuffer's UV offset.
		Vector2 uvShift;
		/// The framebuffer's tint.
		Vector4 tint = Color::WHITE;

		/// The framebuffer's clear color.
		Vector4 color = Color::NONE;
		/// The framebuffer's shape.
		Vertex rect[4];
		/// The framebuffer's rendering shader.
		Shader::Shader* shader;

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
			unsigned int screen;
			unsigned int depth;
		} buffer;
		unsigned int vao;
		unsigned int vbo;
	};
}

#endif // MAKAI_FRAME_BUFFERRING_H
