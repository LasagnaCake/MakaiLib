#ifndef MAKAILIB_GRAPH_RENDERER_REFERENCE_H
#define MAKAILIB_GRAPH_RENDERER_REFERENCE_H

#include "../../../compat/ctl.hpp"

#include "../vertex.hpp"
#include "../color.hpp"

namespace Makai::Graph {
	class Renderable;
	struct IReference;

	struct IReference {
		IReference(List<Triangle*> const& triangles, Renderable& parent):
			triangles(triangles),
			parent(parent) {}

		virtual ~IReference() {destroy();}

		using Triangles = Span<Triangle* const>;

		Triangles getBoundTriangles() {
			return Triangles(triangles.begin(), triangles.end());
		}

		virtual Handle<IReference> reset()		= 0;
		virtual Handle<IReference> transform()	= 0;
		
	protected:
		List<Triangle*> const 	triangles;
		Renderable& 			parent;
	private:
		void destroy();
		void unbind();
	};

	using VertexFunction = Function<void(Vertex&)>;

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpedantic"
	template<usize N>
	struct ShapeRef: public IReference {
		constexpr static usize SIZE = N;

		static_assert(N > 0, "Empty shapes are invalid!");

		ShapeRef(
			List<Triangle*> const& triangles,
			Renderable& parent
		): IReference(triangles, parent) {}

		virtual ~ShapeRef() {}

		virtual void forEachVertex(VertexFunction const& f) {
			for (usize i = 0; i < SIZE; ++i)
				for (usize j = 0; j < 3; ++j)
					f(((Vertex*)triangles[i])[j]);
		}

		virtual Handle<IReference> reset()		= 0;
		virtual Handle<IReference> transform()	= 0;

		bool fixed		= true;
		bool visible	= true;

		Transform3D local;
	};

	#pragma GCC diagnostic pop

	class PlaneRef: public ShapeRef<2> {
	public:
		PlaneRef(
			List<Triangle*> const& triangles,
			Renderable& parent
		);

		virtual ~PlaneRef() {}

		/// Sets the plane's origin.
		Handle<PlaneRef> setOrigin(
			Vector3 const& tlPos = Vector3(-1, +1),
			Vector3 const& trPos = Vector3(+1, +1),
			Vector3 const& blPos = Vector3(-1, -1),
			Vector3 const& brPos = Vector3(+1, -1)
		);

		/// Transforms the plane's origin and normals by a given transform.
		Handle<PlaneRef> setOrigin(Transform3D const& trans);

		Handle<PlaneRef> setUV(
			Vector2 const& tlUV,
			Vector2 const& trUV,
			Vector2 const& blUV,
			Vector2 const& brUV
		);

		Handle<PlaneRef> setColor(
			Vector4 const& tlCol,
			Vector4 const& trCol,
			Vector4 const& blCol,
			Vector4 const& brCol
		);

		Handle<PlaneRef> setColor(Vector4 const& col = Color::WHITE);

		Handle<PlaneRef> setNormal(
			Vector3 const& tln,
			Vector3 const& trn,
			Vector3 const& bln,
			Vector3 const& brn
		);

		Handle<PlaneRef> setNormal(Vector3 const& n);

		/// Sets the plane to its original state (last state set with setPosition).
		Handle<IReference> reset() override final;

		Handle<IReference> transform() override final;

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
			List<Triangle*> const& triangles,
			Renderable& parent
		);

		virtual ~TriangleRef() {}

		/// Sets the triangle's origin.
		Handle<TriangleRef> setOrigin(
			Vector3 const& aPos = Vector3(+0, +1),
			Vector3 const& bPos = Vector3(-1, -1),
			Vector3 const& cPos = Vector3(+1, -1)
		);

		/// Transforms the triangle's origin and normals by a given transform.
		Handle<TriangleRef> setOrigin(Transform3D const& trans);

		Handle<TriangleRef> setUV(
			Vector2 const& aUV,
			Vector2 const& bUV,
			Vector2 const& cUV
		);

		Handle<TriangleRef> setColor(
			Vector4 const& aCol,
			Vector4 const& bCol,
			Vector4 const& cCol
		);

		Handle<TriangleRef> setColor(
			Vector4 const& col = Color::WHITE
		);

		Handle<TriangleRef> setNormal(
			Vector3 const& an,
			Vector3 const& bn,
			Vector3 const& cn
		);

		Handle<TriangleRef> setNormal(
			Vector3 const& n
		);

		/// Sets the triangle to its original state (last state set with setPosition).
		Handle<IReference> reset() override final;

		Handle<IReference> transform() override final;

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
	concept NotEmpty	= Makai::Type::Different<T, IReference>;

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
