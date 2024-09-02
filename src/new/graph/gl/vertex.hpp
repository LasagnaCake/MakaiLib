#ifndef MAKAILIB_GRAPH_VERTEX_H
#define MAKAILIB_GRAPH_VERTEX_H

#include "../../ctl/ctl.hpp"

namespace Makai::Graph {
	using VertexMap = HashMap<String, float>;

	namespace Base {
		#pragma pack(1)
		struct BasicVertex {
			Vector3 position	= 0;
			Vector2 uv			= 0;
			Vector4 color		= 1;
			Vector3 normal		= 0;
		};
	}

	#pragma pack(1)
	struct Vertex: public Base::BasicVertex {
		using BaseType = Base::BasicVertex;

		using BaseType::position;
		using BaseType::uv;
		using BaseType::color;
		using BaseType::normal;

		constexpr static usize COMPONENT_COUNT = (sizeof(BaseType) / sizeof(float));

		static_assert(COMPONENT_COUNT == (3+2+4+3), "Vertex size is off...");

		static VertexMap defaultMap();

		Vertex(VertexMap const& vm);

		constexpr Vertex(Arguments<float> const& args) {
			if(args.size() == 0) return;
			usize i =
				args.size() > COMPONENT_COUNT
			?	COMPONENT_COUNT
			:	args.size()
			;
			float* component = &position.x;
			for (float v: args) {
				component[--i] = v;
				if (!i) break;
			}
		}

		constexpr Vertex(
			float const& x,
			float const& y	= 0,
			float const& z	= 0,
			float const& u	= 0,
			float const& v	= 0,
			float const& r	= 1,
			float const& g	= 1,
			float const& b	= 1,
			float const& a	= 1,
			float const& nx	= 0,
			float const& ny	= 0,
			float const& nz	= 0
		): Vertex(
			Vector3(x, y, z),
			Vector2(u, v),
			Vector4(r, g, b, a),
			Vector3(nx, ny, nz)
		) {}

		constexpr Vertex(
			Vector3 const& position = 0,
			Vector2 const& uv		= 0,
			Vector4 const& color	= 1,
			Vector3 const& normal	= 0
		):	BaseType{position, uv, color, normal} {}

		static void setAttributes();
		static void enableAttributes();
		static void disableAttributes();
	};

	static_assert(sizeof(Vertex) == sizeof(Base::BasicVertex), "Vertex size is off...");

	struct Triangle {
		Vertex verts[3];
	};

	constexpr Vertex INITIAL_VERTEX = Vertex();
}

#endif // MAKAILIB_GRAPH_VERTEX_H
