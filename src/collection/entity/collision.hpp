#ifndef COLLISION_2D_3D_OBJECT_H
#define COLLISION_2D_3D_OBJECT_H

#include "core.hpp"
#include "2d3d.hpp"

#include <map>

// TODO: refactor collision checking code to utilize withinBounds() functions (1/2 to be made)

namespace CollisionData {
	namespace {
		using
		VecMath::Points2D,
		VecMath::Points3D,
		VecMath::angleTo,
		VecMath::center,
		std::map;
		using namespace Vector;
	}
	/**
	*******************************
	*                             *
	*  Projection Data Structure  *
	*                             *
	*******************************
	*/
	struct Projection {
		float min;
		float max;
	};

	/**
	************************************
	*                                  *
	*  2D Box Boundary Data Structure  *
	*                                  *
	************************************
	*/
	struct BoxBounds2D {
		Projection x;
		Projection y;
	};

	/**
	***************************************
	*                                     *
	*  2D Circle Boundary Data Structure  *
	*                                     *
	***************************************
	*/
	struct CircleBounds2D {
		Vector2 position;
		float radius = 1;
	};

	/**
	****************************************
	*                                      *
	*  Simple 2D Collision Data Structure  *
	*                                      *
	****************************************
	*/
	struct AreaCollisionData {
		Vector2 size = Vector2(1);
		bool enabled = true;
		bool isCircle = false;
	};

	struct RayCast2D {
		Vector2 position;
		float length = 1;
		float width	= 1;
		float angle = 0;
	};

	inline RayCast2D makeRayCast(Vector2 from, Vector2 to, float width = 1) {
		return RayCast2D {
			from,
			from.distanceTo(to),
			width,
			$vmt angleTo(from, to)
		};
	}

	inline BoxBounds2D makeBoundsAB(Vector2 a, Vector2 b) {
		return BoxBounds2D {
			Projection {
				Math::min(a.x, b.x),
				Math::max(a.x, b.x)
			},
			Projection {
				Math::min(a.y, b.y),
				Math::max(a.y, b.y)
			}
		};
	}

	inline BoxBounds2D makeBounds(Vector2 position, Vector2 size) {
		size /= 2;
		return makeBoundsAB(
			position - size,
			position + size
		);
	}

	inline bool withinBounds(Vector2 point, BoxBounds2D area) {
		return (
			( area.x.min < point.x) && (point.x < area.x.max)
		) && (
			( area.y.min < point.y) && (point.y < area.y.max)
		);
	}

	inline bool withinBounds(Vector2& point, CircleBounds2D& area) {
		return point.distanceTo(area.position) < area.radius;
	}

	inline bool withinBounds(CircleBounds2D& a, CircleBounds2D& b) {
		return a.position.distanceTo(b.position) < (a.radius + b.radius);
	}

	bool withinBounds(BoxBounds2D& a, BoxBounds2D& b) {
		// Get overlap on X
		bool overlapX = (
			(b.x.min < a.x.min) && (a.x.min < b.x.max)
		) || (
			(b.x.min < a.x.max) && (a.x.max < b.x.max)
		);
		// Get overlap on Y
		bool overlapY = (
			(b.y.min < a.y.min) && (a.y.min < b.y.max)
		) || (
			(b.y.min < a.y.max) && (a.y.max < b.y.max)
		);
		// Return if both axis overlap (i.e. collision)
		return overlapX && overlapY;
	}

	bool withinBounds(CircleBounds2D& a, BoxBounds2D& b) {
		float pointAngle = VecMath::angleTo(
			a.position,
			Vector2(
				(b.x.min + b.x.max) / 2.0,
				(b.y.min + b.y.max) / 2.0
			)
		);
		Vector2 point = VecMath::angleV2(pointAngle) * a.radius + a.position;
		return withinBounds(point, b);
	}

	inline bool withinBounds(BoxBounds2D& a, CircleBounds2D& b) {
		return withinBounds(b, a);
	}

	inline Vector2 getBounded(Vector2& point, BoxBounds2D& box) {
		return point.clamped(
			Vector2(box.x.min, box.y.min),
			Vector2(box.x.max, box.y.max)
		);
	}

	inline bool withinBounds(CircleBounds2D& a, RayCast2D& b) {
		// Get distance between targets
		float distance = a.position.distanceTo(b.position);
		// If too distant, return
		if (distance - b.length > a.radius + b.width)
			return false;
		// Get ray position to check
		Vector2 rayPosition = VecMath::angleV2(b.angle) * distance + b.position;
		// Check collision
		CircleBounds2D targetA{rayPosition, b.width}, targetB{b.position, b.width};
		return withinBounds(a, targetA) || withinBounds(a, targetB);
	}

	// This is wrong.
	inline bool withinBounds(BoxBounds2D& a, RayCast2D& b) {
		// Get ray bounding box
		BoxBounds2D boxB = makeBoundsAB(
			b.position - VecMath::angleV2(b.angle + PI) * (b.width),
			b.position + VecMath::angleV2(b.angle) * (b.length + b.width)
		);
		// Check collision
		return withinBounds(a, boxB);
	}

	inline bool withinBounds(Vector2& a, RayCast2D& b) {
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
}

#define $cdt CollisionData::

namespace EntityClass {
	namespace {
		using
		VecMath::Points2D,
		VecMath::Points3D,
		VecMath::angleTo,
		VecMath::center,
		std::map;
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
	class AreaCollision2D : public Entity2D {
	public:
		/// Constructor.
		DERIVED_CLASS(AreaCollision2D, Entity2D)

		AreaCollisionData collision;

		/// Called when object is created.
		virtual void onCreate() {
			collisionLayers.addEntity(this, 0);
		}

