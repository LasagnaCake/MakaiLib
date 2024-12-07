#ifndef MAKAILIB_GRAPH_RENDERER_BAR_H
#define MAKAILIB_GRAPH_RENDERER_BAR_H

#include "../../../compat/ctl.hpp"
#include "drawable.hpp"
#include "text.hpp"

#ifndef RADIAL_BAR_RESOLUTION
#define RADIAL_BAR_RESOLUTION 12
#endif // RADIAL_BAR_RESOLUTION

/// @brief Graphical facilities.
namespace Makai::Graph {
	/// @brief Base classes.
	namespace Base {
		/// @brief Basic progressbar contents.
		struct Progressbar {
			/// @brief Bar-dependent UV shift.
			float uvShift		= 0;
			/// @brief Current value.
			float value			= 0;
			/// @brief Maximum value.
			float max			= 100;
			/// @brief Size.
			Vector2	size		= 1;
			/// @brief UV scale.
			Vector2	uvScale		= 1;
			/// @brief Whether the UV should adapt to the bar's progress.
			bool 	dynamicUV	= true;
		};
	}

	template<class T>
	concept BarType = Makai::Type::Subclass<T, Base::Progressbar> && Makai::Type::Subclass<T, IGLDrawable>;

	class LinearBar: public IGLDrawable, public Base::Progressbar {
	public:
		LinearBar(usize const& layer = 0, bool const manual = false): IGLDrawable(layer, manual) {}

		Material::ObjectMaterial material;

	private:
		Vertex vertices[4];

		void draw() override;
	};

	class RadialBar: public IGLDrawable, public Base::Progressbar {
	public:
		bool	centered	= false;
		Vector2 offset		= Vector2(0);

		RadialBar(usize const& layer = 0, bool const manual = false): IGLDrawable(layer, manual) {
			vertices[0].uv.u = vertices[0].uv.v = 0.5;
		}

		Material::ObjectMaterial material;

	private:
		Vertex vertices[RADIAL_BAR_RESOLUTION + 2] = {INITIAL_VERTEX};

		void draw() override;

		void update();
	};

	template<BarType T = RadialBar>
	struct LabeledBar {
		using BarType = T;
		BarType	bar;
		Label	label;
	};
}

#endif // MAKAILIB_GRAPH_RENDERER_BAR_H
