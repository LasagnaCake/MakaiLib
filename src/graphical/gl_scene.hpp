class Scene3D: public Base::Drawable {
public:
	Scene3D(size_t layer = 0, bool manual = false): Drawable(layer, manual) {}

	Scene3D(string path, size_t layer = 0, bool manual = false): Scene3D(layer, manual) {
		extendFromSceneFile(path);
	}

	Scene3D(Scene3D* other, size_t layer = 0, bool manual = false): Scene3D(layer, manual) {
		extend(other);
	}

	virtual ~Scene3D() {
		destroy();
	}

	Camera::Camera3D		camera;
	Material::WorldMaterial	world;

	void extendFromSceneFile(string path) {
	}

	void extendFromDefinition(nlohmann::json def) {

	}

	void extend(Scene3D* other) {
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
					tx->create(obj->material.texture.image->getData());
				if (obj->material.emission.image)
					em->create(obj->material.emission.image->getData());
				if (obj->material.warp.image)
					wp->create(obj->material.warp.image->getData());
			}
		}
		camera	= other->camera;
		world	= other->world;
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
		nlohmann::json file = getSceneDefinition(integratedObjects, integratedObjectBinaries, integratedObjectTextures);
		vector<nlohmann::json> objpaths;
		FileSystem::makeDirectory(folder);
		for (auto [i, obj]: Helper::enumerate(objects)) {
			string objname		= "obj_" + toString(i);
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
					mdef["texture"] = obj->saveImageEffect(mat.texture, folderpath, "tx/texture.tga");
					mdef["texture"]["alphaClip"] = mat.texture.alphaClip;
					// Save emission texture
					mdef["emission"] = obj->saveImageEffect(mat.emission, folderpath, "tx/emission.tga");
					mdef["texture"]["alphaClip"] = mat.emission.alphaClip;
					// Save warp texture
					mdef["warp"] = obj->saveImageEffect(mat.warp, folderpath, "tx/warp.tga");
					mdef["warp"]["channelX"] = mat.warp.channelX;
					mdef["warp"]["channelY"] = mat.warp.channelY;
					mdef["warp"]["trans"] = {
						{"position",	{mat.warp.trans.position.x,	mat.warp.trans.position.y	}	},
						{"rotation",	mat.warp.trans.rotation								},
						{"scale",		{mat.warp.trans.scale.x,	mat.warp.trans.scale.y		}	}
					};
				}
			}
		}
		if (!objpaths.empty())
			file["data"] = {{"path", objpaths}};
		// convert to text
		auto contents = file.dump(pretty ? 1 : -1, '\t', false, nlohmann::json::error_handler_t::replace);
		// Save definition file
		FileLoader::saveTextFile(FileSystem::concatenatePath(folder, name) + ".msd", contents);
	}

	nlohmann::json getSceneDefinition(
		bool integratedObjects			= true,
		bool integratedObjectBinaries	= true,
		bool integratedObjectTextures	= true
	) {
		nlohmann::json def;
		if (integratedObjects) {
			vector<nlohmann::json> objdefs;
			for (Renderable* obj: objects)
				objdefs.push_back(obj->getObjectDefinition("base64", integratedObjectBinaries, integratedObjectTextures));
			def["data"] = objdefs;
		}
		file["camera"] = {
			{"eye",		{camera.eye.x, camera.eye.y, camera.eye.z}	},
			{"at",		{camera.at.x, camera.at.y, camera.at.z}		},
			{"up",		{camera.up.x, camera.up.y, camera.up.z}		},
			{"aspect",	{camera.aspect.x, camera.aspect.y,}			},
			{"fov",		camera.fov		},
			{"zNear",	camera.zNear	},
			{"zFar",	camera.zFar		},
			{"ortho", {
				{"enabled",	camera.otho.enabled								},
				{"origin",	{camera.ortho.origin.x, camera.ortho.origin.y}	},
				{"size",	{camera.ortho.size.x, camera.ortho.size.y}		}
			}},
			{"relativeToEye", camera.relativeToEye}
		};
		#define FOG_JSON_VALUE(FOG_TYPE)\
			{#FOG_TYPE, {\
				{"enabled", world.FOG_TYPE.enabled},\
				{"color", {\
					world.FOG_TYPE.color.x,\
					world.FOG_TYPE.color.y,\
					world.FOG_TYPE.color.z,\
					world.FOG_TYPE.color.w\
				}},\
				{"start", world.FOG_TYPE.start},\
				{"stop", world.FOG_TYPE.stop},\
				{"strength", world.FOG_TYPE.strength}\
			}}
		file["world"] = {
			FOG_JSON_VALUE(nearFog),
			FOG_JSON_VALUE(farFog),
			{"ambient", {
				{"enabled", world.ambient.enabled},
				{"color", {
					world.ambient.color.x,
					world.ambient.color.y,
					world.ambient.color.z,
					world.ambient.color.w
				}},
				{"strength", world.ambient.strength}
			}}
		}
		return def;
	}

	Renderable* createObject() {
		Renderable* r = new Renderable();
		textures.push_back(r->material.texture.image	= new Drawer::Texture2D());
		textures.push_back(r->material.emission.image	= new Drawer::Texture2D());
		textures.push_back(r->material.warp.image		= new Drawer::Texture2D());
		return r;
	}

	void deleteObject(Renderable* obj) {
		if (obj) {
			$eraseif(objects, elem == obj);
			std::erase_if(
				textures,
				[=](Texture2D* tx){
					if (tx == obj->material.texture.image) {delete tx; return true;}
					return false;
				}
			);
			std::erase_if(
				textures,
				[=](Texture2D* tx){
					if (tx == obj->material.emission.image) {delete tx; return true;}
					return false;
				}
			);
			std::erase_if(
				textures,
				[=](Texture2D* tx){
					if (tx == obj->material.warp.image) {delete tx; return true;}
					return false;
				}
			);
		}
		delete obj;
	}

	inline RenderableList getObjects() {
		return objects;
	}

private:
	RenderableList		objects;
	vector<Texture2D*>	textures;

	void draw() override {
		auto lastcam = Scene::camera;
		Scene::camera = camera;
		Material::setMaterial($mainshader, world);
		for(Renderable* obj: objects)
			obj->render();
		Scene::camera = lastcam;
	}
};
