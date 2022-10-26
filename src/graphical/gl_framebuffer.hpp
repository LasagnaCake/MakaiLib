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
		VecMath::$srpTransform;
	}
	struct FrameBufferData {
		unsigned int
			id,
			screen,
			depth,
			width,
			height;
	};
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

		FrameBuffer* create(
			unsigned int width,
			unsigned int height
		) {
			if (created) return this;
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
			// Resolution math
			Vector3 screenRes(width, height, 1);
			// Setup display rectangle
			rect[0].position	= Vector3(-1, +1, 0) * screenRes;
			rect[0].uv			= Vector2(0, 1) * screenRes.xy();
			rect[0].color		= Vector4(1);
			rect[1].position	= Vector3(+1, +1, 0) * screenRes;
			rect[1].uv			= Vector2(1, 1) * screenRes.xy();
			rect[0].color		= Vector4(1);
			rect[2].position	= Vector3(-1, -1, 0) * screenRes;
			rect[2].uv			= Vector2(0, 0) * screenRes.xy();
			rect[0].color		= Vector4(1);
			rect[3].position	= Vector3(+1, -1, 0) * screenRes;
			rect[3].uv			= Vector2(1, 0) * screenRes.xy();
			rect[0].color		= Vector4(1);
			// Create buffers
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			// This keeps the alpha from shitting itself
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			//glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
			//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
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
				buffer.screen,
				buffer.depth,
				width,
				height
			};
		}

		FrameBuffer* clearBuffers() {
			this->clearColorBuffer();
			this->clearDepthBuffer();
			return this;
		}

		FrameBuffer* clearColorBuffer() {
			Drawer::clearColorBuffer(color);
			return this;
		}

		FrameBuffer* clearDepthBuffer() {
			glClear(GL_DEPTH_BUFFER_BIT);
			return this;
		}

		FrameBuffer* render(FrameBufferData target) {
			// Set target buffer
			glBindFramebuffer(GL_FRAMEBUFFER, target.id);
			// Clear target frame buffer
			//clearColorBuffer(target.color);
			// Create Intermediary Vertex Buffer (IVB) to be displayed on screen
			RawVertex verts[4];
			for (unsigned char i = 0; i < 4; i++) {
				Vertex v = rect[i];
				v.position = $srpTransform(
						v.position,
						transform.position,
						transform.rotation,
						transform.scale
					);
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
			// Activate shader
			shader();
			// Set shader textures
			Drawer::setTexture2D(30, buffer.depth);
			Drawer::setTexture2D(31, buffer.screen);
			// Set camera's near and far plane
			shader["near"](Scene::camera.zNear);
			shader["far"](Scene::camera.zFar);
			// Set texture locations
			shader["depth"](30);
			shader["screen"](31);
			// Set transformation data
			shader["albedo"](tint);
			shader["accent"](tone);
			shader["uvShift"](uvShift);
			shader["resolution"](glm::vec2(width, height));
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
			return this;
		}

		FrameBuffer* render(FrameBuffer& targetBuffer) {
			return render(targetBuffer.toFrameBufferData());
		}

		/// The framebuffer's transformation.
		Transform3D transform;
		/// The framebuffer's UV offset.
		Vector2 uvShift;
		/// The framebuffer's tint.
		Vector4 tint = Color::WHITE;
		// DO NOT SET TONE TO TRANSPARENT (1, 1, 1, 0) HERE.
		/// The framebuffer's balance color.
		Vector4 tone = Color::NONE;
		/// The framebuffer's clear color.
		Vector4 color = Color::TRANSPARENT;
		/// The framebuffer's shape.
		Vertex rect[4];
		/// The framebuffer's rendering shader.
		Shader::Shader shader;

	private:

		bool created = false;
		unsigned int id;
		unsigned int width, height;
		struct {
			unsigned int screen;
			unsigned int depth;
		} buffer;
		unsigned int vao;
		unsigned int vbo;
	};
}

#endif // MAKAI_FRAME_BUFFERRING_H
