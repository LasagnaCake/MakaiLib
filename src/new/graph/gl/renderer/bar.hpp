#ifndef MAKAILIB_GRAPH_RENDERER_BAR_H
#define MAKAILIB_GRAPH_RENDERER_BAR_H

#include "../../ctl/ctl.hpp"
#include "drawable.hpp"
#include "text.hpp"

#ifndef RADIAL_BAR_RESOLUTION
#define RADIAL_BAR_RESOLUTION 12
#endif // RADIAL_BAR_RESOLUTION

namespace Makai::Graph {
	namespace Base {
		struct Progressbar {
			float	uvShift = 0, value = 0, max = 100;
			Vector2	size		= 1;
			Vector2	uvScale		= 1;
			bool 	dynamicUV	= true;
		};
	}

	template<class T>
	concept BarType = Type::Subclass<T, Base::Progressbar> && Type::Subclass<T, Drawable>;

	class LinearBar: public DrawableObject, public Base::Progressbar {
	public:
		LinearBar(size_t const& layer = 0, bool const& manual = false): DrawableObject(layer, manual) {}

		Material::ObjectMaterial material;

	private:
		Vertex vertices[4];

		void draw() override;
	};

	class RadialBar: public DrawableObject, public Base::Progressbar {
	public:
		bool	centered	= false;
		Vector2 offset		= Vector2(0);

		RadialBar(size_t const& layer = 0, bool const& manual = false): DrawableObject(layer, manual) {
			vertices[0].u = vertices[0].v = 0.5;
		}

		Material::ObjectMaterial material;

	private:
		Vertex vertices[RADIAL_BAR_RESOLUTION + 2] = {INITIAL_VERTEX};

		void draw() override;

		void update();
	};

	template<BarType T = RadialBar>
	struct LabeledBar {
		using DataType = T;
		DataType	bar;
		Label		label;
	};
}

#endif // MAKAILIB_GRAPH_RENDERER_BAR_H
