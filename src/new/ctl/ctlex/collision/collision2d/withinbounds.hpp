#ifndef CTL_EX_COLLISION_COLLISION2D_WITHINBOUNDS_H
#define CTL_EX_COLLISION_COLLISION2D_WITHINBOUNDS_H

#include "bounds.hpp"

CTL_EX_NAMESPACE_BEGIN

namespace Collision::C2D {
	namespace {
		using
			Math::center,
			Math::Transform2D,
			Math::Vector2,
			Math::angleV2
		;
	}

	/// @brief Point-to-Box bounds detection.
	/// @param point Point to check.
	/// @param area Bounds to check against.
	/// @return Whether bounds overlap.
	constexpr bool withinBounds(Vector2 const& point, Box const& area) {
		Decay::AsType<Vector2 const> amin = area.min(), amax = area.max();
		return (
			( amin.x < point.x) && (point.x < amax.x)
		) && (
			( amin.y < point.y) && (point.y < amax.y)
		);
	}

	/// @brief Point-to-Circle bounds detection.
	/// @param point Point to check.
	/// @param area Bounds to check against.
	/// @return Whether bounds overlap.
	constexpr bool withinBounds(Vector2 const& point, Circle const& area) {
		// Calculate circle stretch
		float angle		= point.angleTo(area.position);
		float radius	= area.radiusAt(angle + area.rotation);
		// Check collision
		return point.distanceTo(area.position) < (radius);
	}

	/// @brief Point-to-Capsule bounds detection.
	/// @param point Point to check.
	/// @param area Bounds to check against.
	/// @return Whether bounds overlap.
	constexpr bool withinBounds(Vector2 const& a, Capsule const& area) {
		// Get distance between targets
		float distance = a.distanceTo(area.position);
		// If too distant, return
		if (distance > (area.length + area.width.max())) return false;
		// Get ray position to check
		Vector2 rayPosition = angleV2(area.rotation) * distance + area.position;
		// Check collision
		Circle target(rayPosition, area.width);
		return withinBounds(a, target);
	}

	/// @brief Point-to-Ray bounds detection.
	/// @param point Point to check.
	/// @param area Bounds to check against.
	/// @return Whether bounds overlap.
	constexpr bool withinBounds(Vector2 const& a, Ray const& b) {
		float distance = b.position.distanceTo(a);
		if (distance > b.length) return false;
		return a == b.pointAt(distance);
	}