		/// Called whenever a collision with another object happens.
		virtual void onCollision(Entity* target) {}

		virtual bool colliding(AreaCollision2D* target) {return false;}
		virtual void checkCollision(AreaCollision2D* target) {}

		/// Adds the object to the given collision layer.
		void addToCollisionLayer(size_t layer) {
			collisionLayers.addEntity(this, layer);
		}

		/// Removes the object from a given collision layer.
		void removeFromCollisionLayer(size_t layer) {
			collisionLayers.removeEntity(this, layer);
		}

		/// Sets the object's collision layers.
		void setCollisionLayers(vector<size_t> layers) {
			// Get layers this object is in
			vector<size_t> active = getCollisionLayers();
			// If layers exist...
			if (active.size() > 0)
				// Loop through them and remove object
				for (size_t layer : active)
					collisionLayers.removeEntity(this, layer);
			// Loop through new layers and add object to them
			for (size_t layer : layers)
				collisionLayers.addEntity(this, layer);
		}

		/// Returns the collision layers this object is in.
		vector<size_t> getCollisionLayers() {
			return collisionLayers.getGroups(this);
		}

		bool isInCollisionLayer(size_t layer) {
			return collisionLayers.hasEntity(this, layer);
		}
	};

	/**
	**************************
	*                        *
	*  2D Area Circle Class  *
	*                        *
	**************************
	*/
	class AreaCircle2D : public AreaCollision2D {
	public:
		/// Constructor.
		DERIVED_CLASS(AreaCircle2D, AreaCollision2D)

		virtual void onCreate() {
			collision.isCircle = true;
		}

		/// Returns whether it is colliding with a given area circle.
		bool colliding(AreaCollision2D* target) override {
			// If either object cannot collide, return false
			if (!(collision.enabled && target->collision.enabled)) return false;
			// Check collision
			CircleBounds2D
				selfBounds = getCircleBounds(),
				targetBounds = ((AreaCircle2D*)target)->getCircleBounds();
			return withinBounds(
				selfBounds,
				targetBounds
			);
		}

		CircleBounds2D getCircleBounds() {
			Vector2 colsize = collision.size * globalScale();
			return CircleBounds2D {
				globalPosition(),
				Math::min(colsize.x, colsize.y)
			};
		}

		/// Checks and processes collision between this object and another objecr.
		void checkCollision(AreaCollision2D* target) override {
			// Get self's collision layers
			vector<size_t> self = getCollisionLayers();
			// If no layers, return
			if (!self.size()) return;
			// Check if can collide with object
			bool collidable = false;
			for (size_t layer: self)
				if (target->isInCollisionLayer(layer)) {
					collidable = true;
					break;
				}
			// If can collide and colliding, trigger collision
			if(collidable) {
				if (collision.isCircle && !target->collision.isCircle)
					target->checkCollision(this);
				else if (colliding(target))
					onCollision(target);
			}
		}

	private:
	};

	/**
	**************************
	*                        *
	*  2D Area Square Class  *
	*                        *
	**************************
	*/
	class AreaBox2D : public AreaCollision2D {
	public:
		/// Constructor.
		DERIVED_CLASS(AreaBox2D, AreaCollision2D)

		/// Returns whether it is colliding with a given hitbox.
		bool colliding(AreaCollision2D* target) override {
			// If either object cannot collide, return false
			if (!(collision.enabled && target->collision.enabled)) return false;
			if (target->collision.isCircle) {
				// Check collision
				BoxBounds2D		self = getBoxBounds();
				CircleBounds2D	other = ((AreaCircle2D*)target)->getCircleBounds();
				return withinBounds(
					other,
					self
				);
			}
			// Get projections on X and Y axis
			BoxBounds2D
				self = getBoxBounds(),
				other = ((AreaBox2D*)target)->getBoxBounds();
			// Get overlap on X
			return withinBounds(self, other);
		}

		/// Gets the boundaries of the box along the X and Y axis.
		BoxBounds2D getBoxBounds() {
			// Projection data
			BoxBounds2D bound;
			// Get global position
			Vector2 global = globalPosition();
			// Get global collision size
			Vector2 boxSize = collision.size * VecMath::rotateV2(globalScale(), globalRotation());
			// Get Projected X
			bound.x.min = global.x - (boxSize.x / 2);
			bound.x.max = global.x + (boxSize.x / 2);
			// Get Projected Y
			bound.y.min = global.y - (boxSize.y / 2);
			bound.y.max = global.y + (boxSize.y / 2);
			// Return projection
			return bound;
		}

		/// Checks and processes collision between this object and another objecr.
		void checkCollision(AreaCollision2D* target) override {
			// Get self's collision layers
			vector<size_t> self = getCollisionLayers();
			// If no layers, return
			if (!self.size()) return;
			// Check if can collide with object
			bool collidable = false;
			for (size_t layer: self)
				if (target->isInCollisionLayer(layer)) {
					collidable = true;
					break;
				}
			// If can collide and colliding, trigger collision
			if(collidable)
				if (colliding(target))
					onCollision(target);
		}
	private:
	};

	/**
	****************************
	*                          *
	*  2D Simple Hitbox Class  *
	*                          *
	****************************
	*/
	class SimpleHitbox2D : public Entity2D {
	public:
		/// Constructor.
		DERIVED_CLASS(SimpleHitbox2D, Entity2D)

		/// Returns whether it is colliding with a given hitbox.
		bool colliding(SimpleHitbox2D* target) {
			// TODO: collision (Separating Axis Theorem or some shit)
			return false;
		}

	private:
	};
};

#endif // COLLISION_2D_3D_OBJECT_H
