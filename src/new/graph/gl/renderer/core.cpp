#include "core.hpp"

using namespace Makai::Graph;

void Renderer::renderLayer(usize const& layer) {
	#ifdef MAKAILIB_DEBUG
	API::Debug::Context ctx("Renderer::renderLayer");
	#endif // MAKAILIB_DEBUG
	for (auto cb: layers[layer])
		(*cb)();
}
