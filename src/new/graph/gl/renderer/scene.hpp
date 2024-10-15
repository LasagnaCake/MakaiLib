#ifndef MAKAILIB_GRAPH_RENDERER_SCENE_H
#define MAKAILIB_GRAPH_RENDERER_SCENE_H

#include "drawable.hpp"
#include "renderable.hpp"
#include "../material/material.hpp"

namespace Makai::Graph {
	template<class T>
	concept CameraType = Type::Convertible<T, Camera3D>;

	namespace Base {
		template<DrawableObjectType TDraw>
		class Collection: public Drawable {
		public:
			using DrawableType = TDraw;

			using DrawableEntry	= KeyValuePair<String, DrawableType*>;
			using DrawableBank	= SortedDictionary<DrawableType*>;

			Collection(usize const& layer = 0, bool const& manual = false): Drawable(layer, manual) {}

			virtual ~Collection() {
				destroy();
			}

			VecMath::Transform3D	space;

			void destroy() {
				auto objs = objects;
				for (auto& [n, o]: objs)
					delete o;
				objects.clear();
			}

			DrawableEntry createObject(String name = "") {
				#ifdef MAKAILIB_SCENE_ERROR_IF_DUPLICATE_NAME
				if (objects.contains(name))
					throw Error::InvalidValue(
						"Object of name '"+name+"' already exists!",
						__FILE__,
						toString(__LINE__),
						"createObject"
					);
				#endif // MAKAILIB_SCENE_ERROR_IF_DUPLICATE_NAME
				DrawableType* r = nullptr;
				if (name.empty())	name = validateName("unnamed");
				else				name = validateName(name);
				objects[name] = (r = new DrawableType(true));
				return DrawableEntry{name, r};
			}

			void deleteObject(DrawableType* const& obj) {
				if (obj)
					for (auto& [k, v]: objects)
						if (obj == v) {
							objects.erase(k);
							delete obj;
							break;
						}
			}

			void deleteObject(String const& name = "") {
				if (!objects.contains(name)) return;
				delete objects[name];
				objects.erase(name);
			}

			inline DrawableBank getObjects() {
				return objects;
			}

			String getNameOfObject(DrawableType* const& obj) {
				String name = "";
				if (obj)
					for (auto& [k, v]: objects)
						if (obj == v)
							name = k;
				return name;
			}

			DrawableType* getObject(String const& name) {
				if (!objects.contains(name))
					return nullptr;
				return objects[name];
			}

			inline DrawableType* operator[](String const& name) {
				return getObject(name);
			}

			static bool isValidName(String const& name) {
				return !regexContains(name, "([\\cA-\\cZ]|[ \\t\"\\\\/?*<>:|])");
			}

		protected:
			String validateName(String const& name) {
				if (!isValidName(name))
					throw Error::InvalidValue(
						"Name must not contain control and/or specific characters!",
						__FILE__,
						toString(__LINE__),
						"validateName",
						"Name must not contain control characters, and the following:"
						"\n- newlines, spaces or tabs"
						"\n- \\, /, ?, *, <, >, :, \" and/or |"
					);
				String newName = name;
				usize i = 0;
				while (objects.contains(newName))
					newName = name + toString(i++);
				return newName;
			}

			DrawableBank objects;
		};
	}

	class Scene: public Base::Collection<Renderable> {
	public:
		using BaseType = Base::Collection<Renderable>;

		using BaseType::Collection;

		Material::WorldMaterial	world;
		GimbalCamera3D			camera;

		constexpr static usize VERSION = 0;

		Scene(Scene& other, usize const& layer, bool const& manual = false);

		Scene(usize const& layer, String const& path, bool manual = false);

		void extendFromSceneFile(String const& path);

		void saveToSceneFile(
			String const& folder,
			String const& name,
			bool const& integratedObjects			= false,
			bool const& integratedObjectBinaries	= false,
			bool const& integratedObjectTextures	= false,
			bool const& pretty = false

		);

		void extend(Scene& other);

	private:
		void draw() override;

		void extendFromDefinition(JSON::JSONData const& def, String const& sourcepath);

		void extendFromDefinitionV0(JSON::JSONData def, String const& sourcepath);

		JSON::JSONData getSceneDefinition(
			bool const& integratedObjects			= true,
			bool const& integratedObjectBinaries	= true,
			bool const& integratedObjectTextures	= true
		);
	};
}

#endif // MAKAILIB_GRAPH_RENDERER_SCENE_H
