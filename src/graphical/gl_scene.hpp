class Scene3D: public Base::Drawable {
public:
	constexpr static String version = "0.0.0";

	Scene3D(size_t layer = 0, bool manual = false): Drawable(layer, manual) {}

	Scene3D(size_t layer, string const& path, bool manual = false): Scene3D(layer, manual) {
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

	Camera::GimbalCamera3D	camera;
	Material::WorldMaterial	world;

	void extendFromSceneFile(string const& path) {
		extendFromDefinition(FileLoader::getJSON(path), FileSystem::getDirectoryFromPath(path));
	}

	void extend(Scene3D* const& other) {
		if (!other) return;
		for(Renderable* obj: other->objects) {
			Renderable* nobj = createObject();
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
		for (Renderable* o: objs)
			delete o;
		objects.clear();
	}

	void saveToSceneFile(
		string folder,
		string name,
		bool integratedObjects			= false,
		bool integratedObjectBinaries	= false,
		bool integratedObjectTextures	= false,
		bool pretty = false

	) {
		JSONData file = getSceneDefinition(integratedObjects, integratedObjectBinaries, integratedObjectTextures);
		vector<JSONData> objpaths;
		FileSystem::makeDirectory(folder);
		for (auto [i, obj]: Helper::enumerate(objects)) {
			string objname		= toString("obj_", i);
			string folderpath	= FileSystem::concatenatePath(folder, objname);
			string objpath		= FileSystem::concatenatePath(folder, objname);
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
					auto& mdef = file["data"][i]["material"];
					auto& mat = obj->material;
					// Save image texture
					mdef["texture"] = Material::saveImageEffect(obj->material.texture, folderpath, "tx/texture.tga");
					mdef["texture"]["alphaClip"] = mat.texture.alphaClip;
					// Save emission texture
					mdef["emission"] = Material::saveImageEffect(obj->material.emission, folderpath, "tx/emission.tga");
					mdef["texture"]["alphaClip"] = mat.emission.alphaClip;
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

	Renderable* createObject() {
		Renderable* r = new Renderable(0, true);
		return r;
	}

	void deleteObject(Renderable* const& obj) {
		if (obj) {
			ERASE_IF(objects, elem == obj);
			delete obj;
		}
	}

	inline RenderableList getObjects() {
		return objects;
	}

private:
	RenderableList		objects;

	void extendFromDefinition(
		JSONData def,
		String const& sourcepath
	) {
		if (def["version"].is_number()) {
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
				cam.eye		= Vector3(dcam["eye"][0].get<float>(), dcam["eye"][1].get<float>(), dcam["eye"][2].get<float>());
				cam.at		= Vector3(dcam["at"][0].get<float>(), dcam["at"][1].get<float>(), dcam["at"][2].get<float>());
				cam.up		= Vector3(dcam["up"][0].get<float>(), dcam["up"][1].get<float>(), dcam["up"][2].get<float>());
				cam.aspect	= Vector2(dcam["aspect"][0].get<float>(), dcam["aspect"][1].get<float>());
				cam.fov		= dcam["fov"].get<float>();
				cam.zNear	= dcam["zNear"].get<float>();
				cam.zFar	= dcam["zFar"].get<float>();
				if (dcam["ortho"].is_object()) {
					cam.ortho.strength	= dcam["ortho"]["strength"].get<float>();
					cam.ortho.origin	= Vector2(dcam["ortho"]["origin"][0].get<float>(), dcam["ortho"]["origin"][1].get<float>());
					cam.ortho.size		= Vector2(dcam["ortho"]["size"][0].get<float>(), dcam["ortho"]["size"][1].get<float>());
				}
				if (dcam["relativeToEye"].is_boolean())
					cam.relativeToEye	= dcam["relativeToEye"].get<bool>();
			}
			camera.fromCamera3D(cam);
			// Get world data
			{
				auto& dmat = def["world"];
				#define _SET_FOG_PROPERTY(FOG_TYPE)\
					if (dmat[#FOG_TYPE].is_object()) {\
						mat.FOG_TYPE.enabled	= dmat[#FOG_TYPE]["enabled"].get<bool>();\
						mat.FOG_TYPE.start		= dmat[#FOG_TYPE]["start"].get<float>();\
						mat.FOG_TYPE.stop		= dmat[#FOG_TYPE]["stop"].get<float>();\
						if (dmat[#FOG_TYPE]["color"].is_array())\
							mat.FOG_TYPE.color		= Vector4(\
								dmat[#FOG_TYPE]["color"][0].get<float>(),\
								dmat[#FOG_TYPE]["color"][1].get<float>(),\
								dmat[#FOG_TYPE]["color"][2].get<float>(),\
								dmat[#FOG_TYPE]["color"][3].get<float>()\
							);\
						else if (dmat[#FOG_TYPE]["color"].is_string())\
							mat.FOG_TYPE.color = Color::fromHexCodeString(dmat[#FOG_TYPE]["color"]);\
						mat.FOG_TYPE.strength	= dmat[#FOG_TYPE]["strength"].get<float>();\
					}
				_SET_FOG_PROPERTY(nearFog)
				_SET_FOG_PROPERTY(farFog)
				#undef _SET_FOG_PROPERTY
				if (dmat["ambient"].is_object()) {
					if (dmat["ambient"]["color"].is_array())
						mat.ambient.color = Vector3(
							dmat["ambient"]["color"][0].get<float>(),
							dmat["ambient"]["color"][1].get<float>(),
							dmat["ambient"]["color"][2].get<float>()
						);
					else if (dmat["ambient"]["color"].is_string())
						mat.ambient.color = Color::fromHexCodeString(dmat["ambient"]["color"].get<String>()).xyz();
					mat.ambient.strength = dmat["ambient"]["strength"].get<float>();
				}
			}
			// Get objects data
			{
				if (def["data"].is_object()) {
					for(auto& obj: def["data"]["path"].get<List<JSONData>>()) {
						Renderable* r = createObject();
						if (obj["type"].get<string>() == "MROD")
							r->extendFromDefinitionFile(obj["source"].get<string>());
						if (obj["type"].get<string>() == "MESH" || obj["type"].get<string>() == "MSBO") {
							r->extendFromBinaryFile(obj["source"].get<string>());
						}
					}
				} else {
					for(auto& obj: def["data"].get<List<JSONData>>()) {
						createObject()->extendFromDefinition(
							obj,
							sourcepath + FileSystem::getDirectoryFromPath(obj)
						);
					}
				}
			}
		} catch (JSON::exception e) {
			throw Error::FailedAction(
				"Failed at getting image effect!",
				__FILE__,
				toString(__LINE__),
				"extendFromDefinition",
				e.what(),
				"Please check to see if values are correct!"
			);
		}
	}

	JSONData getSceneDefinition(
		bool integratedObjects			= true,
		bool integratedObjectBinaries	= true,
		bool integratedObjectTextures	= true
	) {
		JSONData def;
		def["version"] = version;
		if (integratedObjects) {
			vector<JSONData> objdefs;
			for (Renderable* obj: objects)
				objdefs.push_back(obj->getObjectDefinition("base64", integratedObjectBinaries, integratedObjectTextures));
			def["data"] = objdefs;
		}
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
		Scene::camera = camera;
		Material::setMaterial(MAIN_SHADER, world);
		for(Renderable* obj: objects)
			obj->render();
		Scene::camera = lastcam;
	}
};
