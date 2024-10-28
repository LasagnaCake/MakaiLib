#ifndef MAKAILIB_GRAPH_H
#define MAKAILIB_GRAPH_H

namespace Makai::Graph {
	enum class GraphicalAPI {
		GA_OPENGL/*,
		GA_VULKAN,
		GA_DIRECT3D*/
	};

	GraphicalAPI getAPIUsed();
}

#include "gl/graph.hpp"

namespace MkGraph = Makai::Graph;

#endif // MAKAILIB_GRAPH_H
