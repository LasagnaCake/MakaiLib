#ifndef MAKAILIB_GRAPH_H
#define MAKAILIB_GRAPH_H

namespace Makai::Graph {
	enum class GraphicalAPI {
		GA_OPENGL,
		GA_VULKAN,
		GA_DIRECT3D
	}
}

#define MAKAILIB_USE_OPENGL (1)

#ifndef MAKAILIB_GRAPHICAL_API
#define MAKAILIB_GRAPHICAL_API MAKAILIB_USE_OPENGL
#endif

#if (MAKAILIB_GRAPHICAL_API == MAKAILIB_USE_OPENGL)
namespace Makai::Graph {constexpr GraphicalAPI API = GraphicalAPI::GA_OPENGL;}
#include "gl/graph.hpp"
#endif

#endif // MAKAILIB_GRAPH_H
