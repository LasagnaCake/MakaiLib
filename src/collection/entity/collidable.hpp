#ifndef COLLISION_2D_3D_OBJECT_H
#define COLLISION_2D_3D_OBJECT_H

#include "core.hpp"
#include "2d3d.hpp"

#include <unordered_map>
#include <vector>
#include "../errors.hpp"
#include "../collisions.hpp"

namespace EntityClass {
	namespace {
		using
		VecMath::Points2D,
		VecMath::Points3D,
		VecMath::angleTo,
		VecMath::center,
		std::unordered_map;
		using namespace Collision;
	}

	/// Collision Layers (used for collision detection).
	EntityGroup collisionLayers;

	#define CT2D_CASE(TYPE, RESULT) if (obj->getCoreClass() == TYPE) return CT2D_##RESULT
	CollisionType2D getCollisionType2D(Entity* const& obj) {
		using enum CollisionType2D;
		CT2D_CASE("AreaCircle2D",	CIRCLE		);
		CT2D_CASE("AreaBox2D",		BOX			);
		CT2D_CASE("AreaCapsule2D",	CAPSULE		);
		CT2D_CASE("AreaRay2D",		RAY			);
		CT2D_CASE("AreaTriangle2D",	TRIANGLE	);
		CT2D_CASE("AreaQuad2D",		QUAD		);
		CT2D_CASE("AreaShape2D",	SHAPE		);
		CT2D_CASE("AreaPolygon2D",	POLYGON		);
		return CT2D_NULL;
	}
	#undef CT2D_CASE

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
		typedef T DataType;

		/// Constructor.
		DERIVED_CLASS(AreaCollision2D, Entity2D)

		AreaCollisionData<T> collision;

		/// Called when object is created.
		virtual void onCreate() {
			collisionLayers.addObject(this, 0);
		}

		virtual ~AreaCollision2D() {
			collisionLayers.removeFromAll(this);
		}

		/// Called whenever a collision with another object happens.
		virtual void onCollision(Entity* target) {}

		template <CollisionType C>
		bool colliding(AreaCollision2D<C>* const& target) {
			return isColliding(collision, target->collision);
		}

		template <CollisionType C>
		void checkCollision(AreaCollision2D<C>* const& target) {
			if (colliding(target)) {
				onCollision(target);
				target->onCollision(this);
			}
		}

		void castAndCheck(Entity* const& target) {
			using enum CollisionType2D;
			#define CT2D_CASE(RESULT, TYPE) case CT2D_##TYPE: {RESULT* cTarget = (RESULT*)target; checkCollision(cTarget); break;}
			switch (getCollisionType2D(target)) {
				CT2D_CASE(AreaCollision2D<CircleBounds2D>,		CIRCLE		)
				CT2D_CASE(AreaCollision2D<BoxBounds2D>,			BOX			)
				CT2D_CASE(AreaCollision2D<CapsuleBounds2D>,		CAPSULE		)
				CT2D_CASE(AreaCollision2D<RayBounds2D>,			RAY			)
				CT2D_CASE(AreaCollision2D<TriangleBounds2D>,	TRIANGLE	)
				CT2D_CASE(AreaCollision2D<QuadBounds2D>,		QUAD		)
				CT2D_CASE(AreaCollision2D<ShapeBounds2D>,		SHAPE		)
				CT2D_CASE(AreaCollision2D<PolygonBounds2D>,		POLYGON		)
				default: return;
			}
			#undef CT2D_CASE
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

	typedef AreaCollision2D<CircleBounds2D>		AreaCircle2D;
	typedef AreaCollision2D<BoxBounds2D>		AreaBox2D;
	typedef AreaCollision2D<CapsuleBounds2D>	AreaCapsule2D;
	typedef AreaCollision2D<RayBounds2D>		AreaRay2D;
	typedef AreaCollision2D<TriangleBounds2D>	AreaTriangle2D;
	typedef AreaCollision2D<QuadBounds2D>		AreaQuad2D;
	typedef AreaCollision2D<ShapeBounds2D>		AreaShape2D;
	typedef AreaCollision2D<PolygonBounds2D>	AreaPolygon2D;

	void castAndCheck(Entity* a, Entity* b) {
		using enum CollisionType2D;
		#define CT2D_CASE(RESULT, TYPE) case CT2D_##TYPE: {RESULT* target = (RESULT*)a; target->castAndCheck(b); break;}
		switch (getCollisionType2D(a)) {
			CT2D_CASE(AreaCircle2D,		CIRCLE		)
			CT2D_CASE(AreaBox2D,		BOX			)
			CT2D_CASE(AreaCapsule2D,	CAPSULE		)
			CT2D_CASE(AreaRay2D,		RAY			)
			CT2D_CASE(AreaTriangle2D,	TRIANGLE	)
			CT2D_CASE(AreaQuad2D,		QUAD		)
			CT2D_CASE(AreaShape2D,		SHAPE		)
			CT2D_CASE(AreaPolygon2D,	POLYGON		)
			default: return;
		}
		#undef CT2D_CASE
	}
};

#endif // COLLISION_2D_3D_OBJECT_H
