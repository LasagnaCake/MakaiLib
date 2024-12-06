#ifndef MAKAILIB_GRAPH_RENDERER_DRAWABLE_H
#define MAKAILIB_GRAPH_RENDERER_DRAWABLE_H

#include "../../../compat/ctl.hpp"
#include "../material/material.hpp"
#include "../blend.hpp"
#include "../vertex.hpp"
#include "../global.hpp"
#include "mode.hpp"
#include "core.hpp"

/// @brief Graphical facilities.
namespace Makai::Graph {
	/// @brief Drawable object interface.
	class IDrawable {
	public:
		IDrawable(bool const manual = false);

		IDrawable(bool const manual, usize const layer);

		virtual ~IDrawable();

		IDrawable& setManual();

		IDrawable& setAuto(usize const renderLayer);

		IDrawable& setRenderLayer(usize const renderLayer);

		IDrawable& addToRenderLayer(usize const renderLayer);

		IDrawable& removeFromRenderLayer(usize const renderLayer);

		/// Renders the object to the screen.
		RenderCallback render = [&] {doRender();};

		bool active	= true;

	protected:
		virtual void draw() {}

	private:
		void doRender();

		/// Whether manually rendering or not.
		bool manualMode = false;
	};

	/// @brief OpenGL drawable object interface.
	class IGLDrawable: public IDrawable, public IBlendable {
	public:
		IGLDrawable(usize const layer = 0, bool const manual = false);

		virtual ~IGLDrawable();

		virtual void draw() = 0;

		Transform3D			trans;
		Shader				shader		= Shader::DEFAULT;
		Nullable<float>		pointSize	= nullptr;
		float				lineWidth	= 1.0;

	protected:
		void display(
			Vertex* const		vertices,
			usize const			count,
			CullMode const&		culling,
			FillMode const&		fill,
			DisplayMode const&	mode		= DisplayMode::ODM_TRIS,
			usize const			instances	= 1
		);

		void prepare();

		uint vao, vbo;
	};

	template<class T>
	concept DrawableObjectType = Makai::Type::Subclass<T, IGLDrawable>;
}

#endif // MAKAILIB_GRAPH_RENDERER_DRAWABLE_H
