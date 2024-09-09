#ifndef MAKAILIB_GRAPH_CORE_H
#define MAKAILIB_GRAPH_CORE_H

#include "../../ctl/ctl.hpp"

namespace Makai::Graph::API {
	void open();

	bool hasRequiredVersion();

	String versionString();
}

#endif // MAKAILIB_GRAPH_CORE_H
