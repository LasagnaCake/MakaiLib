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
			VecMath::srpTransform,
			RenderData::Material::BaseBufferMaterial,
			RenderData::Material::BufferMaterial,
			RenderData::Material::setMaterial
		;
	}

	struct FrameBufferData {
		unsigned int
			id		= 0,
			width	= 0,
			height	= 0
		;
		Texture2D
			screen,
			depth
		;
	};

	class BaseBuffer {
	public:
		BaseBuffer() {
		}

		BaseBuffer(
			unsigned int const& width,
			unsigned int const& height
		) {
			create(width, height);
		}

		~BaseBuffer() {
			destroy();
		}

		virtual BaseBuffer& destroy() {
			if (!created) return *this;
			else created = false;
			glDeleteFramebuffers(1, &id);
			return *this;
		}

		virtual BaseBuffer& create(
			unsigned int const& width,
			unsigned int const& height
		) {
			if (created) return *this;
			else created = true;
			glGenFramebuffers(1, &id);
			glBindFramebuffer(GL_FRAMEBUFFER, id);
			this->width = width;
			this->height = height;
			disable();
			return *this;
		}

		virtual BaseBuffer& enable() {
			if (!created) return *this;
			glBindFramebuffer(GL_FRAMEBUFFER, id);
			return *this;
		}

		BaseBuffer& operator()() {return enable();}

		virtual BaseBuffer& disable() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return *this;
		}

		inline bool exists() {return created;}

	protected:
		inline unsigned int getWidth()	{return width;	}
		inline unsigned int getHeight()	{return height;	}
		inline unsigned int getID()		{return id;		}

	private:
		bool created = false;
		unsigned int id;
		unsigned int width, height;
	};

	// Todo: Fix this
	template<Type::Derived<BaseBufferMaterial> T>
	class BaseFrameBuffer: public BaseBuffer, public Blendable {
	public:
		BaseFrameBuffer() {
		}

		BaseFrameBuffer(
			unsigned int const& width,
			unsigned int const& height
		) {
			create(width, height);
		}

		~BaseFrameBuffer() {
			destroy();
		}

		BaseFrameBuffer& destroy() override {
			if (!exists()) return *this;
			buffer.screen.destroy();
			buffer.depth.destroy();
			glDeleteBuffers(1, &vbo);
			glDeleteVertexArrays(1, &vao);
			BaseBuffer::destroy();
			return *this;
		}

		BaseFrameBuffer& create(
			unsigned int const& width,
			unsigned int const& height
		) override {
			if (exists()) return *this;
			BaseBuffer::create(width, height);
			glBindFramebuffer(GL_FRAMEBUFFER, getID());
			buffer.screen.create(width, height, GL_FLOAT, GL_RGBA, GL_LINEAR, GL_LINEAR, NULL, GL_RGBA16F);
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
			setBlendFunction(DEFAULT_BLEND_FUNC);
			setBlendEquation(DEFAULT_BLEND_EQUATION);
			disable();
			return *this;
		}

		BaseFrameBuffer& enable() override {
			if (!exists()) return *this;
			BaseBuffer::enable();
			this->clearDepthBuffer();
			return *this;
		}

		FrameBufferData toFrameBufferData() {
			if (!exists())
				return FrameBufferData{};
			return FrameBufferData{
				getID(),
				getWidth(),
				getHeight(),
				&buffer.screen,
				&buffer.depth
			};
		}

		BaseFrameBuffer& clearBuffers() {
			this->clearColorBuffer();
			this->clearDepthBuffer();
			return *this;
		}

		BaseFrameBuffer& clearColorBuffer() {
			Drawer::clearColorBuffer(material.background);
			return *this;
		}

		BaseFrameBuffer& clearDepthBuffer() {
			glClear(GL_DEPTH_BUFFER_BIT);
			return *this;
		}

		virtual BaseFrameBuffer& render(FrameBufferData const& target) {
			if (!exists()) return *this;
			// Set target buffer
			glBindFramebuffer(GL_FRAMEBUFFER, target.id);
			// set blend func & equation data
			setBlend();
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
			Vector2 const resolution = Vector2(getWidth(), getHeight());
			shader["resolution"](resolution);
			shader["screenVUSpace"](screenVUSpace);
			shader["pixelVU"](resolution / screenVUSpace);
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
			disable();
			return *this;
		}

		BaseFrameBuffer& render(BaseFrameBuffer& targetBuffer) {
			if (!exists()) return *this;
			if (!targetBuffer.exists()) return *this;
			return render(targetBuffer.toFrameBufferData());
		}

		BaseFrameBuffer& disable() override {
			BaseBuffer::disable();
			return *this;
		}

		/// The framebuffer's vertex transformation.
		Transform3D trans;
		/// The framebuffer's UV transformation.
		Transform3D uv;
		/// The framebuffer's shape.
		RawVertex rect[4];
		/// The framebuffer's rendering shader.
		Shader::Shader shader;
		/// The framebuffer's material.
		T material;
		/// The framebuffer's screen Vertex Unit space. Usually the inverse of the camera's orthographic size.
		Vector2 screenVUSpace = 1;
		/// The framebuffer's blend function & equation setting.
		BlendData blend;

	private:
		struct {
			Texture2D screen;
			Texture2D depth;
		} buffer;
		unsigned int vao;
		unsigned int vbo;
	};

	class FrameBuffer: public BaseFrameBuffer<BufferMaterial> {
	public:
		using BaseFrameBuffer::BaseFrameBuffer;

		FrameBuffer& render(FrameBufferData const& target) final {
			if (!exists()) return *this;
			// Set material data
			setMaterial(shader, material);
			// Render buffer
			BaseFrameBuffer::render(target);
			return *this;
		}

		FrameBuffer& render(FrameBuffer& targetBuffer) {
			if (!exists()) return *this;
			if (!targetBuffer.exists()) return *this;
			return render(targetBuffer.toFrameBufferData());
		}
	};
}

#endif // MAKAI_FRAME_BUFFERRING_H
