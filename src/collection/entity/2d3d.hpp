#ifndef POSITIONAL_2D_3D_OBJECT_H
#define	POSITIONAL_2D_3D_OBJECT_H

#include "core.hpp"
#include "../vectorn.hpp"

namespace EntityClass {
	namespace {
		using
		VecMath::srpTransform,
		VecMath::Transform2D,
		VecMath::Transform3D;
	}

	/**
	*********************
	*                   *
	*  2D Entity Class  *
	*                   *
	*********************
	*/
	class Entity2D: public Entity{
	public:
		/// The object's position.
		Vector2 position = Vector2();

		/// The object's rotation.
		float rotation = 0;

		/// The object's scale.
		Vector2 scale = Vector2(1.0);

		/// The object's Z Index.
		float zIndex = 0;

		/// The object's Z Scale.
		float zScale = 1;

		/// Constructor.
		ENTITY_CLASS(Entity2D, Entity)

		/// Destructor.
		virtual ~Entity2D() {
		}

		/// Calculates the object's global position.
		Vector2 globalPosition() {
			// Try and get parent
			Entity* par = getParent();
			// If unparented, or parent is root, return own position
			if (!par) return position;
			if (par->getName() == $_ROOT_NAME) return position;
			// Try getting parent's global position
			if (par->getCoreClass() == "Entity2D") {
				Entity2D* parent2D = (Entity2D*)par;
				// Transform position based on parent's transforms
				return srpTransform(
					position,
					parent2D->globalTransform()
				);
			// On fail, return own position
			} else {
				return position;
			}
		}

		/// Calculates the object's global rotation.
		float globalRotation() {
			// Try and get parent
			Entity* par = getParent();
			// If unparented, or parent is root, return own rotation
			if (!par) return rotation;
			if (par->getName() == $_ROOT_NAME) return rotation;
			// Try getting parent's global rotation
			if (par->getCoreClass() == "Entity2D") {
				Entity2D* parent2D = (Entity2D*)par;
				// Transform rotation based on parent's rotation
				return rotation + parent2D->globalRotation();
			// On fail, return own rotation
			} else {
				return rotation;
			}
		}

		/// Calculates the object's global scale.
		Vector2 globalScale() {
			// Try and get parent
			Entity* par = getParent();
			// If unparented, or parent is root, return own scale
			if (!par) return scale;
			if (par->getName() == $_ROOT_NAME) return scale;
			// Try getting parent's global scale
			if (par->getCoreClass() == "Entity2D") {
				Entity2D* parent2D = (Entity2D*)par;
				// Transform scale based on parent's scale
				return scale + parent2D->globalScale();
			// On fail, return own scale
			} else {
				return scale;
			}
		}

		/// Gets the object's local transformation data.
		inline Transform2D localTransform() {
			return Transform2D(position, rotation, scale);
		}

		/// Gets the object's global transformation data.
		inline Transform2D globalTransform() {
			return Transform2D(globalPosition(), globalRotation(), globalScale());
		}

		/// Sets the object's position, rotation and scale based on a transform.
		inline void setTransform(Transform2D trans) {
			position = trans.position;
			rotation = trans.rotation;
			scale = trans.scale;
		}
	};

	/**
	*********************
	*                   *
	*  3D Entity Class  *
	*                   *
	*********************
	*/
	class Entity3D: public Entity{
	public:
		/// The object's position.
		Vector3 position = Vector3();

		/// The object's rotation.
		Vector3 rotation = Vector3();

		/// The object's scale.
		Vector3 scale = Vector3(1.0);

		/// Constructor.
		ENTITY_CLASS(Entity3D, Entity)

		/// Destructor.
		virtual ~Entity3D() {
		}

		/// Calculates the object's global position.
		Vector3 globalPosition() {
			// Try and get parent
			Entity* par = getParent();
			// If unparented, or parent is root, return own position
			if (!par) return position;
			if (par->getName() == $_ROOT_NAME) return position;
			// Try getting parent's global position
			if (par->getCoreClass() == "Entity3D") {
				Entity3D* parent3D = (Entity3D*)par;
				// Transform position based on parent's transforms
				return srpTransform(
					position,
					parent3D->globalTransform()
				);
			// On fail, return own position
			} else {
				return position;
			}
		}

		/// Calculate's the object's global rotation.
		Vector3 globalRotation() {
			// Try and get parent
			Entity* par = getParent();
			// If unparented, or parent is root, return own rotation
			if (!par) return rotation;
			if (par->getName() == $_ROOT_NAME) return rotation;
			// Try getting parent's global rotation
			if (par->getCoreClass() == "Entity3D") {
				Entity3D* parent3D = (Entity3D*)par;
				// Transform rotation based on parent's rotation
				return rotation + parent3D->globalRotation();
			// On fail, return own rotation
			} else {
				return rotation;
			}
		}

		/// Calculates the object's global scale.
		Vector3 globalScale() {
			// Try and get parent
			Entity* par = getParent();
			// If unparented, or parent is root, return own scale
			if (!par) return scale;
			if (par->getName() == $_ROOT_NAME) return scale;
			// Try getting parent's global scale
			if (par->getCoreClass() == "Entity3D") {
				Entity3D* parent3D = (Entity3D*)par;
				// Transform scale based on parent's scale
				return scale + parent3D->globalScale();
			// On fail, return own scale
			} else {
				return scale;
			}
		}

		/// Gets the object's local transformation data.
		inline Transform3D localTransform() {
			return Transform3D(position, rotation, scale);
		}

		/// Gets the object's global transformation data.
		inline Transform3D globalTransform() {
			return Transform3D(globalPosition(), globalRotation(), globalScale());
		}

		/// Sets the object's position, rotation and scale based on a transform.
		inline void setTransform(Transform3D trans) {
			position = trans.position;
			rotation = trans.rotation;
			scale = trans.scale;
		}
	};
}

#endif // 2D_3D_POSITIONAL_OBJECT_H
