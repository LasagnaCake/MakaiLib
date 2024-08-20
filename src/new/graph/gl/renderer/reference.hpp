#ifndef MAKAILIB_GRAPH_RENDERER_REFERENCE_H
#define MAKAILIB_GRAPH_RENDERER_REFERENCE_H

#include "../vertex.hpp"

namespace Makai::Graph {
	using VertexFunction = Function<void(RawVertex&)>;

	struct Empty {
		void destroy()		{onDestroy();					}
		void unbind()		{onUnbind();					}
		virtual ~Empty()	{onDestroy = onUnbind = []{};	}
		friend class Renderable;
	private:
		Event::Signal onDestroy;
		Event::Signal onUnbind;
	};

	template<usize N>
	struct Shape: public Empty {
		constexpr static usize COUNT = N;

		using Triangles = Span<Triangle*, COUNT>;

		Shape(Decay::AsType<Triangle*[COUNT]> const& tries);

		virtual ~Shape();

		virtual Shape* reset()		= 0;
		virtual Shape* transform()	= 0;

		Triangles getBoundTriangles();

		virtual void forEachVertex(VertexFunction const& f);

		bool fixed		= true;
		bool visible	= true;

		Transform3D local;

		friend class RenderData::Renderable;
	protected:
		Triangle** tris = new Triangle*[COUNT](nullptr);
	};

	class PlaneRef: public Shape<2> {
	public:
		PlaneRef(
			Triangle* const(& tris)[2]
		);

		virtual ~PlaneRef() {}

		/// Sets the plane's posOrigin.
		PlaneRef* setOrigin(
			Vector3 const& tlPos = Vector3(-1, +1),
			Vector3 const& trPos = Vector3(+1, +1),
			Vector3 const& blPos = Vector3(-1, -1),
			Vector3 const& brPos = Vector3(+1, -1)
		);

		/// Transforms the plane's origin and normals by a given transform.
		PlaneRef* setOrigin(Transform3D const& trans);

		PlaneRef* setUV(
			Vector2 const& tlUV,
			Vector2 const& trUV,
			Vector2 const& blUV,
			Vector2 const& brUV
		);

		PlaneRef* setColor(
			Vector4 const& tlCol,
			Vector4 const& trCol,
			Vector4 const& blCol,
			Vector4 const& brCol
		);

		PlaneRef* setColor(Vector4 const& col = Color::WHITE);

		PlaneRef* setNormal(
			Vector3 const& tln,
			Vector3 const& trn,
			Vector3 const& bln,
			Vector3 const& brn
		);

		PlaneRef* setNormal(Vector3 const& n);

		/// Sets the plane to its original state (last state set with setPosition).
		PlaneRef* reset() override final;

		PlaneRef* transform() override final;

		void forEachVertex(VertexFunction const& f) override final;

		Vertex	origin[4];

	protected:
		virtual void onTransform() {};

	private:
		RawVertex
			*tl		= nullptr,
			*tr1	= nullptr,
			*tr2	= nullptr,
			*bl1	= nullptr,
			*bl2	= nullptr,
			*br		= nullptr;
	};

	class AnimatedPlaneRef: public PlaneRef {
	public:
		using PlaneRef::PlaneRef;
		Vector2 frame;
		Vector2 size = Vector2(1);

		void onTransform() override;
	};
}

#endif // MAKAILIB_GRAPH_RENDERER_REFERENCE_H
