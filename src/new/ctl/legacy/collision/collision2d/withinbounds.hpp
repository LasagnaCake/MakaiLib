#ifndef COLLISION_COLLISION2D_WITHIN_BOUNDS_H
#define COLLISION_COLLISION2D_WITHIN_BOUNDS_H

#include "bounds.hpp"

namespace Collision2D {
	namespace {
		using
			VecMath::angleTo,
			VecMath::center,
			VecMath::Transform2D
		;
	}

	constexpr float getRadius(Vector2 const& radius, float const& angle) {
		return
			(Math::absin(angle) * radius.x)
		+	(Math::abcos(angle) * radius.y)
		;
	}

	inline bool withinBounds(Vector2 const& point, Box const& area) {
		return (
			( area.min.x < point.x) && (point.x < area.max.x)
		) && (
			( area.min.y < point.y) && (point.y < area.max.y)
		);
	}

	inline bool withinBounds(Vector2 const& point, Circle const& area) {
		// Calculate circle stretch
		float angle		= point.angleTo(area.position);
		float radius	= getRadius(area.radius, angle + area.rotation);
		// Check collision
		return point.distanceTo(area.position) < (radius);
	}

	inline bool withinBounds(Vector2 const& a, Capsule const& area) {
		// Get distance between targets
		float distance = a.distanceTo(area.position);
		// If too distant, return
		if (distance > area.length + area.width.max()) return false;
		// Get ray position to check
		Vector2 rayPosition = VecMath::angleV2(area.rotation) * distance + area.position;
		// Check collision
		Circle target(rayPosition, area.width);
		return withinBounds(a, target);
	}

	[[gnu::warning("Unimplemented!")]]
	inline bool withinBounds(Vector2 const& a, Ray const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	inline bool withinBounds(Vector2 const& a, Figure const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	inline bool withinBounds(Vector2 const& a, Polygon const& b) {
		return false;
	}

	// Shape to Shape collision detection code

	// Circle

	inline bool withinBounds(Circle const& a, Circle const& b) {
		// Calculate circle stretches
		float angle		= a.position.angleTo(b.position);
		float radiusA	= getRadius(a.radius, angle + a.rotation);
		float radiusB	= getRadius(b.radius, angle + b.rotation);
		// Check collision
		return a.position.distanceTo(b.position) < (radiusA + radiusB);
	}

	// Box

	bool withinBounds(Box const& a, Box const& b) {
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

	[[gnu::warning("Unimplemented!")]]
	bool withinBounds(Circle const& a, Box const& b) {
		return false;
	}

	// Capsule

	inline bool withinBounds(Circle const& a, Capsule const& b) {
		// Get distance between targets
		float distance = a.position.distanceTo(b.position);
		// If too distant, return
		if (distance > b.length + b.width.max() + a.radius.max()) return false;
		// Get ray position to check
		Vector2 rayPosition = VecMath::angleV2(b.angle) * distance + b.position;
		// Check collision
		Circle target(rayPosition, b.width, b.angle + b.rotation);
		return withinBounds(a, target);
	}

	[[gnu::warning("Unimplemented!")]]
	inline bool withinBounds(Box const& a, Capsule const& b) {
		return false;
	}

	inline bool withinBounds(Capsule const& a, Capsule const& b) {
		// Get distance between targets
		float distance = a.position.distanceTo(b.position);
		// If too distant, return
		if (distance > a.length + b.length + a.width.max() + b.width.max()) return false;
		// Get ray position to check
		Vector2 rayPosition = VecMath::angleV2(b.angle) * distance + b.position;
		// Check collision
		Circle target(rayPosition, b.width, b.angle + b.rotation);
		return withinBounds(target, a);
	}

	// Ray

	[[gnu::warning("Unimplemented!")]]
	inline bool withinBounds(Box const& a, Ray const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	inline bool withinBounds(Circle const& a, Ray const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	inline bool withinBounds(Capsule const& a, Ray const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	inline bool withinBounds(Ray const& a, Ray const& b) {
		return false;
	}

	// Shape

	[[gnu::warning("Unimplemented!")]]
	inline bool withinBounds(Box const& a, Figure const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	inline bool withinBounds(Circle const& a, Figure const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	inline bool withinBounds(Capsule const& a, Figure const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	inline bool withinBounds(Ray const& a, Figure const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	inline bool withinBounds(Figure const& a, Figure const& b) {
		return false;
	}

	// Polygon

	[[gnu::warning("Unimplemented!")]]
	inline bool withinBounds(Box const& a, Polygon const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	inline bool withinBounds(Circle const& a, Polygon const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	inline bool withinBounds(Capsule const& a, Polygon const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	inline bool withinBounds(Ray const& a, Polygon const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	inline bool withinBounds(Figure const& a, Polygon const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	inline bool withinBounds(Polygon const& a, Polygon const& b) {
		return false;
	}

	// Flipped Functions

	template<Collidable T>
	inline bool withinBounds(Circle const& a, T const& b) {
		return withinBounds(b, a);
	}

	template<Collidable T>
	inline bool withinBounds(Box const& a, T const& b) {
		return withinBounds(b, a);
	}

	template<Collidable T>
	inline bool withinBounds(Capsule const& a, T const& b) {
		return withinBounds(b, a);
	}

	template<Collidable T>
	inline bool withinBounds(Ray const& a, T const& b) {
		return withinBounds(b, a);
	}

	template<Collidable B>
	inline bool withinBounds(Figure const& a, B const& b) {
		return withinBounds(b, a);
	}

	template<Collidable T>
	inline bool withinBounds(Polygon const& a, T const& b) {
		return withinBounds(b, a);
	}

	// CollisionShape

	template<Collidable T>
	constexpr bool withinBounds(CollisionShape const& a, T const& b) {
		switch (a.type()) {
			case CollisionType::CT_NULL:	return false;
			case CollisionType::CT_BOX:		return withinBounds((Box)a,		b);
			case CollisionType::CT_CIRCLE:	return withinBounds((Circle)a,	b);
			case CollisionType::CT_CAPSULE:	return withinBounds((Capsule)a,	b);
			case CollisionType::CT_RAY:		return withinBounds((Ray)a,		b);
			case CollisionType::CT_SHAPE:	return withinBounds((Figure)a,	b);
			case CollisionType::CT_POLYGON:	return withinBounds((Polygon)a,	b);
		}
	}

	template<Collidable T>
	constexpr bool withinBounds(T const& a, CollisionShape const& b) {
		return withinBounds(b, a);
	}

	constexpr bool withinBounds(CollisionShape const& a, CollisionShape const& b) {
		switch (a.type()) {
			case CollisionType::CT_NULL:	return false;
			case CollisionType::CT_BOX:		return withinBounds((Box)a,		b);
			case CollisionType::CT_CIRCLE:	return withinBounds((Circle)a,	b);
			case CollisionType::CT_CAPSULE:	return withinBounds((Capsule)a,	b);
			case CollisionType::CT_RAY:		return withinBounds((Ray)a,		b);
			case CollisionType::CT_SHAPE:	return withinBounds((Figure)a,	b);
			case CollisionType::CT_POLYGON:	return withinBounds((Polygon)a,	b);
		}
	}
}

#endif // COLLISION_COLLISION2D_WITHIN_BOUNDS_H
