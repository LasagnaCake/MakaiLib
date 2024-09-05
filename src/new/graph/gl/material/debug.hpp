#ifndef MAKAILIB_GRAPH_MATERIAL_DEBUG_H
#define MAKAILIB_GRAPH_MATERIAL_DEBUG_H

namespace Makai::Graph::Material {
	enum class BufferDebugView: unsigned int {
		BDV_NONE = 0,
		BDV_DEPTH,
	};

	enum class ObjectDebugView: unsigned int {
		ODV_NONE = 0,
		ODV_NORMAL,
	};
}

#endif // MAKAILIB_GRAPH_MATERIAL_DEBUG_H
