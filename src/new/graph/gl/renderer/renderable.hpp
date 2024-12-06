#ifndef MAKAILIB_GRAPH_RENDERER_RENDERABLE_H
#define MAKAILIB_GRAPH_RENDERER_RENDERABLE_H

#include "../../../file/json.hpp"
#include "../../../data/encdec.hpp"
#include "drawable.hpp"
#include "reference.hpp"
#include "../material/material.hpp"
#include "../vertex.hpp"

/// @brief Graphical facilities.
namespace Makai::Graph {
	class Scene;

	/// @brief Renderable object.
	class Renderable: public IGLDrawable {
	public:
		/// @brief Latest renderable object definition file version supported.
		constexpr static usize VERSION = 0;

		/// @brief Constructs the renderable object.
		/// @param layer Layer to register the object to. By default, it is layer zero.
		/// @param manual Whether the object is manually rendered. By default, it is `false`.
		Renderable(usize const layer = 0, bool const manual = false);

		/// @brief Shader material to use.
		Material::ObjectMaterial material;

		/// @brief Constructs the renderable.
		/// @param triangles Triangles to use.
		/// @param layer Layer to register the object to. By default, it is layer zero.
		/// @param manual Whether the object is manually rendered. By default, it is `false`.
		Renderable(
			List<Triangle*>&& triangles,
			usize const layer = 0,
			bool const manual = false
		);

		/// @brief Constructs the renderable.
		/// @param vertices Vertices to construct from.
		/// @param count Vertex count.
		/// @param layer Layer to register the object to. By default, it is layer zero.
		/// @param manual Whether the object is manually rendered. By default, it is `false`.
		Renderable(
			Vertex* const vertices,
			usize const count,
			usize const layer = 0,
			bool const manual = false
		);

		/// @brief Destructor.
		virtual ~Renderable();

		/// @brief Creates a shape reference bound to this object.
		/// @tparam T Reference type.
		/// @return Reference instance.
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

		/// @brief
		///		Destroys a reference and its associated triangles.
		///		Will only execute if reference is associated with this object.
		/// @tparam T Reference type.
		/// @param ref Reference to remove.
		/// @note If successful, also destroys the reference.
		template <ShapeRefType T>
		void removeReference(Instance<T> const& ref) {
			if (!ref) return;
			if (locked) return;
			removeReference(*ref);
			ref.destroy();
		}

		/// @brief
		///		Destroys a reference while keeping its associated triangles.
		///		Will only execute if reference is associated with this object.
		/// @tparam T Reference type.
		/// @param ref Reference to remove.
		/// @note If successful, also destroys the reference.
		template <ShapeRefType T>
		void unbindReference(Instance<T> const& ref) {
			if (!ref) return;
			if (locked) return;
			unbindReference(*ref);
			ref.destroy();
		}

		/// @brief IRREVERSIBLE. bakes and locks the object.
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
			String const& name				= "object",
			String const& texturesFolder	= "tx",
			bool const integratedBinary		= false,
			bool const integratedTextures	= false,
			bool const pretty				= false
		);

		List<Triangle*> triangles;

	private:
		void removeReference(IReference& ref);
		void unbindReference(IReference& ref);

		friend class Scene;
		friend class IReference;

		/// @brief Vertices used in rendering.
		Vertex* vertices = nullptr;

		/// @brief Whether the object has been baked.	
		bool baked	= false;
		/// @brief Whether the object is locked.
		bool locked	= false;

		void copyVertices();

		void draw() override;

		/// @brief References bound to this object.
		List<IReference*>	references;

		/// @brief Vertex count.
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
			bool const integratedBinary		= true,
			bool const integratedTextures	= true
		);
	};
}

#endif // MAKAILIB_GRAPH_RENDERER_RENDERABLE_H
