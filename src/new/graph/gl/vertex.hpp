#ifndef MAKAILIB_GRAPH_VERTEX_H
#define MAKAILIB_GRAPH_VERTEX_H

namespace Makai::Graph {
	using VertexMap = HashMap<String, float>;

	struct Vertex {
		float
			x	= 0,
			y	= 0,
			z	= 0,
			u	= 0,
			v	= 0,
			r	= 1,
			g	= 1,
			b	= 1,
			a	= 1,
			nx	= 0,
			ny	= 0,
			nz	= 0
		;

		static Vertex from(VertexMap vm);
		constexpr static Vertex from(
			Vector3 const& position = 0,
			Vector2 const& uv		= 0,
			Vector4 const& color	= 1,
			Vector3 const& normal	= 0
		);

		static void setAttributes();
		static void enableAttributes();
		static void disableAttributes();

		// Vertex setters
		constexpr static void setPosition(RawVertex& vtx, Vector3 const& position);
		constexpr static void setUV(RawVertex& vtx, Vector2 const& uv);
		constexpr static void setColor(RawVertex& vtx, Vector4 const& color);
		constexpr static void setNormal(RawVertex& vtx, Vector3 const& normal);

		// Vertex Getters
		constexpr static Vector3 getPosition(RawVertex const& vtx);
		constexpr static Vector2 getUV(RawVertex const& vtx);
		constexpr static Vector4 getColor(RawVertex const& vtx);
		constexpr static Vector3 getNormal(RawVertex const& vtx);
	};

	struct Triangle {
		RawVertex verts[3];
	};

	constexpr Vertex INITIAL_VERTEX = {};
}

#endif // MAKAILIB_GRAPH_VERTEX_H
