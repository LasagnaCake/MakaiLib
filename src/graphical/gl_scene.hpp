class Scene3D: public Base::Drawable {
public:
	constexpr static size_t version = 0;

	typedef Pair<String, Renderable*>		RenderableEntry;
	typedef SortedDictionary<Renderable*>	RenderableBank;

	Scene3D(size_t layer = 0, bool manual = false): Drawable(layer, manual) {}

	Scene3D(size_t layer, String const& path, bool manual = false): Scene3D(layer, manual) {
		extendFromSceneFile(path);
	}

	Scene3D(Scene3D* const& other, size_t layer, bool manual = false): Scene3D(layer, manual) {
		extend(other);
	}

	Scene3D(Scene3D& other, size_t layer, bool manual = false): Scene3D(layer, manual) {
		extend(other);
	}

	virtual ~Scene3D() {
		destroy();
	}

	VecMath::Transform3D	global;
	Camera::GimbalCamera3D	camera;
	Material::WorldMaterial	world;

	void extendFromSceneFile(String const& path) {
		extendFromDefinition(FileLoader::getJSON(path), FileSystem::getDirectoryFromPath(path));
	}

	void extend(Scene3D* const& other) {
		if (!other) return;
		for(auto& [name, obj]: other->objects) {
			auto [_, nobj] = createObject(name);
			{
				bool wasBaked = obj->baked;
				if(!wasBaked)
					obj->bake();
				nobj->extend(obj->vertices, obj->vertexCount);
				if(!wasBaked)
					obj->unbake();
				nobj->trans = obj->trans;
			}
			{
				Texture2D
					*tx = nobj->material.texture.image,
					*em = nobj->material.emission.image,
					*wp = nobj->material.warp.image
				;
				nobj->material = obj->material;
				nobj->material.texture.image	= tx;
				nobj->material.emission.image	= em;
				nobj->material.warp.image		= wp;
				if (obj->material.texture.image)
					tx->create(*obj->material.texture.image);
				if (obj->material.emission.image)
					em->create(*obj->material.emission.image);
				if (obj->material.warp.image)
					wp->create(*obj->material.warp.image);
			}
		}
		camera	= other->camera;
		world	= other->world;
	}

	void extend(Scene3D& other) {
		extend(&other);
	}

	void destroy() {
		auto objs = objects;
		for (auto& [n, o]: objs)
			delete o;
		objects.clear();
	}

	void saveToSceneFile(
		String const& folder,
		String const& name,
		bool const& integratedObjects			= false,
		bool const& integratedObjectBinaries	= false,
		bool const& integratedObjectTextures	= false,
		bool const& pretty = false

	) {
		JSONData file = getSceneDefinition(integratedObjects, integratedObjectBinaries, integratedObjectTextures);
		vector<JSONData> objpaths;
		FileSystem::makeDirectory(folder);
		for (auto& [objname, obj]: objects) {
			String folderpath	= FileSystem::concatenatePath(folder, objname);
			String objpath		= FileSystem::concatenatePath(folder, objname);
			if (!integratedObjects) {
				FileSystem::makeDirectory(folderpath);
				obj->saveToDefinitionFile(
					FileSystem::concatenatePath(folder, objname),
					objname,
					"tx",
					integratedObjectBinaries,
					integratedObjectTextures
				);
				objpaths.push_back({
					{"source", FileSystem::concatenatePath(objname, objname + ".mrod")},
					{"type", "MROD"}
				});
			} else {
				if (!integratedObjectBinaries) {
					bool wasBaked = obj->baked;
					if (!wasBaked)
						obj->bake();
					FileLoader::saveBinaryFile(FileSystem::concatenatePath(folderpath, objname + ".mesh"), obj->vertices, obj->vertexCount);
					file["mesh"]["data"] = {{"path", objname + ".mesh"}};
					if (!wasBaked)
						obj->unbake();
				}
				if (!integratedObjectTextures) {
					FileSystem::makeDirectory(FileSystem::concatenatePath(folderpath, "tx"));
					auto& mdef = file["data"][objname]["material"];
					auto& mat = obj->material;
					// Save image texture
					mdef["texture"] = Material::saveImageEffect(obj->material.texture, folderpath, "tx/texture.tga");
					mdef["texture"]["alphaClip"] = mat.texture.alphaClip;
					// Save emission texture
					mdef["emission"] = Material::saveImageEffect(obj->material.emission, folderpath, "tx/emission.tga");
					mdef["texture"]["strength"] = mat.emission.strength;
					// Save warp texture
					mdef["warp"] = Material::saveImageEffect(obj->material.warp, folderpath, "tx/warp.tga");
					mdef["warp"]["channelX"] = mat.warp.channelX;
					mdef["warp"]["channelY"] = mat.warp.channelY;
					mdef["warp"]["trans"] = {
						{"position",	{mat.warp.trans.position.x,	mat.warp.trans.position.y	}	},
						{"rotation",	mat.warp.trans.rotation										},
						{"scale",		{mat.warp.trans.scale.x,	mat.warp.trans.scale.y		}	}
					};
				}
			}
		}
		if (!objpaths.empty())
			file["data"] = {{"path", objpaths}};
		// convert to text
		auto contents = file.dump(pretty ? 1 : -1, '\t', false, JSON::error_handler_t::replace);
		// Save definition file
		FileLoader::saveTextFile(FileSystem::concatenatePath(folder, name) + ".msd", contents);
	}

	RenderableEntry createObject(String name = "") {
		#ifdef MAKAILIB_SCENE_ERROR_IF_DUPLICATE_NAME
		if (objects.contains(name))
			throw Error::InvalidValue(
				"Object of name '"+name+"' already exists!",
				__FILE__,
				toString(__LINE__),
				"createObject"
			);
		#endif // MAKAILIB_SCENE_ERROR_IF_DUPLICATE_NAME
		Renderable* r = nullptr;
		if (name.empty())	name = validateName("unnamed");
		else				name = validateName(name);
		objects[name] = (r = new Renderable(0, true));
		return RenderableEntry{name, r};
	}

	void deleteObject(Renderable* const& obj) {
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

	inline RenderableBank getObjects() {
		return objects;
	}

	String getNameOfObject(Renderable* const& obj) {
		String name = "";
		if (obj)
			for (auto& [k, v]: objects)
				if (obj == v)
					name = k;
		return name;
	}

	Renderable* getObject(String const& name) {
		if (!objects.contains(name))
			return nullptr;
		return objects[name];
	}

	inline Renderable* operator[](String const& name) {
		return getObject(name);
	}

	static bool isValidName(String const& name) {
		return regexContains(name, "([\\cA-\\cZ]|[ \\t\"\\\\/?*<>:|])");
	}

private:
	RenderableBank objects;

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
		size_t i = 0;
		while (objects.contains(newName))
			newName = name + toString(i++);
		return newName;
	}

	void extendFromDefinition(
		JSONData const& def,
		String const& sourcepath
	) {
		if (def.contains("number") && def["version"].is_number()) {
			// Do stuff for versions
			switch (def["version"].get<size_t>()) {
				default:
				case 0: extendFromDefinitionV0(def, sourcepath); break;
			}
		} else extendFromDefinitionV0(def, sourcepath);
	}

	void extendFromDefinitionV0(JSONData def, String const& sourcepath) {
		try {
			Camera::Camera3D		cam;
			Material::WorldMaterial	mat;
			// Get camera data
			{
				auto& dcam = def["camera"];
				String camType = "DEFAULT";
				if (dcam["type"].is_string()) camType = dcam["type"].get<String>();
				if (camType == "DEFAULT") {
					Camera::Camera3D cam;
					cam.eye		= VecMath::fromJSONArrayV3(dcam["eye"]);
					cam.at		= VecMath::fromJSONArrayV3(dcam["at"]);
					cam.up		= VecMath::fromJSONArrayV3(dcam["up"]);
					if (dcam["relativeToEye"].is_boolean())
						cam.relativeToEye	= dcam["relativeToEye"].get<bool>();
					camera.fromCamera3D(cam);
				} else if (camType == "GIMBAL") {
					camera.position	= VecMath::fromJSONArrayV3(dcam["position"]);
					camera.rotation	= VecMath::fromJSONArrayV3(dcam["rotation"]);
				}
				camera.aspect	= VecMath::fromJSONArrayV2(dcam["aspect"]);
				camera.fov		= dcam["fov"].get<float>();
				camera.zNear	= dcam["zNear"].get<float>();
				camera.zFar		= dcam["zFar"].get<float>();
				if (dcam["ortho"].is_object()) {
					camera.ortho.strength	= dcam["ortho"]["strength"].get<float>();
					camera.ortho.origin		= VecMath::fromJSONArrayV2(dcam["ortho"]["origin"]);
					camera.ortho.size		= VecMath::fromJSONArrayV2(dcam["ortho"]["origin"]);
				}
			}
			// Get world data
			{
				auto& dmat = def["world"];
				#define _SET_FOG_PROPERTY(FOG_TYPE)\
					if (dmat[#FOG_TYPE].is_object()) {\
						mat.FOG_TYPE.enabled	= dmat[#FOG_TYPE]["enabled"].get<bool>();\
						mat.FOG_TYPE.start		= dmat[#FOG_TYPE]["start"].get<float>();\
						mat.FOG_TYPE.stop		= dmat[#FOG_TYPE]["stop"].get<float>();\
						if (dmat[#FOG_TYPE]["color"].is_array())\
							mat.FOG_TYPE.color		= VecMath::fromJSONArrayV4(dmat[#FOG_TYPE]["color"]);\
						else if (dmat[#FOG_TYPE]["color"].is_string())\
							mat.FOG_TYPE.color = Color::fromHexCodeString(dmat[#FOG_TYPE]["color"]);\
						mat.FOG_TYPE.strength	= dmat[#FOG_TYPE]["strength"].get<float>();\
					}
				_SET_FOG_PROPERTY(nearFog)
				_SET_FOG_PROPERTY(farFog)
				#undef _SET_FOG_PROPERTY
				if (dmat["ambient"].is_object()) {
					if (dmat["ambient"]["color"].is_array())
						mat.ambient.color = VecMath::fromJSONArrayV3(dmat["ambient"]["color"]);
					else if (dmat["ambient"]["color"].is_string())
						mat.ambient.color = Color::fromHexCodeString(dmat["ambient"]["color"].get<String>()).xyz();
					mat.ambient.strength = dmat["ambient"]["strength"].get<float>();
				}
			}
			// Get objects data
			{
				if (def["path"].is_object()) {
					for(auto& obj: def["data"]["path"].get<List<JSONData>>()) {
						auto r = createObject(
							FileSystem::getFileName(obj["source"].get<String>(), true)
						).second;
						if (obj["type"].get<String>() == "MROD")
							r->extendFromDefinitionFile(obj["source"].get<String>());
						if (obj["type"].get<String>() == "MESH" || obj["type"].get<String>() == "MSBO") {
							r->extendFromBinaryFile(obj["source"].get<String>());
						}
						r->bake();
					}
				} else if (def["data"].is_array()) {
					for(auto& obj: def["data"].get<List<JSONData>>()) {
						auto r = createObject().second;
						r->extendFromDefinition(
							obj,
							FileSystem::concatenatePath(sourcepath, obj)
						);
						r->bake();
					}
				} else if (def["data"].is_object()) {
					for(auto& [name, obj]: def["data"].items()) {
						DEBUGLN("[[ ", name," ]]");
						auto r = createObject(name).second;
						r->extendFromDefinition(
							obj,
							FileSystem::concatenatePath(sourcepath, name)
						);
						r->bake();
					}
				}
			}
		} catch (JSON::exception const& e) {
			throw Error::FailedAction(
				"Failed at parsing scene file!",
				__FILE__,
				toString(__LINE__),
				"extendFromDefinition",
				e.what(),
				"Please check to see if values are correct!"
			);
		}
	}

	JSONData getSceneDefinition(
		bool const& integratedObjects			= true,
		bool const& integratedObjectBinaries	= true,
		bool const& integratedObjectTextures	= true
	) {
		JSONData def;
		def["version"] = version;
		if (integratedObjects)
			for (auto& [name, obj]: objects)
				def["data"][name] = obj->getObjectDefinition("base64", integratedObjectBinaries, integratedObjectTextures);
		Camera::Camera3D cam = camera;
		def["camera"] = {
			{"eye",		{cam.eye.x, cam.eye.y, cam.eye.z}	},
			{"at",		{cam.at.x, cam.at.y, cam.at.z}		},
			{"up",		{cam.up.x, cam.up.y, cam.up.z}		},
			{"aspect",	{cam.aspect.x, cam.aspect.y,}		},
			{"fov",		cam.fov		},
			{"zNear",	cam.zNear	},
			{"zFar",	cam.zFar	},
			{"ortho", {
				{"strength",	cam.ortho.strength							},
				{"origin",		{cam.ortho.origin.x, cam.ortho.origin.y}	},
				{"size",		{cam.ortho.size.x, cam.ortho.size.y}		}
			}},
			{"relativeToEye", cam.relativeToEye}
		};
		#define _FOG_JSON_VALUE(FOG_TYPE)\
			{#FOG_TYPE, {\
				{"enabled", world.FOG_TYPE.enabled},\
				{"color", Color::toHexCodeString(world.FOG_TYPE.color, false, true)},\
				{"start", world.FOG_TYPE.start},\
				{"stop", world.FOG_TYPE.stop},\
				{"strength", world.FOG_TYPE.strength}\
			}}
		def["world"] = {
			_FOG_JSON_VALUE(nearFog),
			_FOG_JSON_VALUE(farFog),
			{"ambient", {
				{"color", Color::toHexCodeString(world.ambient.color, true, true)},
				{"strength", world.ambient.strength}
			}}
		};
		return def;
	}

	void draw() override {
		auto lastcam = Scene::camera;
		auto lastmat = Scene::world;
		Scene::camera	= camera;
		Scene::world	= Matrix4x4(global);
		Material::setMaterial(MAIN_SHADER, world);
		for(auto& [_, obj]: objects)
			obj->render();
		Scene::camera	= lastcam;
		Scene::world	= lastmat;
	}
};
