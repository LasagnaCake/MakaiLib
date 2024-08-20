#ifndef MAKAILIB_GRAPH_RENDERER_FRAMEBUFFER_H
#define MAKAILIB_GRAPH_RENDERER_FRAMEBUFFER_H

#include "../texture.hpp"

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

	class BaseBuffer {
	public:
		BaseBuffer() {
		}

		BaseBuffer(
			unsigned int const& width,
			unsigned int const& height
		);

		~BaseBuffer();

		virtual BaseBuffer& destroy();

		virtual BaseBuffer& create(
			unsigned int const& width,
			unsigned int const& height
		);

		virtual BaseBuffer& enable();

		BaseBuffer& operator()();

		virtual BaseBuffer& disable();

		inline bool exists();

	protected:
		inline unsigned int getWidth();
		inline unsigned int getHeight();
		inline unsigned int getID();

	private:
		bool created = false;
		uint id, width, height;
	};
}

#endif // MAKAILIB_GRAPH_RENDERER_FRAMEBUFFER_H
