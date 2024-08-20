#ifndef MAKAILIB_GRAPH_RENDERER_RENDERABLE_H
#define MAKAILIB_GRAPH_RENDERER_RENDERABLE_H

#include "../../file/json.hpp"
#include "drawable.hpp"
#include "material/material.hpp"

namespace Makai::Graph {
	class Renderable: public DrawableObject {
	public:
		constexpr static usize VERSION = 0;

		Renderable(size_t layer = 0, bool manual = false);

		Material::ObjectMaterial material;

		Renderable(
			List<Triangle*> triangles,
			size_t layer = 0,
			bool manual = false
		);

		Renderable(
			Vertex* vertices,
			size_t count,
			size_t layer = 0,
			bool manual = false
		);

		Renderable(
			Renderable& other,
			size_t layer = 0,
			bool manual = false
		);

		virtual ~Renderable();

		/// Creates a reference and binds it to this object.
		template<ShapeRefType T>
		[[nodiscard]]
		T* createReference() {
			constexpr size_t count = T::COUNT;
			if (locked) throw Error::InvalidAction("Renderable object is locked!");
			Triangle* tris[count] = {nullptr};
			// Create triangles
			for SRANGE(i, 0, count) {
				tris[i] = new Triangle();
				triangles.push_back(tris[i]);
			}
			// Create shape
			T* shape = new T(tris);
			// Set destructor function
			shape->onDestroy =	[this, shape](){this->removeReference<T>(shape);};
			shape->onUnbind =	[this, shape](){this->unbindReference<T>(shape);};
			// Add to reference list
			references.shape.push_back((Reference3D::Shape<0>*)shape);
			// return shape
			return shape;
		}

		/// Gets a reference bound to this object by index.
		template <ShapeRefType T>
		inline T* getReference(size_t index) {
			if (locked) throw Error::InvalidAction("Renderable object is locked!");
			return (T*)references.shape[index];
		}

		/**
		* Deletes a reference bound to this object.
		*
		* More specifically, it removes the reference
		* and the triangles associated to it.
		* It also deletes the reference.
		*/
		template <ShapeRefType T>
		void removeReference(T* ref) {
			if (!ref) return;
			if (locked) return;
			constexpr usize count = T::COUNT;
			auto tris = ref->getBoundTriangles();
			if (!triangles.empty()) std::erase_if(
				triangles,
				[=](Triangle* e){
					for SRANGE(i, 0, count)
						if (e == tris[i]) {delete tris[i]; return true;}
					return false;
				}
			);
			unbindReference<T>(ref);
		}

		/**
		* Unbinds a reference bound to this object.
		*
		* More specifically, it removes the reference,
		* but keeps the triangles associated to it.
		* It also deletes the reference.
		*/
		template <ShapeRefType T>
		void unbindReference(T* ref) {
			if (!ref) return;
			if (locked) return;
			auto& rs = references.shape;
			ERASE_IF(rs, elem == (Reference3D::Shape<0>*)ref);
			delete ref;
		}

		/// IRREVERSIBLE.
		void bakeAndLock();

		void extend(Vertex* vertices, size_t size);

		void extend(Renderable& other);

		void extend(List<Renderable*> parts);

		void extendFromBinaryFile(String const& path);

		inline void extendFromDefinitionFile(String const& path);

		void bake();

		void unbake();

		void clearData();

		void saveToBinaryFile(String const& path);

		void saveToDefinitionFile(
			String const& folder,
			String const& name = "object",
			String const& texturesFolder = "tx",
			bool const& integratedBinary	= false,
			bool const& integratedTextures	= false,
			bool const& pretty = false
		);

		List<Triangle*> triangles;

	private:
		friend class Scene3D;

		Vertex* vertices = nullptr;

		bool
			baked	= false,
			locked	= false
		;

		void copyVertices();

		void draw() override;

		/// List of references linked to this object.
		struct {
			List<ShapeRef<0>*>	shape;
		} references;

		/// The amount of vertices this object has.
		size_t vertexCount = 0;

		void extendFromDefinition(
			JSON::JSONData def,
			String const& sourcepath
		);

		void extendFromDefinitionV0(
			JSON::JSONData def,
			String const& sourcepath
		);

		JSON::JSONData getObjectDefinition(
			String const& encoding			= "base64",
			bool const& integratedBinary	= true,
			bool const& integratedTextures	= true
		);
	};
}

#endif // MAKAILIB_GRAPH_RENDERER_RENDERABLE_H
