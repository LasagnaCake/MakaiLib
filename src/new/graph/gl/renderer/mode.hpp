#ifndef MAKAILIB_GRAPH_MODE_H
#define MAKAILIB_GRAPH_MODE_H

#include "../../../ctl/ctl.hpp"

namespace Makai::Graph {
	enum class CullMode {
		OCM_NONE,
		OCM_FRONT,
		OCM_BACK,
	};

	enum class FillMode {
		OFM_NONE = -1,
		OFM_FILL,
		OFM_LINE,
		OFM_POINT,
	};

	enum class DisplayMode {
		ODM_POINTS,
		ODM_LINE_STRIP,
		ODM_LINE_LOOP,
		ODM_LINES,
		ODM_TRI_STRIP,
		ODM_TRI_FAN,
		ODM_TRIS,
		ODM_LINES_ADJ,
		ODM_LINE_STRIP_ADJ,
		ODM_TRI_ADJ,
		ODM_TRI_STRIP_ADJ,
		ODM_PATCHES
	};

	void setCullMode(CullMode const& mode = CullMode::OCM_NONE);
	bool setFillMode(FillMode const& mode = FillMode::OFM_FILL);

	uint getGLDisplayMode(DisplayMode const& mode = DisplayMode::ODM_TRIS);
}

#endif // MAKAILIB_GRAPH_MODE_H
