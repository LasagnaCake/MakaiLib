#ifndef MAKAILIB_GRAPH_RENDERER_DRAWABLE_H
#define MAKAILIB_GRAPH_RENDERER_DRAWABLE_H

#include "../../../ctl/ctl.hpp"
#include "../material/material.hpp"
#include "../blend.hpp"
#include "../vertex.hpp"
#include "../global.hpp"
#include "mode.hpp"
#include "core.hpp"

namespace Makai::Graph {
	class Drawable {
	public:
		Drawable(bool const& manual = false);

		Drawable(bool const& manual, usize const& layer);

		virtual ~Drawable();

		Drawable& setManual();

		Drawable& setAuto(usize const& renderLayer);

		Drawable& setRenderLayer(usize const& renderLayer);

		Drawable& addToRenderLayer(usize const& renderLayer);

		Drawable& removeFromRenderLayer(usize const& renderLayer);

		/// Renders the object to the screen.
		RenderCallback render = SIGNAL {doRender();};

		bool active	= true;

	protected:
		virtual void draw() {}

	private:
		void doRender();

		/// Whether manually rendering or not.
		bool manualMode = false;
	};

	class DrawableObject: public Drawable, public Blendable {
	public:
		DrawableObject(usize const& layer = 0, bool const& manual = false);

		virtual ~DrawableObject();

		virtual void draw() {}

		Transform3D			trans;
		Shader				shader		= Shader::DEFAULT;
		Nullable<float>		pointSize	= nullptr;
		float				lineWidth	= 1.0;

	protected:
		void display(
			Vertex* const&		vertices,
			usize const&		count,
			CullMode const&		culling,
			FillMode const&		fill,
			DisplayMode const&	mode		= DisplayMode::ODM_TRIS,
			usize const&		instances	= 1
		);

		void prepare();

		uint vao, vbo;
	};

	template<class T>
	concept DrawableObjectType = Type::Subclass<T, DrawableObject>;
}

#endif // MAKAILIB_GRAPH_RENDERER_DRAWABLE_H
