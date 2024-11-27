#include "core.hpp"

using namespace Makai; using namespace Makai::Graph;

void Renderer::renderLayer(usize const layer) {
	return renderLayer(layers[layer]);
}

void Renderer::renderLayer(typename Renderer::Layers::GroupType const& layer) {
	#ifdef MAKAILIB_DEBUG
	API::Debug::Context ctx("Renderer::renderLayer");
	#endif // MAKAILIB_DEBUG
	for (auto draw: layer)
		(*draw)();
}
