#ifndef LEGACY_COLLISION_SYSTEM_H
#define LEGACY_COLLISION_SYSTEM_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"

namespace Collision {
	namespace {
		using
		VecMath::Points2D,
		VecMath::Points3D,
		VecMath::angleTo,
		VecMath::center,
		VecMath::Transform2D,
		std::unordered_map,
		std::string,
		std::vector;
	}

	// Concepts, here I go!
	struct Bounds2D {};
	template<typename T>
	concept CollisionType = Type::Subclass<T, Bounds2D>;

	/**
	************************************
	*                                  *
	*  2D Box Boundary Data Structure  *
	*                                  *
	************************************
	*/
	struct Box {
		Vector2 min;
		Vector2 max;
	};

	/**
	***************************************
	*                                     *
	*  2D Circle Boundary Data Structure  *
	*                                     *
	***************************************
	*/
	struct Circle {
		Vector2 position;
		Vector2 radius = 1;
		float rotation = 0;
	};

	/**
	****************************************
	*                                      *
	*  2D Capsule Boundary Data Structure  *
	*                                      *
	****************************************
	*/
	struct Capsule {
		Vector2 position;
		Vector2 width = 1;
		float length = 1;
		float angle = 0;
		float rotation = 0;
	};

	/**
	**********************************************
	*                                            *
	*  2D Ray (Raycast) Boundary Data Structure  *
	*                                            *
	**********************************************
	*/
	struct Ray {
		Vector2 position;
		float length = 1;
		float angle = 0;
	};

	template<size_t COUNT = 0>
	struct BaseShape {
		Transform2D trans;
		Vector2	points[COUNT];
		Vector2	normals[COUNT];
	};

	/**
	***********************************************
	*                                             *
	*  2D Convex Polygon Boundary Data Structure  *
	*                                             *
	***********************************************
	*/
	struct Polygon {
		Transform2D trans;
		size_t	count = 0;
		Vector2	points[C2_MAX_POLYGON_VERTS];
		Vector2	normals[C2_MAX_POLYGON_VERTS];
	};

	/**
	***************************************
	*                                     *
	*  2D Shape Boundary Data Structures  *
	*                                     *
	***************************************
	*/
	struct Triangle:	BaseShape<3> {};
	struct Quad:		BaseShape<4> {};
	struct Shape {
		Transform2D trans;
		vector<Triangle> triangles;
	};

	struct PolyX {
		c2Poly	p;
		c2x		t;
	};

	#define	CDT_BOUNDS(CLASS) struct CLASS##Bounds2D: CLASS, Bounds2D {}
	CDT_BOUNDS(Circle);
	CDT_BOUNDS(Box);
	CDT_BOUNDS(Capsule);
	CDT_BOUNDS(Ray);
	CDT_BOUNDS(Polygon);
	CDT_BOUNDS(Triangle);
	CDT_BOUNDS(Quad);
	CDT_BOUNDS(Shape);
	#undef	CDT_BOUNDS

	constexpr float getRadius(Vector2 const& radius, float const& angle) {
		return
			(Math::absin(angle) * radius.x)
		+	(Math::abcos(angle) * radius.y)
		;
	}

	inline CapsuleBounds2D makeCapsuleBounds(Vector2 const& from, Vector2 const& to, Vector2 const& width = 1) {
		return CapsuleBounds2D {
			from,
			width,
			from.distanceTo(to),
			VecMath::angleTo(from, to)
		};
	}

	inline BoxBounds2D makeBoundsAB(Vector2 const& a, Vector2 const& b) {
		return BoxBounds2D {
			Vector2(
				Math::min(a.x, b.x),
				Math::min(a.y, b.y)
			),
			Vector2(
				Math::max(a.x, b.x),
				Math::max(a.y, b.y)
			)
		};
	}

	inline BoxBounds2D makeBounds(Vector2 const& position, Vector2 const& size) {
		Vector2 hSize = size / 2.0;
		return makeBoundsAB(
			position - hSize,
			position + hSize
		);
	}

	// CUTE conversions

	inline c2AABB cuteify(BoxBounds2D const& b) {
		return c2AABB{c2v{b.min.x, b.min.y}, c2v{b.max.x, b.max.y}};
	}

	inline c2Circle cuteify(CircleBounds2D const& b) {
		return c2Circle{c2v{b.position.x, b.position.y}, b.radius.average()};
	}

