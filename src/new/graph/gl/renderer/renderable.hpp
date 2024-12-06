#ifndef MAKAILIB_GRAPH_RENDERER_RENDERABLE_H
#define MAKAILIB_GRAPH_RENDERER_RENDERABLE_H

#include "../../../file/json.hpp"
#include "../../../data/encdec.hpp"
#include "drawable.hpp"
#include "reference.hpp"
#include "../material/material.hpp"
#include "../vertex.hpp"

namespace Makai::Graph {
	class Scene;

	class Renderable: public IGLDrawable {
	public:
		constexpr static usize VERSION = 0;

		Renderable(usize const layer = 0, bool const manual = false);

		Material::ObjectMaterial material;

		Renderable(
			List<Triangle*> const& triangles,
			usize const layer = 0,
			bool const manual = false
		);

		Renderable(
			Vertex* const vertices,
			usize const count,
			usize const layer = 0,
			bool const manual = false
		);

		Renderable(
			Renderable& other,
			usize const layer = 0,
			bool const manual = false
		);

		virtual ~Renderable();

		/// Creates a reference and binds it to this object.
		template<ShapeRefType T>
		[[nodiscard]]
		Instance<T> createReference() {
			constexpr usize count = T::SIZE;
			if (locked) throw Error::InvalidAction("Renderable object is locked!");
			List<Triangle*> tris(count);
			// Create triangles
			for (Triangle*& t: tris)
				t = new Triangle();
			triangles.appendBack(tris);
			// Create shape
			T* shape = new T(tris, *this);
			// Add to reference list
			references.pushBack(shape);
			// return shape
			return shape;
		}

		/**
		* Deletes a reference bound to this object.
		*
		* More specifically, it removes the reference
		* and the triangles associated to it.
		* It also deletes the reference.
		*/
		template <ShapeRefType T>
		void removeReference(Instance<T> const& ref) {
			if (!ref) return;
			if (locked) return;
			removeReference(*ref);
		}

		/**
		* Unbinds a reference bound to this object.
		*
		* More specifically, it removes the reference,
		* but keeps the triangles associated to it.
		* It also deletes the reference.
		*/
		template <ShapeRefType T>
		void unbindReference(Instance<T> const& ref) {
			if (!ref) return;
			if (locked) return;
			unbindReference(*ref);
			ref.destroy();
		}

		/// IRREVERSIBLE.
		void bakeAndLock();

		void extend(Vertex* const vertices, usize const size);

		void extend(Renderable& other);

		void extend(List<Renderable*> const& parts);

		void extendFromBinaryFile(String const& path);

		void extendFromDefinitionFile(String const& path);

		void bake();

		void unbake();

		void clearData();

		void saveToBinaryFile(String const& path);

		void saveToDefinitionFile(
			String const& folder,
			String const& name = "object",
			String const& texturesFolder = "tx",
			bool const integratedBinary	= false,
			bool const integratedTextures	= false,
			bool const pretty = false
		);

		List<Triangle*> triangles;

	private:
		void removeReference(IReference& ref);
		void unbindReference(IReference& ref);

		friend class Scene;
		friend class IReference;

		Vertex* vertices = nullptr;

		bool
			baked	= false,
			locked	= false
		;

		void copyVertices();

		void draw() override;

		/// List of references linked to this object.
		List<IReference*>	references;

		/// The amount of vertices this object has.
		usize vertexCount = 0;

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
			bool const integratedBinary	= true,
			bool const integratedTextures	= true
		);
	};
}

#endif // MAKAILIB_GRAPH_RENDERER_RENDERABLE_H
