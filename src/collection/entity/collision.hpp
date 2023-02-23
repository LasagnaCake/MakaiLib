#ifndef COLLISION_2D_3D_OBJECT_H
#define COLLISION_2D_3D_OBJECT_H

#include "core.hpp"
#include "2d3d.hpp"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#define CUTE_C2_IMPLEMENTATION
#include "../libs/cute_c2.h"
#pragma GCC diagnostic pop

#include <unordered_map>

namespace CollisionData {
	namespace {
		using
		VecMath::Points2D,
		VecMath::Points3D,
		VecMath::angleTo,
		VecMath::center,
		std::unordered_map,
		std::string,
		std::runtime_error;

		using namespace Vector;
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
		float radius	= 1;
	};

	/**
	**********************************************
	*                                            *
	*  2D Ray (Capsule) Boundary Data Structure  *
	*                                            *
	**********************************************
	*/
	struct Ray {
		Vector2 position;
		float length = 1;
		float width	= 1;
		float angle = 0;
	};

	/**
	***********************************************
	*                                             *
	*  2D Line (Raycast) Boundary Data Structure  *
	*                                             *
	***********************************************
	*/
	struct Line {
		Vector2 position;
		float length = 1;
		float angle = 0;
	};


	#define	CDT_BOUNDS(CLASS) struct CLASS##Bounds2D: CLASS, Bounds2D {}
	CDT_BOUNDS(Circle);
	CDT_BOUNDS(Box);
	CDT_BOUNDS(Ray);
	CDT_BOUNDS(Line);
	#undef	CDT_BOUNDS

	inline RayBounds2D makeRayBounds(Vector2 from, Vector2 to, float width = 1) {
		return RayBounds2D {
			from,
			from.distanceTo(to),
			width,
			$vmt angleTo(from, to)
		};
	}

