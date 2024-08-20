#include "render.hpp"

using namespace Makai::Graph;

void Renderer::renderLayer(usize const& layer) {
	for (auto cb: layers[layer])
		(*cb)();
}
