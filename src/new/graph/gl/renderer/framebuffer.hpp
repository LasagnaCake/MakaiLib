#ifndef MAKAILIB_GRAPH_RENDERER_FRAMEBUFFER_H
#define MAKAILIB_GRAPH_RENDERER_FRAMEBUFFER_H

#include "../texture.hpp"
#include "../vertex.hpp"
#include "../shader.hpp"
#include "../color.hpp"
#include "../blend.hpp"
#include "../material/material.hpp"

namespace Makai::Graph {
	struct FrameBufferData {
		uint
			id		= 0,
			width	= 0,
			height	= 0
		;
		Texture2D
			screen,
			depth
		;
	};

	namespace Base {
		class DrawBuffer {
		public:
			DrawBuffer() {}

			DrawBuffer(
				uint const& width,
				uint const& height
			);

			virtual ~DrawBuffer();

			virtual DrawBuffer& destroy();

			virtual DrawBuffer& create(
				uint const& width,
				uint const& height
			);

			virtual DrawBuffer const& enable() const;

			DrawBuffer const& operator()() const;

			virtual DrawBuffer const& disable() const;

			bool exists() const;

		protected:
			uint getWidth() const;
			uint getHeight() const;
			uint getID() const;

		private:
			bool created = false;
			uint id, width, height;
		};

		class FrameBuffer: public DrawBuffer, public Blendable {
		public:
			FrameBuffer() {}

			FrameBuffer(
				uint const& width,
				uint const& height
			);

			virtual ~FrameBuffer();

			FrameBuffer& destroy() override;

			FrameBuffer& create(
				uint const& width,
				uint const& height
			) override;

			FrameBuffer const& enable() const override;

			FrameBufferData toFrameBufferData() const;

			FrameBuffer const& clearBuffers() const;

			FrameBuffer const& clearColorBuffer() const;

			FrameBuffer const& clearDepthBuffer() const;

			virtual FrameBuffer const& render(FrameBufferData const& target) const;

			FrameBuffer const& render(FrameBuffer& targetBuffer) const;

			FrameBuffer const& disable() const override;

			/// The framebuffer's vertex transformation.
			Transform3D trans;
			/// The framebuffer's UV transformation.
			Transform3D uv;
			/// The framebuffer's shape.
			Vertex rect[4];
			/// The framebuffer's rendering shader.
			Shader shader;
			/// The framebuffer's screen Vertex Unit space. Usually the inverse of the camera's orthographic size.
			Vector2 screenVUSpace = 1;
			/// The framebuffer's blend function & equation setting.
			BlendData blend;

		private:
			struct {
				Texture2D screen;
				Texture2D depth;
			} buffer;
			uint vao;
			uint vbo;
		};
	}

	class FrameBuffer: public Base::FrameBuffer {
	public:
		using Base::FrameBuffer::FrameBuffer;

		Material::BufferMaterial material;

		virtual FrameBuffer const& render(FrameBufferData const& target) const final {
			if (!exists()) return *this;
			// Set material data
			material.use(shader);
			// Render buffer
			Base::FrameBuffer::render(target);
			return *this;
		}

		FrameBuffer const& render(FrameBuffer& targetBuffer) const {
			if (!exists()) return *this;
			if (!targetBuffer.exists()) return *this;
			return render(targetBuffer.toFrameBufferData());
		}
	};
}

#endif // MAKAILIB_GRAPH_RENDERER_FRAMEBUFFER_H