	inline BoxBounds2D makeBoundsAB(Vector2 a, Vector2 b) {
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

	inline BoxBounds2D makeBounds(Vector2 position, Vector2 size) {
		size /= 2;
		return makeBoundsAB(
			position - size,
			position + size
		);
	}

	// CUTE conversions

	inline c2AABB cuteify(BoxBounds2D& b) {
		return c2AABB{c2v{b.min.x, b.min.y}, c2v{b.max.x, b.max.y}};
	}

	inline c2Circle cuteify(CircleBounds2D& b) {
		return c2Circle{c2v{b.position.x, b.position.y}, b.radius};
	}

	inline c2Capsule cuteify(RayBounds2D& b) {
		Vector2 end = VecMath::angleV2(b.angle) * b.length + b.position;
		return c2Capsule{
			c2v{b.position.x, b.position.y},
			c2v{end.x, end.y},
			b.width
		};
	}

	inline c2Ray cuteify(LineBounds2D& b) {
		Vector2 normal = VecMath::angleV2(b.angle);
		return c2Ray{
			c2v{b.position.x, b.position.y},
			c2v{normal.x, normal.y},
			b.length
		};
	}

	// Shape to Point collision detection code

	inline bool withinBounds(Vector2 point, BoxBounds2D area) {
		return (
			( area.min.x < point.x) && (point.x < area.max.x)
		) && (
			( area.min.y < point.y) && (point.y < area.max.y)
		);
	}

	inline bool withinBounds(Vector2& point, CircleBounds2D& area) {
		return point.distanceTo(area.position) < area.radius;
	}

	inline bool withinBounds(Vector2& a, RayBounds2D& b) {
		// Get distance between targets
		float distance = a.distanceTo(b.position);
		// If too distant, return
		if (distance > b.length + b.width) return false;
		// Get ray position to check
		Vector2 rayPosition = VecMath::angleV2(b.angle) * distance + b.position;
		// Check collision
		CircleBounds2D target{rayPosition, b.width};
		return withinBounds(a, target);
	}

	// Shape to Shape collision detection code

	// Circle

	inline bool withinBounds(CircleBounds2D& a, CircleBounds2D& b) {
		return a.position.distanceTo(b.position) < (a.radius + b.radius);
	}

	// Box

	bool withinBounds(BoxBounds2D& a, BoxBounds2D& b) {
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

	bool withinBounds(CircleBounds2D& a, BoxBounds2D& b) {
		return c2CircletoAABB(cuteify(a), cuteify(b));
	}

	// Ray

	inline bool withinBounds(CircleBounds2D& a, RayBounds2D& b) {
		return c2CircletoCapsule(cuteify(a), cuteify(b));
	}

	inline bool withinBounds(BoxBounds2D& a, RayBounds2D& b) {
		return c2AABBtoCapsule(cuteify(a), cuteify(b));
	}

	inline bool withinBounds(RayBounds2D& a, RayBounds2D& b) {
		return c2CapsuletoCapsule(cuteify(a), cuteify(b));
	}

	// Line

	inline bool withinBounds(BoxBounds2D& a, LineBounds2D& b, c2Raycast* result = nullptr) {
		c2Raycast r;
		return c2RaytoAABB(cuteify(b), cuteify(a), result ? result : &r);
	}

	inline bool withinBounds(CircleBounds2D& a, LineBounds2D& b, c2Raycast* result = nullptr) {
		c2Raycast r;
		return c2RaytoCircle(cuteify(b), cuteify(a), result ? result : &r);
	}

	inline bool withinBounds(RayBounds2D& a, LineBounds2D& b, c2Raycast* result = nullptr) {
		c2Raycast r;
		return c2RaytoCapsule(cuteify(b), cuteify(a), result ? result : &r);
	}

	inline bool withinBounds(LineBounds2D& a, LineBounds2D& b, c2Raycast* result = nullptr) {
		c2Raycast r;
		RayBounds2D ray = {b.position, b.length, 0, b.angle};
		return c2RaytoCapsule(cuteify(b), cuteify(ray), result ? result : &r);
	}

	// Flipped Functions

	template<CollisionType T>
	inline bool withinBounds(CircleBounds2D& a, T& b) {
		return withinBounds(b, a);
	}

	template<CollisionType T>
	inline bool withinBounds(BoxBounds2D& a, T& b) {
		return withinBounds(b, a);
	}

	template<CollisionType T>
	inline bool withinBounds(RayBounds2D& a, T& b) {
		return withinBounds(b, a);
	}

	template<CollisionType T>
	inline bool withinBounds(LineBounds2D& a, T& b, c2Raycast* result = nullptr) {
		return withinBounds(b, a, result);
	}

	// Clamping functions

	inline Vector2 getBounded(Vector2& point, BoxBounds2D& box) {
		return point.clamped(
			Vector2(box.min.x, box.min.y),
			Vector2(box.max.x, box.max.y)
		);
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
	typedef AreaCollisionData<RayBounds2D>		AreaRayData;
	typedef AreaCollisionData<LineBounds2D>		AreaLineData;

	template <CollisionType A, CollisionType B>
	bool isColliding(AreaCollisionData<A>& a, AreaCollisionData<B>& b) {
		if (!(a.enabled && b.enabled))
			return false;
		return withinBounds(a.shape, b.shape);
	}
}

#define $cdt CollisionData::

namespace EntityClass {
	namespace {
		using
		VecMath::Points2D,
		VecMath::Points3D,
		VecMath::angleTo,
		VecMath::center,
		std::unordered_map;
		using namespace CollisionData;
	}

	/// Collision Layers (used for collision detection).
	EntityGroup collisionLayers;

	/**
	*****************************
	*                           *
	*  2D Base Collision Class  *
	*                           *
	*****************************
	*/
	template<CollisionType T>
	class AreaCollision2D : public Entity2D {
	public:
		/// Constructor.
		DERIVED_CLASS(AreaCollision2D, Entity2D)

		AreaCollisionData<T> collision;

		/// Called when object is created.
		virtual void onCreate() {
			collisionLayers.addObject(this, 0);
		}

		/// Called whenever a collision with another object happens.
		virtual void onCollision(Entity* target) {}

		template <CollisionType C>
		bool colliding(AreaCollision2D<C>* target) {
			return isColliding(collision, target->collision);
		}

		template <CollisionType C>
		void checkCollision(AreaCollision2D<C>* target) {
			if (colliding(target->collision)) {
				onCollision(target);
				target->onCollision(this);
			}
		}

		/// Adds the object to the given collision layer.
		void addToCollisionLayer(size_t layer) {
			collisionLayers.addObject(this, layer);
		}

		/// Removes the object from a given collision layer.
		void removeFromCollisionLayer(size_t layer) {
			collisionLayers.removeFromGroup(this, layer);
		}

		/// Sets the object's collision layers.
		void setCollisionLayers(vector<size_t> layers) {
			// Get layers this object is in
			vector<size_t> active = getCollisionLayers();
			// If layers exist...
			if (active.size() > 0)
				// Loop through them and remove object
				for (size_t layer : active)
					collisionLayers.removeFromGroup(this, layer);
			// Loop through new layers and add object to them
			for (size_t layer : layers)
				collisionLayers.addObject(this, layer);
		}

		/// Returns the collision layers this object is in.
		vector<size_t> getCollisionLayers() {
			return collisionLayers.getGroups(this);
		}

		bool isInCollisionLayer(size_t layer) {
			return collisionLayers.isInGroup(this, layer);
		}
	};

	typedef AreaCollision2D<CircleBounds2D>	AreaCircle2D;
	typedef AreaCollision2D<BoxBounds2D>	AreaBox2D;
	typedef AreaCollision2D<RayBounds2D>	AreaRay2D;
	typedef AreaCollision2D<LineBounds2D>	AreaLine2D;
};

#endif // COLLISION_2D_3D_OBJECT_H
