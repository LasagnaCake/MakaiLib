#ifndef MAKAI_FRAME_BUFFERRING_H
#define MAKAI_FRAME_BUFFERRING_H

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
			destroy();
		}

		void destroy() {
			if (!created) return;
			else created = false;
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
			rect[0].color		= Vector4(1);
			rect[1].position	= Vector3(+1, +1, 0);
			rect[1].uv			= Vector2(1, 0);
			rect[0].color		= Vector4(1);
			rect[2].position	= Vector3(-1, -1, 0);
			rect[2].uv			= Vector2(0, 1);
			rect[0].color		= Vector4(1);
			rect[3].position	= Vector3(+1, -1, 0);
			rect[3].uv			= Vector2(1, 1);
			rect[0].color		= Vector4(1);
			// Create buffers
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
		}

		void operator()() {
			glBindFramebuffer(GL_FRAMEBUFFER, id);
		}

		void operator[](unsigned int targetBuffer) {
			render(targetBuffer);
		}

		void render(unsigned int targetBuffer = 0) {
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

#endif // MAKAI_FRAME_BUFFERRING_H
