#ifndef GRAPHICAL_RENDERER_H
#define GRAPHICAL_RENDERER_H

#include <map>
#include <vector>
#include <stdexcept>
#include <string>

// #include <OGRE/Ogre.h>
#include <irrlicht.h>

#include <collection/vectorn.hpp>
#include <collection/entity/entity.hpp>

#define DERIVED_CLASS(NAME, BASE)\
	inline	virtual string getClass() {return #NAME;}\
	inline	virtual string getBaseClass() {return #BASE;}\
	inline	static string getCoreClass() {return #NAME;}

namespace VecMath {
	namespace {
		using
		Vector::Vector2,
		Vector::Vector3;

		using namespace irr;
	}

	/// Base transformation data structure.

	template <class T>
	struct Transform {
		Transform();
		Transform(T position, T rotation, T scale) {
			this->position	= position;
			this->rotation	= rotation;
			this->scale		= scale;
		}
		T position;
		T rotation;
		T scale;
	};

	typedef Transform<Vector2> Transform2D;
	typedef Transform<Vector3> Transform3D;

	/// Converts a Vector3 to an irrlicht 3D vector.
	inline core::vector3df toIrrV3(Vector3 vec) {
		return core::vector3df(vec.x, vec.y, vec.z);
	}
	/// Converts an irrlicht 3D vector to a Vector3.
	inline Vector3 toVector3(core::vector3df vec) {
		return Vector3(vec.X, vec.Y, vec.Z);
	}

	/// Converts a Vector2 to an irrlicht 2D vector.
	inline core::vector2df toIrrV2(Vector2 vec) {
		return core::vector2df(vec.x, vec.y);
	}
	/// Converts an irrlicht 2D vector to a Vector2.
	inline Vector2 toVector2(core::vector2df vec) {
		return Vector2(vec.X, vec.Y);
	}
}

namespace RenderData {
	namespace {
		using
		Vector::Vector2,
		Vector::Vector3,
		std::vector;

		using namespace irr;
	}

	class RenderObject3D {
	public:
		RenderObject3D(EntityClass::Entity3D* parent, irr::scene::ISceneManager* scene) {
			this->parent = parent;
			render.node = scene->addMeshSceneNode(render.mesh);
			this->scene = scene;
			updateTransform();
		}

		void updateTransform() {
			render.node->setPosition(VecMath::toIrrV3(parent->globalPosition()));
			render.node->setRotation(VecMath::toIrrV3(parent->globalRotation()));
			render.node->setScale(VecMath::toIrrV3(parent->globalScale()));
		}

		irr::scene::IMeshSceneNode* getSceneNode() {
			return render.node;
		}

		irr::scene::SMesh* getMesh() {
			return render.mesh;
		}

		irr::scene::ISceneManager* scene;
	private:
		struct {
			irr::scene::IMeshSceneNode* node;
			irr::scene::SMesh* mesh = new irr::scene::SMesh();
		} render;
		EntityClass::Entity3D* parent;
	};

	vector<RenderObject3D> renderList3D;
	//vector<RenderObject2D> renderList2D;

	irr::scene::ISceneManager* $_DEF_SCENE;
}

namespace EntityClass {
	namespace {
		using
		Vector::Vector2,
		Vector::Vector3;
	}

	/**
	*******************************
	*                             *
	*  2D Graphical Object Class  *
	*                             *
	*******************************
	*/
	class RenderableEntity3D: public Entity3D {
		DERIVED_CLASS(RenderableEntity3D, Entity3D)

		RenderableEntity2D(, Entity* parent, string name = "Renderable", bool uniqueEntity = true)
		: Entity3D(parent, name, uniqueEntity) {

		}

		virtual ~RenderableEntity2D() {
		}
	private:

	};
}

#undef DERIVED_CLASS

#endif // GRAPHICAL_RENDERER_H
