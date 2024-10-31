#ifndef CTL_EX_COLLISION_COLLISION2D_WITHINBOUNDS_H
#define CTL_EX_COLLISION_COLLISION2D_WITHINBOUNDS_H

#include "bounds.hpp"

CTL_EX_NAMESPACE_BEGIN

namespace Collision::C2D {
	namespace {
		using
			Math::angleTo,
			Math::center,
			Math::Transform2D,
			Math::Vector2,
			Math::angleV2
		;
	}

	constexpr float getRadius(Vector2 const& radius, float const& angle) {
		return
			(CTL::Math::absin(angle) * radius.x)
		+	(CTL::Math::abcos(angle) * radius.y)
		;
	}

	constexpr bool withinBounds(Vector2 const& point, Box const& area) {
		return (
			( area.min.x < point.x) && (point.x < area.max.x)
		) && (
			( area.min.y < point.y) && (point.y < area.max.y)
		);
	}

	constexpr bool withinBounds(Vector2 const& point, Circle const& area) {
		// Calculate circle stretch
		float angle		= point.angleTo(area.position);
		float radius	= getRadius(area.radius, angle + area.rotation);
		// Check collision
		return point.distanceTo(area.position) < (radius);
	}

	constexpr bool withinBounds(Vector2 const& a, Capsule const& area) {
		// Get distance between targets
		float distance = a.distanceTo(area.position);
		// If too distant, return
		if (distance > area.length + area.width.max()) return false;
		// Get ray position to check
		Vector2 rayPosition = angleV2(area.rotation) * distance + area.position;
		// Check collision
		Circle target(rayPosition, area.width);
		return withinBounds(a, target);
	}

	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Vector2 const& a, Ray const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Vector2 const& a, Figure const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Vector2 const& a, Polygon const& b) {
		return false;
	}

	// Shape to Shape collision detection code

	// Circle

	constexpr bool withinBounds(Circle const& a, Circle const& b) {
		// Calculate circle stretches
		float angle		= a.position.angleTo(b.position);
		float radiusA	= getRadius(a.radius, angle + a.rotation);
		float radiusB	= getRadius(b.radius, angle + b.rotation);
		// Check collision
		return a.position.distanceTo(b.position) < (radiusA + radiusB);
	}

	// Box

	constexpr bool withinBounds(Box const& a, Box const& b) {
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
	constexpr bool withinBounds(Circle const& a, Box const& b) {
		return false;
	}

	// Capsule

	constexpr bool withinBounds(Circle const& a, Capsule const& b) {
		// Get distance between targets
		float distance = a.position.distanceTo(b.position);
		// If too distant, return
		if (distance > b.length + b.width.max() + a.radius.max()) return false;
		// Get ray position to check
		Vector2 rayPosition = angleV2(b.angle) * distance + b.position;
		// Check collision
		Circle target(rayPosition, b.width, b.angle + b.rotation);
		return withinBounds(a, target);
	}

	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Box const& a, Capsule const& b) {
		return false;
	}

	constexpr bool withinBounds(Capsule const& a, Capsule const& b) {
		// Get distance between targets
		float distance = a.position.distanceTo(b.position);
		// If too distant, return
		if (distance > a.length + b.length + a.width.max() + b.width.max()) return false;
		// Get ray position to check
		Vector2 rayPosition = angleV2(b.angle) * distance + b.position;
		// Check collision
		Circle target(rayPosition, b.width, b.angle + b.rotation);
		return withinBounds(target, a);
	}

	// Ray

	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Box const& a, Ray const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Circle const& a, Ray const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Capsule const& a, Ray const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Ray const& a, Ray const& b) {
		return false;
	}

	// Shape

	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Box const& a, Figure const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Circle const& a, Figure const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Capsule const& a, Figure const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Ray const& a, Figure const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Figure const& a, Figure const& b) {
		return false;
	}

	// Polygon

	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Box const& a, Polygon const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Circle const& a, Polygon const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Capsule const& a, Polygon const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Ray const& a, Polygon const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Figure const& a, Polygon const& b) {
		return false;
	}

	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Polygon const& a, Polygon const& b) {
		return false;
	}

	// Flipped Functions

	template<Ex::Type::Collision::C2D::Collidable T>
	constexpr bool withinBounds(Circle const& a, T const& b) {
		return withinBounds(b, a);
	}

	template<Ex::Type::Collision::C2D::Collidable T>
	constexpr bool withinBounds(Box const& a, T const& b) {
		return withinBounds(b, a);
	}

	template<Ex::Type::Collision::C2D::Collidable T>
	constexpr bool withinBounds(Capsule const& a, T const& b) {
		return withinBounds(b, a);
	}

	template<Ex::Type::Collision::C2D::Collidable T>
	constexpr bool withinBounds(Ray const& a, T const& b) {
		return withinBounds(b, a);
	}

	template<Ex::Type::Collision::C2D::Collidable B>
	constexpr bool withinBounds(Figure const& a, B const& b) {
		return withinBounds(b, a);
	}

	template<Ex::Type::Collision::C2D::Collidable T>
	constexpr bool withinBounds(Polygon const& a, T const& b) {
		return withinBounds(b, a);
	}

	// CollisionShape

	template<Ex::Type::Collision::C2D::Collidable T>
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

	template<Ex::Type::Collision::C2D::Collidable T>
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

CTL_EX_NAMESPACE_END

#endif // CTL_EX_COLLISION_COLLISION2D_WITHINBOUNDS_H