	inline c2Capsule cuteify(CapsuleBounds2D const& b) {
		Vector2 end = VecMath::angleV2(b.angle) * b.length + b.position;
		return c2Capsule{
			c2v{b.position.x, b.position.y},
			c2v{end.x, end.y},
			b.width.average()
		};
	}

	inline c2Ray cuteify(RayBounds2D const& b) {
		Vector2 normal = VecMath::angleV2(b.angle);
		return c2Ray{
			c2v{b.position.x, b.position.y},
			c2v{normal.x, normal.y},
			b.length
		};
	}

	inline PolyX cuteify(PolygonBounds2D const& b) {
		c2Poly shape = c2Poly{b.count};
		auto& scale = b.trans.scale;
		for SRANGE(i, 0, b.count) {
			Vector2
				p = b.points[i] * scale,
				n = (b.normals[i] * scale).normalized();
			;
			shape.verts[i].x = p.x;
			shape.verts[i].y = p.y;
			shape.norms[i].x = n.x;
			shape.norms[i].y = n.y;
		}
		Vector2 av = VecMath::angleV2(b.trans.rotation);
		c2x trans = {
			c2v{b.trans.position.x, b.trans.position.y},
			c2r{av.x, av.y}
		};
		return PolyX{shape, trans};
	}

	template <size_t COUNT = 0>
	inline PolyX cuteify(BaseShape<COUNT> const& b) {
		c2Poly shape = c2Poly{COUNT};
		auto& scale = b.trans.scale;
		for SRANGE(i, 0, COUNT) {
			Vector2
				p = b.points[i] * scale,
				n = (b.normals[i] * scale).normalized();
			;
			shape.verts[i].x = p.x;
			shape.verts[i].y = p.y;
			shape.norms[i].x = n.x;
			shape.norms[i].y = n.y;
		}
		Vector2 av = VecMath::angleV2(b.trans.rotation);
		c2x trans = {
			c2v{b.trans.position.x, b.trans.position.y},
			c2r{av.x, av.y}
		};
		return PolyX{shape, trans};
	}

	// Shape to Point collision detection code

	inline bool withinBounds(Vector2 const& point, BoxBounds2D const& area) {
		return (
			( area.min.x < point.x) && (point.x < area.max.x)
		) && (
			( area.min.y < point.y) && (point.y < area.max.y)
		);
	}

	inline bool withinBounds(Vector2 const& point, CircleBounds2D const& area) {
		// Calculate circle stretch
		float angle		= point.angleTo(area.position);
		float radius	= getRadius(area.radius, angle + area.rotation);
		// Check collision
		return point.distanceTo(area.position) < (radius);
	}

	inline bool withinBounds(Vector2 const& a, CapsuleBounds2D const& b) {
		// Get distance between targets
		float distance = a.distanceTo(b.position);
		// If too distant, return
		if (distance > b.length + b.width.max()) return false;
		// Get ray position to check
		Vector2 rayPosition = VecMath::angleV2(b.rotation) * distance + b.position;
		// Check collision
		CircleBounds2D target{rayPosition, b.width};
		return withinBounds(a, target);
	}

	// Shape to Shape collision detection code

	// Circle

	inline bool withinBounds(CircleBounds2D const& a, CircleBounds2D const& b) {
		// Calculate circle stretches
		float angle		= a.position.angleTo(b.position);
		float radiusA	= getRadius(a.radius, angle + a.rotation);
		float radiusB	= getRadius(b.radius, angle + b.rotation);
		// Check collision
		return a.position.distanceTo(b.position) < (radiusA + radiusB);
	}

	// Box

	bool withinBounds(BoxBounds2D const& a, BoxBounds2D const& b) {
		// Get overlap on X
		bool overlapX = (
			(b.min.x < a.min.x) && (a.min.x < b.max.x)
		) || (
			(b.min.x < a.max.x) && (a.max.x < b.max.x)
		);
		// Get overlap on Y
		bool overlapY = (
			(b.min.y < a.min.y) && (a.min.y < b.max.y)
		) || (
			(b.min.y < a.max.y) && (a.max.y < b.max.y)
		);
		// Return if both axis overlap (i.e. collision)
		return overlapX && overlapY;
	}

