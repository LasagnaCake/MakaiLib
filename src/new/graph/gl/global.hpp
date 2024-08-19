#ifndef MAKAILIB_GRAPH_GLOBAL_H
#define MAKAILIB_GRAPH_GLOBAL_H

#include "../ctl/ctl.hpp"
#include "camera.hpp"

namespace Makai::Graph {
	namespace Global {
		Matrix4x4 world = Matrix4x4::identity();
		Camera3D camera;
	}
}

#endif // MAKAILIB_GRAPH_GLOBAL_H
