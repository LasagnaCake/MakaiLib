#ifndef MAKAILIB_GRAPH_RENDERER_REFERENCE_H
#define MAKAILIB_GRAPH_RENDERER_REFERENCE_H

#include "../../../compat/ctl.hpp"

#include "../vertex.hpp"
#include "../color.hpp"

namespace Makai::Graph {
	using VertexFunction = Function<void(Vertex&)>;

	class Renderable;

	struct Empty {
		void destroy()		{onDestroy();					}
		void unbind()		{onUnbind();					}
		virtual ~Empty()	{onDestroy = onUnbind = []{};	}
		friend class Renderable;
	private:
		Signal<> onDestroy;
		Signal<> onUnbind;
	};

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpedantic"
	template<usize N>
	struct ShapeRef: public Empty {
		constexpr static usize SIZE = N;

		using Triangles = Span<Triangle*, SIZE>;

		ShapeRef(Triangle* const(& tris)[SIZE]) {
			for (usize i = 0; i < SIZE; ++i)
				this->tris[i] = tris[i];
		}

		virtual ~ShapeRef() {
			DEBUGLN("Deleting bound triangles (", SIZE, ")...");
			delete[] tris;
		};

		Triangles getBoundTriangles() {
			return Triangles(tris, SIZE);
		}

		virtual ShapeRef* reset()		= 0;
		virtual ShapeRef* transform()	= 0;

		virtual void forEachVertex(VertexFunction const& f) {
			for (usize i = 0; i < SIZE; ++i)
				for (usize j = 0; j < 3; ++j)
					f(((Vertex*)tris[i])[j]);
		}

		bool fixed		= true;
		bool visible	= true;

		Transform3D local;

		friend class Makai::Graph::Renderable;
	protected:
		Triangle** tris = new Triangle*[SIZE](nullptr);
	};
	#pragma GCC diagnostic pop

	class PlaneRef: public ShapeRef<2> {
	public:
		PlaneRef(
			Triangle* const(& tris)[2]
		);

		virtual ~PlaneRef() {}

		/// Sets the plane's origin.
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
		Vertex
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

	class TriangleRef: public ShapeRef<1> {
	public:
		TriangleRef(
			Triangle* const(& tris)[1]
		);

		virtual ~TriangleRef() {}

		/// Sets the triangle's origin.
		TriangleRef* setOrigin(
			Vector3 const& aPos = Vector3(+0, +1),
			Vector3 const& bPos = Vector3(-1, -1),
			Vector3 const& cPos = Vector3(+1, -1)
		);

		/// Transforms the triangle's origin and normals by a given transform.
		TriangleRef* setOrigin(Transform3D const& trans);

		TriangleRef* setUV(
			Vector2 const& aUV,
			Vector2 const& bUV,
			Vector2 const& cUV
		);

		TriangleRef* setColor(
			Vector4 const& aCol,
			Vector4 const& bCol,
			Vector4 const& cCol
		);

		TriangleRef* setColor(
			Vector4 const& col = Color::WHITE
		);

		TriangleRef* setNormal(
			Vector3 const& an,
			Vector3 const& bn,
			Vector3 const& cn
		);

		TriangleRef* setNormal(
			Vector3 const& n
		);

		/// Sets the triangle to its original state (last state set with setPosition).
		TriangleRef* reset() override final;

		TriangleRef* transform() override final;

		virtual void forEachVertex(VertexFunction const& f) override final;

		Vertex origin[3];

	protected:
		virtual void onTransform() {};

	private:
		Vertex
			*a	= nullptr,
			*b	= nullptr,
			*c	= nullptr;
	};

	template<class T>
	concept NotEmpty	= Makai::Type::Different<T, Empty>;

	template<class T>
	concept ShapeRefType	= requires {
		T::SIZE;
	} && Makai::Type::Derived<T, ShapeRef<T::SIZE>> && NotEmpty<T>;

	template<class T>
	concept PlaneRefType	= Makai::Type::Derived<T, PlaneRef> && NotEmpty<T>;

	template<class T>
	concept TriangleRefType	= Makai::Type::Derived<T, TriangleRef> && NotEmpty<T>;
}

#endif // MAKAILIB_GRAPH_RENDERER_REFERENCE_H
