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
		float radius;
	};

	struct AreaCollisionData {
		Vector2 size = Vector2(1);
		bool enabled = true;
		bool isCircle = false;
	};

	inline bool withinBounds(Vector2 point, BoxBounds2D area) {
		return (
			( area.x.min < point.x) && (point.x < area.x.max)
		) && (
			( area.y.min < point.y) && (point.y < area.y.max)
		);
	}
}

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
			// Get distance between objects
			float distance = globalPosition().distanceTo(target->globalPosition());
			// Get self's and target's global scale
			Vector2
				selfScale = globalScale(),
				targetScale = target->globalScale();
			// Calculate self's and target's true diameter
			float selfDiameter		= collision.size.x * Math::min(selfScale.x, selfScale.y);
			float targetDiameter	= target->collision.size.x * Math::min(targetScale.x, targetScale.y);
			// Return if distance between circles is below zero (collision)
			return (distance - selfDiameter - targetDiameter) < 0;
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
				BoxBounds2D self = getBoxBounds();
				float pointAngle = VecMath::angleTo(globalPosition(), target->globalPosition());
				Vector2 other = VecMath::angleV2(pointAngle) * target->globalScale() * target->collision.size.x;
				other = VecMath::rotateV2(other, target->globalRotation());
				return withinBounds(other, self);
			}
			// Get projections on X and Y axis
			BoxBounds2D
				self = getBoxBounds(),
				other = ((AreaBox2D*)target)->getBoxBounds();
			// Get overlap on X
			bool overlapX = (
				(other.x.min < self.x.min) && (self.x.min < other.x.max)
			) || (
				(other.x.min < self.x.max) && (self.x.max < other.x.max)
			);
			// Get overlap on Y
			bool overlapY = (
				(other.y.min < self.y.min) && (self.y.min < other.y.max)
			) || (
				(other.y.min < self.y.max) && (self.y.max < other.y.max)
			);
			// Return if both axis overlap (i.e. collision)
			return overlapX && overlapY;
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
