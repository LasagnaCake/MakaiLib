#ifndef MAKAILIB_GRAPH_RENDERER_DUMMY_H
#define MAKAILIB_GRAPH_RENDERER_DUMMY_H

#include "drawable.hpp"

namespace Makai::Graph {
	class Dummy: public IDrawable {
	public:
		Dummy(Arguments<usize> const& layers) {
			for (usize const l : layers)
				addToRenderLayer(l);
		}
	};
}

#endif // MAKAILIB_GRAPH_RENDERER_DUMMY_H
