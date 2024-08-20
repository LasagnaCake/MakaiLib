#ifndef MAKAILIB_GRAPH_RENDERER_DRAWABLE_H
#define MAKAILIB_GRAPH_RENDERER_DRAWABLE_H

#include "../../ctl/ctl.hpp"
#include "../material/material.hpp"
#include "../blend.hpp"
#include "render.hpp"

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
	/*
	GL_POINTS,
	GL_LINE_STRIP,
	GL_LINE_LOOP,
	GL_LINES,
	GL_TRIANGLE_STRIP,
	GL_TRIANGLE_FAN,
	GL_TRIANGLES,
	GL_LINES_ADJACENCY,
	GL_LINE_STRIP_ADJACENCY,
	GL_TRIANGLES_ADJACENCY,
	GL_TRIANGLE_STRIP_ADJACENCY,
	GL_PATCHES
	*/
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

	template<Material::ValidObjectMaterial T>
	class DrawableObject: public Drawable, public Blendable {
	public:
		typedef T MaterialType;

		DrawableObject(usize const& layer = 0, bool const& manual = false);

		virtual ~DrawableObject();

		virtual void draw() {}

		MaterialType		material;
		Transform3D			trans;
		Shader				shader		= defaultShader;
		Nullable<float>		pointSize	= nullptr;
		float				lineWidth	= 1.0;

	protected:
		void display(Vertex* const& vertices, usize const& count, ObjectDisplayMode const& mode = ObjectDisplayMode::ODM_TRIANGLES);

		void setShaderData();

		usize vao, vbo;
	};
}

#endif // MAKAILIB_GRAPH_RENDERER_DRAWABLE_H
