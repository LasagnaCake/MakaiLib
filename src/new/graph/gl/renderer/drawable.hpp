#ifndef MAKAILIB_GRAPH_RENDERER_DRAWABLE_H
#define MAKAILIB_GRAPH_RENDERER_DRAWABLE_H

#include "../../../ctl/ctl.hpp"
#include "../material/material.hpp"
#include "../blend.hpp"
#include "core.hpp"

namespace Makai::Graph {
	class Drawable {
	public:
		Drawable(usize const& layer = 0, bool const& manual = false);

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

	enum class ObjectDisplayMode {
		ODM_POINTS,
		ODM_LINE_STRIP,
		ODM_LINE_LOOP,
		ODM_LINES,
		ODM_TRIANGLE_STRIP,
		ODM_TRIANGLE_FAN,
		ODM_TRIANGLES,
		ODM_LINES_ADJACENCY,
		ODM_LINE_STRIP_ADJACENCY,
		ODM_TRIANGLES_ADJACENCY,
		ODM_TRIANGLE_STRIP_ADJACENCY,
		ODM_PATCHES
	};

	class DrawableObject: public Drawable, public Blendable {
	public:
		DrawableObject(usize const& layer = 0, bool const& manual = false);

		virtual ~DrawableObject();

		virtual void draw() {}

		Transform3D			trans;
		Shader				shader		= defaultShader;
		Nullable<float>		pointSize	= nullptr;
		float				lineWidth	= 1.0;

	protected:
		void display(Vertex* const& vertices, usize const& count, ObjectDisplayMode const& mode = ObjectDisplayMode::ODM_TRIANGLES);

		void prepare();

		usize vao, vbo;
	};

	template<class T>
	concept DrawableObjectType = Type::Subclass<T, DrawableObject>;
}

#endif // MAKAILIB_GRAPH_RENDERER_DRAWABLE_H
