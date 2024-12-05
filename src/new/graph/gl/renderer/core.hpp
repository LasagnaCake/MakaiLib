#ifndef MAKAILIB_GRAPH_RENDERER_CORE_H
#define MAKAILIB_GRAPH_RENDERER_CORE_H

#include "../../../compat/ctl.hpp"
#include "../core.hpp"

namespace Makai {
	struct App;
}

namespace Makai::Graph {
	class IDrawable;

	struct Renderer {
		using Callback	= const Signal<>;
		using Layers	= Groups<Callback*>;

		static void renderLayer(usize const layer);

		static typename Layers::IdentifierListType getLayers() {
			return layers.all();
		}

		static bool isLayerEmpty(usize const layer) {
			return layers.get(layer).empty();
		}

	private:
		inline static Layers layers;

		static void renderLayer(typename Layers::GroupType const& layer);

		friend class IDrawable;
		friend class ::Makai::App;
	};

	using RenderCallback = typename Renderer::Callback;
}

#endif // MAKAILIB_GRAPH_RENDERER_CORE_H
