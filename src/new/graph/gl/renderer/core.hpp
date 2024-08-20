#ifndef MAKAILIB_GRAPH_RENDERER_CORE_H
#define MAKAILIB_GRAPH_RENDERER_CORE_H

#include "../../ctl/ctl.hpp"

namespace Makai::Graph {
	struct Renderer {
		using Callback = const Event::Signal;

		static void renderLayer(usize const& layer);

		static List<usize> getLayers() {
			return layers.getAllGroups();
		}

		static bool isLayerEmpty(usize const& layer) {
			return layers[layer].empty();
		}

	private:
		inline static Group::Group<Renderer::Callback> layers;
		friend class Drawable;
	};

	using RenderCallback = typename Renderer::Callback;
}

#endif // MAKAILIB_GRAPH_RENDERER_CORE_H
