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

}

#endif // MAKAILIB_GRAPH_RENDERER_FRAMEBUFFER_H