	bool withinBounds(CircleBounds2D const& a, BoxBounds2D const& b) {
		return c2CircletoAABB(cuteify(a), cuteify(b));
	}

	// Ray

	inline bool withinBounds(CircleBounds2D const& a, CapsuleBounds2D const& b) {
		// Get distance between targets
		float distance = a.position.distanceTo(b.position);
		// If too distant, return
		if (distance > b.length + b.width.max() + a.radius.max()) return false;
		// Get ray position to check
		Vector2 rayPosition = VecMath::angleV2(b.angle) * distance + b.position;
		// Check collision
		CircleBounds2D target{rayPosition, b.width, b.angle + b.rotation};
		return withinBounds(a, target);
	}

	inline bool withinBounds(BoxBounds2D const& a, CapsuleBounds2D const& b) {
		return c2AABBtoCapsule(cuteify(a), cuteify(b));
	}

	inline bool withinBounds(CapsuleBounds2D const& a, CapsuleBounds2D const& b) {
		return c2CapsuletoCapsule(cuteify(a), cuteify(b));
	}

	// Line

	inline bool withinBounds(BoxBounds2D const& a, RayBounds2D const& b, c2Raycast* result = nullptr) {
		c2Raycast r;
		return c2RaytoAABB(cuteify(b), cuteify(a), result ? result : &r);
	}

	inline bool withinBounds(CircleBounds2D const& a, RayBounds2D const& b, c2Raycast* result = nullptr) {
		c2Raycast r;
		return c2RaytoCircle(cuteify(b), cuteify(a), result ? result : &r);
	}

	inline bool withinBounds(CapsuleBounds2D const& a, RayBounds2D const& b, c2Raycast* result = nullptr) {
		c2Raycast r;
		return c2RaytoCapsule(cuteify(b), cuteify(a), result ? result : &r);
	}

	inline bool withinBounds(RayBounds2D const& a, RayBounds2D const& b, c2Raycast* result = nullptr) {
		c2Raycast r;
		CapsuleBounds2D ray = {b.position, b.length, 0.001, b.angle};
		return c2RaytoCapsule(cuteify(b), cuteify(ray), result ? result : &r);
	}

	// TODO: Collision code for between Polygons and GenericShapes
	// Polygon

	inline bool withinBounds(PolygonBounds2D const& a, PolygonBounds2D const& b) {
		PolyX pa = cuteify(a), pb = cuteify(b);
		return c2PolytoPoly(&pa.p, &pa.t, &pb.p, &pb.t);
	}

	inline bool withinBounds(CircleBounds2D const& a, PolygonBounds2D const& b) {
		PolyX pb = cuteify(b);
		return c2CircletoPoly(cuteify(a), &pb.p, &pb.t);
	}

	inline bool withinBounds(BoxBounds2D const& a, PolygonBounds2D const& b) {
		PolyX pb = cuteify(b);
		return c2AABBtoPoly(cuteify(a), &pb.p, &pb.t);
	}

	inline bool withinBounds(CapsuleBounds2D const& a, PolygonBounds2D const& b) {
		PolyX pb = cuteify(b);
		return c2CapsuletoPoly(cuteify(a), &pb.p, &pb.t);
	}

	inline bool withinBounds(RayBounds2D const& a, PolygonBounds2D const& b, c2Raycast* result = nullptr) {
		c2Raycast r;
		PolyX pb = cuteify(b);
		return c2RaytoPoly(cuteify(a), &pb.p, &pb.t, result ? result : &r);
	}

	// Triangle

	inline bool withinBounds(TriangleBounds2D const& a, TriangleBounds2D const& b) {
		PolyX pa = cuteify(a), pb = cuteify(b);
		return c2PolytoPoly(&pa.p, &pa.t, &pb.p, &pb.t);
	}

	inline bool withinBounds(PolygonBounds2D const& a, TriangleBounds2D const& b) {
		PolyX pa = cuteify(a), pb = cuteify(b);
		return c2PolytoPoly(&pa.p, &pa.t, &pb.p, &pb.t);
	}

	inline bool withinBounds(CircleBounds2D const& a, TriangleBounds2D const& b) {
		PolyX pb = cuteify(b);
		return c2CircletoPoly(cuteify(a), &pb.p, &pb.t);
	}

	inline bool withinBounds(BoxBounds2D const& a, TriangleBounds2D const& b) {
		PolyX pb = cuteify(b);
		return c2AABBtoPoly(cuteify(a), &pb.p, &pb.t);
	}