	/// @brief Point-to-Shape bounds detection.
	/// @param point Point to check.
	/// @param area Bounds to check against.
	/// @return Whether bounds overlap.
	/// @warning Currently unimplemented.
	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Vector2 const& a, Shape const& b) {
		return false;
	}

	/// @brief Point-to-Polygon bounds detection.
	/// @param point Point to check.
	/// @param area Bounds to check against.
	/// @return Whether bounds overlap.
	/// @warning Currently unimplemented.
	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Vector2 const& a, Polygon const& b) {
		return false;
	}

	// Shape to Shape collision detection code

	// Circle
	
	/// @brief Circle-to-Circle bounds detection.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	constexpr bool withinBounds(Circle const& a, Circle const& b) {
		// Calculate circle stretches
		float angle		= a.position.angleTo(b.position);
		float radiusA	= a.radiusAt(angle + a.rotation);
		float radiusB	= b.radiusAt(angle + b.rotation);
		// Check collision
		return a.position.distanceTo(b.position) < (radiusA + radiusB);
	}

	// Box

	/// @brief Box-to-Box bounds detection.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	constexpr bool withinBounds(Box const& a, Box const& b) {
		Decay::AsType<Vector2 const>
			amin = a.min(), amax = a.max(),
			bmin = b.min(), bmax = b.max()
		;
		// Get overlap on X
		bool overlapX = (
			(bmin.x < amin.x) && (amin.x < bmax.x)
		) || (
			(bmin.x < amax.x) && (amax.x < bmax.x)
		);
		// Get overlap on Y
		bool overlapY = (
			(bmin.y < amin.y) && (amin.y < bmax.y)
		) || (
			(bmin.y < amax.y) && (amax.y < bmax.y)
		);
		// Return if both axis overlap (i.e. collision)
		return overlapX && overlapY;
	}

	/// @brief Circle-to-Box bounds detection.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	constexpr bool withinBounds(Circle const& a, Box const& b) {
		Vector2 pos = a.position.clamped(b.min(), b.max());
		return withinBounds(pos, a);
	}

	// Capsule

	/// @brief Circle-to-Capsule bounds detection.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	constexpr bool withinBounds(Circle const& a, Capsule const& b) {
		// Get distance between targets
		float distance = a.position.distanceTo(b.position);
		// If too distant, return
		if (distance > (b.length + b.width.max() + a.radius.max())) return false;
		// Get ray position to check
		Vector2 rayPosition = angleV2(b.angle) * distance + b.position;
		// Check collision
		Circle target(rayPosition, b.width, b.angle + b.rotation);
		return withinBounds(a, target);
	}

	/// @brief Box-to-Capsule bounds detection.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	constexpr bool withinBounds(Box const& a, Capsule const& b) {
		Vector2 pos = b.position.clamped(a.min(), a.max());
		return withinBounds(pos, b);
	}

	/// @brief Capsule-to-Capsule bounds detection.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	constexpr bool withinBounds(Capsule const& a, Capsule const& b) {
		// Get distance between targets
		float distance = a.position.distanceTo(b.position);
		// If too distant, return
		if (distance > (a.length + b.length + a.width.max() + b.width.max())) return false;
		// Get ray position to check
		Vector2 rayPosition = angleV2(b.angle) * distance + b.position;
		// Check collision
		Circle target(rayPosition, b.width, b.angle + b.rotation);
		return withinBounds(target, a);
	}

	// Ray

	/// @brief Box-to-Ray bounds detection.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	constexpr bool withinBounds(Box const& a, Ray const& b) {
		float distance = b.position.distanceTo(a.position);
		if (distance > a.size.max()) return false;
		return withinBounds(b.pointAt(distance), a);
	}

	/// @brief Circle-to-Ray bounds detection.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	constexpr bool withinBounds(Circle const& a, Ray const& b) {
		float distance = b.position.distanceTo(a.position);
		if (distance > (a.radius.max() + b.length)) return false;
		return withinBounds(b.pointAt(distance), a);
	}

	/// @brief Capsule-to-Ray bounds detection.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	constexpr bool withinBounds(Capsule const& a, Ray const& b) {
		float distance = b.position.distanceTo(a.position);
		if (distance > (a.length + a.width.max() + b.length)) return false;
		return withinBounds(b.pointAt(distance), a);
	}

	/// @brief Ray-to-Ray bounds detection.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	constexpr bool withinBounds(Ray const& a, Ray const& b) {
		float distance = b.position.distanceTo(a.position);
		if (distance > (a.length + b.length)) return false;
		return a.pointAt(distance) == b.pointAt(distance);
	}

	// Shape

	/// @brief Box-to-Shape bounds detection.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	/// @warning Currently unimplemented.
	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Box const& a, Shape const& b) {
		return false;
	}

	/// @brief Circle-to-Shape bounds detection.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	/// @warning Currently unimplemented.
	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Circle const& a, Shape const& b) {
		return false;
	}

	/// @brief Capsule-to-Shape bounds detection.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	/// @warning Currently unimplemented.
	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Capsule const& a, Shape const& b) {
		return false;
	}

	/// @brief Ray-to-Shape bounds detection.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	/// @warning Currently unimplemented.
	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Ray const& a, Shape const& b) {
		return false;
	}

	/// @brief Shape-to-Shape bounds detection.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	/// @warning Currently unimplemented.
	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Shape const& a, Shape const& b) {
		return false;
	}

	// Polygon

	/// @brief Box-to-Polygon bounds detection.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	/// @warning Currently unimplemented.
	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Box const& a, Polygon const& b) {
		return false;
	}

	/// @brief Circle-to-Polygon bounds detection.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	/// @warning Currently unimplemented.
	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Circle const& a, Polygon const& b) {
		return false;
	}

	/// @brief Capsule-to-Polygon bounds detection.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	/// @warning Currently unimplemented.
	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Capsule const& a, Polygon const& b) {
		return false;
	}

	/// @brief Ray-to-Polygon bounds detection.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	/// @warning Currently unimplemented.
	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Ray const& a, Polygon const& b) {
		return false;
	}

	/// @brief Shape-to-Polygon bounds detection.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	/// @warning Currently unimplemented.
	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Shape const& a, Polygon const& b) {
		return false;
	}

	/// @brief Polygon-to-Polygon bounds detection.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	/// @warning Currently unimplemented.
	[[gnu::warning("Unimplemented!")]]
	constexpr bool withinBounds(Polygon const& a, Polygon const& b) {
		return false;
	}

	// Flipped Functions

	/// @brief Point-to-Bounds bounds detection.
	/// @tparam T Bounds type.
	/// @param area Bounds to check.
	/// @param point Point to check against.
	/// @return Depending on the shape, may be potentially unimplemented.
	template<Type::Ex::Collision::C2D::Collidable T>
	constexpr bool withinBounds(T const& area, Vector2 const& point) {
		return withinBounds(point, area);
	}

	/// @brief Circle-to-Bounds bounds detection.
	/// @tparam T Bounds type.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	/// @return Depending on the shape, may be potentially unimplemented.
	template<Type::Ex::Collision::C2D::Collidable T>
	constexpr bool withinBounds(Circle const& a, T const& b) {
		return withinBounds(b, a);
	}

	/// @brief Box-to-Bounds bounds detection.
	/// @tparam T Bounds type.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	/// @return Depending on the shape, may be potentially unimplemented.
	template<Type::Ex::Collision::C2D::Collidable T>
	constexpr bool withinBounds(Box const& a, T const& b) {
		return withinBounds(b, a);
	}

	/// @brief Capsule-to-Bounds bounds detection.
	/// @tparam T Bounds type.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	/// @return Depending on the shape, may be potentially unimplemented.
	template<Type::Ex::Collision::C2D::Collidable T>
	constexpr bool withinBounds(Capsule const& a, T const& b) {
		return withinBounds(b, a);
	}

	/// @brief Ray-to-Bounds bounds detection.
	/// @tparam T Bounds type.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	/// @return Depending on the shape, may be potentially unimplemented.
	template<Type::Ex::Collision::C2D::Collidable T>
	constexpr bool withinBounds(Ray const& a, T const& b) {
		return withinBounds(b, a);
	}

	/// @brief Shape-to-Bounds bounds detection.
	/// @tparam T Bounds type.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	/// @return Depending on the shape, may be potentially unimplemented.
	template<Type::Ex::Collision::C2D::Collidable B>
	constexpr bool withinBounds(Shape const& a, B const& b) {
		return withinBounds(b, a);
	}

	/// @brief Polygon-to-Bounds bounds detection.
	/// @tparam T Bounds type.
	/// @param a Bounds to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	/// @return Depending on the shape, may be potentially unimplemented.
	template<Type::Ex::Collision::C2D::Collidable T>
	constexpr bool withinBounds(Polygon const& a, T const& b) {
		return withinBounds(b, a);
	}

	// CollisionShape
	
	/// @brief Shape-to-Bounds bounds detection.
	/// @tparam T Bounds type.
	/// @param a Shape to check.
	/// @param b Bounds to check against.
	/// @return Whether bounds overlap.
	/// @return Depending on the shape, may be potentially unimplemented.
	template<Type::Ex::Collision::C2D::Collidable T>
	constexpr bool withinBounds(CollisionShape const& a, T const& b) {
		switch (a.type()) {
			case CollisionType::CT_NULL:	return false;
			case CollisionType::CT_BOX:		return withinBounds((Box)a,		b);
			case CollisionType::CT_CIRCLE:	return withinBounds((Circle)a,	b);
			case CollisionType::CT_CAPSULE:	return withinBounds((Capsule)a,	b);
			case CollisionType::CT_RAY:		return withinBounds((Ray)a,		b);
			case CollisionType::CT_SHAPE:	return withinBounds((Shape)a,	b);
			case CollisionType::CT_POLYGON:	return withinBounds((Polygon)a,	b);
		}
	}

	/// @brief Bounds-to-Shape bounds detection.
	/// @tparam T Bounds type.
	/// @param a Bounds to check.
	/// @param b Shape to check against.
	/// @return Whether bounds overlap.
	/// @return Depending on the shape, may be potentially unimplemented.
	template<Type::Ex::Collision::C2D::Collidable T>
	constexpr bool withinBounds(T const& a, CollisionShape const& b) {
		return withinBounds(b, a);
	}

	/// @brief Shape-to-Shape bounds detection.
	/// @param a Shape to check.
	/// @param b Shape to check against.
	/// @return Whether bounds overlap.
	/// @return Depending on the shape, may be potentially unimplemented.
	constexpr bool withinBounds(CollisionShape const& a, CollisionShape const& b) {
		switch (a.type()) {
			case CollisionType::CT_NULL:	return false;
			case CollisionType::CT_BOX:		return withinBounds((Box)a,		b);
			case CollisionType::CT_CIRCLE:	return withinBounds((Circle)a,	b);
			case CollisionType::CT_CAPSULE:	return withinBounds((Capsule)a,	b);
			case CollisionType::CT_RAY:		return withinBounds((Ray)a,		b);
			case CollisionType::CT_SHAPE:	return withinBounds((Shape)a,	b);
			case CollisionType::CT_POLYGON:	return withinBounds((Polygon)a,	b);
		}
	}
}

CTL_EX_NAMESPACE_END

#endif // CTL_EX_COLLISION_COLLISION2D_WITHINBOUNDS_H
