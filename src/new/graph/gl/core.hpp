#ifndef MAKAILIB_GRAPH_CORE_H
#define MAKAILIB_GRAPH_CORE_H

#include "../../ctl/ctl.hpp"

namespace Makai::Graph::API {
	void open();

	bool hasRequiredVersion();

	String versionString();

	enum class Facility {
		GAF_DEPTH_TEST,
		GAF_BLEND,
		GAF_DEBUG
	};

	void toggle(Facility const& fac, bool const& state);

	template<Facility... Facilities>
	void toggle(bool const& state, Facilities const&... facs) {
		(..., setFlag(facs, state));
	}

	enum class Buffer {
		GAB_COLOR,
		GAB_DEPTH
	};

	void clear(Buffer const& buffer);

	template<Buffer... Buffers>
	void clear(Buffers const&... buffers) {
		(..., clear(buffers));
	}

	void setClearColor(Vector4 const& color);

	void setFrontFace(bool const& clockwise = true);
}

#endif // MAKAILIB_GRAPH_CORE_H