	inline bool withinBounds(CapsuleBounds2D const& a, TriangleBounds2D const& b) {
		PolyX pb = cuteify(b);
		return c2CapsuletoPoly(cuteify(a), &pb.p, &pb.t);
	}

	inline bool withinBounds(RayBounds2D const& a, TriangleBounds2D const& b, c2Raycast* result = nullptr) {
		c2Raycast r;
		PolyX pb = cuteify(b);
		return c2RaytoPoly(cuteify(a), &pb.p, &pb.t, result ? result : &r);
	}

	// Quad

	inline bool withinBounds(QuadBounds2D const& a, QuadBounds2D const& b) {
		PolyX pa = cuteify(a), pb = cuteify(b);
		return c2PolytoPoly(&pa.p, &pa.t, &pb.p, &pb.t);
	}

	inline bool withinBounds(TriangleBounds2D const& a, QuadBounds2D const& b) {
		PolyX pa = cuteify(a), pb = cuteify(b);
		return c2PolytoPoly(&pa.p, &pa.t, &pb.p, &pb.t);
	}

	inline bool withinBounds(PolygonBounds2D const& a, QuadBounds2D const& b) {
		PolyX pa = cuteify(a), pb = cuteify(b);
		return c2PolytoPoly(&pa.p, &pa.t, &pb.p, &pb.t);
	}

	inline bool withinBounds(CircleBounds2D const& a, QuadBounds2D const& b) {
		PolyX pb = cuteify(b);
		return c2CircletoPoly(cuteify(a), &pb.p, &pb.t);
	}

	inline bool withinBounds(BoxBounds2D const& a, QuadBounds2D const& b) {
		PolyX pb = cuteify(b);
		return c2AABBtoPoly(cuteify(a), &pb.p, &pb.t);
	}

	inline bool withinBounds(CapsuleBounds2D const& a, QuadBounds2D const& b) {
		PolyX pb = cuteify(b);
		return c2CapsuletoPoly(cuteify(a), &pb.p, &pb.t);
	}

	inline bool withinBounds(RayBounds2D const& a, QuadBounds2D const& b, c2Raycast* result = nullptr) {
		c2Raycast r;
		PolyX pb = cuteify(b);
		return c2RaytoPoly(cuteify(a), &pb.p, &pb.t, result ? result : &r);
	}

	// Shape

	inline bool withinBounds(ShapeBounds2D const& a, ShapeBounds2D const& b) {
		TriangleBounds2D ta;
		TriangleBounds2D tb;
		bool colliding = true;
		for (Triangle const& tra: b.triangles) {
			for (Triangle const& trb: a.triangles) {
				ta = (TriangleBounds2D)tra;
				ta = (TriangleBounds2D)trb;
				ta.trans = a.trans;
				tb.trans = b.trans;
				colliding = colliding && withinBounds(ta, tb);
			}
		}
		return colliding;
	}

	inline bool withinBounds(TriangleBounds2D const& a, ShapeBounds2D const& b) {
		TriangleBounds2D tb;
		bool colliding = true;
		for (Triangle const& t: b.triangles) {
			tb = (TriangleBounds2D)t;
			tb.trans = b.trans;
			colliding = colliding && withinBounds(a, tb);
		}
		return colliding;
	}

	inline bool withinBounds(QuadBounds2D const& a, ShapeBounds2D const& b) {
		TriangleBounds2D tb;
		bool colliding = true;
		for (Triangle const& t: b.triangles) {
			tb = (TriangleBounds2D)t;
			tb.trans = b.trans;
			colliding = colliding && withinBounds(tb, a);
		}
		return colliding;
	}

	inline bool withinBounds(PolygonBounds2D const& a, ShapeBounds2D const& b) {
		TriangleBounds2D tb;
		bool colliding = true;
		for (Triangle const& t: b.triangles) {
			tb = (TriangleBounds2D)t;
			tb.trans = b.trans;
			colliding = colliding && withinBounds(a, tb);
		}
		return colliding;
	}

	inline bool withinBounds(CircleBounds2D const& a, ShapeBounds2D const& b) {
		TriangleBounds2D tb;
		bool colliding = true;
		for (Triangle const& t: b.triangles) {
			tb = (TriangleBounds2D)t;
			tb.trans = b.trans;
			colliding = colliding && withinBounds(a, tb);
		}
		return colliding;
	}

