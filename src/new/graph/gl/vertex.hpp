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
		) {
			Vertex res;
			res.x	= position.x;
			res.y	= position.y;
			res.z	= position.z;
			res.u	= uv.x;
			res.v	= uv.y;
			res.r	= color.x;
			res.g	= color.y;
			res.b	= color.z;
			res.a	= color.w;
			res.nx	= normal.x;
			res.ny	= normal.y;
			res.nz	= normal.z;
			return res;
		}

		static void setAttributes();
		static void enableAttributes();
		static void disableAttributes();

		// Vertex setters
		constexpr static void setPosition(Vertex& vtx, Vector3 const& position) {
			vtx.x = position.x;
			vtx.y = position.y;
			vtx.z = position.z;
		}

		constexpr static void setUV(Vertex& vtx, Vector2 const& uv) {
			vtx.u = uv.x;
			vtx.v = uv.y;
		}

		constexpr static void setColor(Vertex& vtx, Vector4 const& color) {
			vtx.r = color.x;
			vtx.g = color.y;
			vtx.b = color.z;
			vtx.a = color.w;
		}

		constexpr static void setNormal(Vertex& vtx, Vector3 const& normal) {
			vtx.nx = normal.x;
			vtx.ny = normal.y;
			vtx.nz = normal.z;
		}

		// Vertex Getters
		constexpr static Vector3 getPosition(Vertex const& vtx) {
			return Vector3(vtx.x, vtx.y, vtx.z);
		}

		constexpr static Vector2 getUV(Vertex const& vtx) {
			return Vector2(vtx.u, vtx.v);
		}

		constexpr static Vector4 getColor(Vertex const& vtx) {
			return Vector4(vtx.r, vtx.g, vtx.b, vtx.a);
		}

		constexpr static Vector3 getNormal(Vertex const& vtx) {
			return Vector3(vtx.nx, vtx.ny, vtx.nz);
		}
	};

	struct Triangle {
		Vertex verts[3];
	};

	constexpr Vertex INITIAL_VERTEX = {};
}

#endif // MAKAILIB_GRAPH_VERTEX_H