	inline bool withinBounds(BoxBounds2D const& a, ShapeBounds2D const& b) {
		TriangleBounds2D tb;
		bool colliding = true;
		for (Triangle const& t: b.triangles) {
			tb = (TriangleBounds2D)t;
			tb.trans = b.trans;
			colliding = colliding && withinBounds(a, tb);
		}
		return colliding;
	}

	inline bool withinBounds(CapsuleBounds2D const& a, ShapeBounds2D const& b) {
		TriangleBounds2D tb;
		bool colliding = true;
		for (Triangle const& t: b.triangles) {
			tb = (TriangleBounds2D)t;
			tb.trans = b.trans;
			colliding = colliding && withinBounds(a, tb);
		}
		return colliding;
	}

	inline bool withinBounds(RayBounds2D const& a, ShapeBounds2D const& b, c2Raycast** results = nullptr) {
		TriangleBounds2D tb;
		bool colliding = true;
		size_t i = 0;
		for (Triangle const& t: b.triangles) {
			tb = (TriangleBounds2D)t;
			tb.trans = b.trans;
			colliding = colliding && withinBounds(a, tb, results ? results[i] : nullptr);
			i++;
		}
		return colliding;
	}

	// Flipped Functions

	template<CollisionType T>
	inline bool withinBounds(CircleBounds2D const& a, T const& b) {
		return withinBounds(b, a);
	}

	template<CollisionType T>
	inline bool withinBounds(BoxBounds2D const& a, T const& b) {
		return withinBounds(b, a);
	}

	template<CollisionType T>
	inline bool withinBounds(CapsuleBounds2D const& a, T const& b) {
		return withinBounds(b, a);
	}

	template<CollisionType T>
	inline bool withinBounds(RayBounds2D const& a, T const& b, c2Raycast* result = nullptr) {
		return withinBounds(b, a, result);
	}

	template<CollisionType T>
	inline bool withinBounds(PolygonBounds2D const& a, T const& b) {
		return withinBounds(b, a);
	}

	template<CollisionType T>
	inline bool withinBounds(TriangleBounds2D const& a, T const& b) {
		return withinBounds(b, a);
	}

	template<CollisionType T>
	inline bool withinBounds(QuadBounds2D const& a, T const& b) {
		return withinBounds(b, a);
	}

	template<CollisionType T>
	inline bool withinBounds(ShapeBounds2D const& a, T const& b) {
		return withinBounds(b, a);
	}

	// Clamping functions

	inline Vector2 getBounded(Vector2 const& point, BoxBounds2D const& box) {
		return point.clamped(box.min,box.max);
	}

	/**
	****************************************
	*                                      *
	*  Simple 2D Collision Data Structure  *
	*                                      *
	****************************************
	*/
	template<CollisionType T>
	struct AreaCollisionData {
		T shape			= T{};
		bool enabled	= true;
	};

	typedef AreaCollisionData<CircleBounds2D>	AreaCircleData;
	typedef AreaCollisionData<BoxBounds2D>		AreaBoxData;
	typedef AreaCollisionData<CapsuleBounds2D>	AreaCapsuleData;
	typedef AreaCollisionData<RayBounds2D>		AreaRayData;
	typedef AreaCollisionData<TriangleBounds2D>	AreaTriangleData;
	typedef AreaCollisionData<QuadBounds2D>		AreaQuadData;
	typedef AreaCollisionData<ShapeBounds2D>	AreaShapeData;
	typedef AreaCollisionData<PolygonBounds2D>	AreaPolygonData;

	enum class CollisionType2D {
		CT2D_CIRCLE,
		CT2D_BOX,
		CT2D_CAPSULE,
		CT2D_RAY,
		CT2D_TRIANGLE,
		CT2D_QUAD,
		CT2D_SHAPE,
		CT2D_POLYGON,
		CT2D_NULL
	};

	template <CollisionType A, CollisionType B>
	bool isColliding(AreaCollisionData<A> const& a, AreaCollisionData<B> const& b) {
		if (!(a.enabled && b.enabled))
			return false;
		return withinBounds(a.shape, b.shape);
	}
}

#pragma GCC diagnostic pop

#endif // LEGACY_COLLISION_SYSTEM_H
