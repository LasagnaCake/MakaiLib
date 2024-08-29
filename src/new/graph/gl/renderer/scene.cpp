#include "scene.hpp"

#include "../color.hpp"

using namespace Makai::Graph;
using BaseType = Scene3D::BaseType;
namespace JSON = Makai::JSON;

Vector2 fromJSONArrayV2(JSON::JSONData const& json, Vector2 const& defaultValue = 0) {
	try {
		if (json.isArray())
			return Vector2(
				json[0].get<float>(),
				json[1].get<float>()
			);
		else if (json.isNumber())
			return json.get<float>();
		else return defaultValue;
	} catch (std::exception const& e) {
		return defaultValue;
	}
}

Vector3 fromJSONArrayV3(JSON::JSONData const& json, Vector3 const& defaultValue = 0) {
	try {
		if (json.isArray())
			return Vector3(
				json[0].get<float>(),
				json[1].get<float>(),
				json[2].get<float>()
			);
		else if (json.isNumber())
			return json.get<float>();
		else return defaultValue;
	} catch (std::exception const& e) {
		return defaultValue;
	}
}

Vector4 fromJSONArrayV4(JSON::JSONData const& json, Vector4 const& defaultValue = 0) {
	try {
		if (json.isArray())
			return Vector4(
				json[0].get<float>(),
				json[1].get<float>(),
				json[2].get<float>(),
				json[3].get<float>()
			);
		else if (json.isNumber())
			return json.get<float>();
		else return defaultValue;
	} catch (std::exception const& e) {
		return defaultValue;
	}
}

void Scene3D::draw() {
	GlobalState state(camera, Matrix4x4(global), world);
	for(auto& [_, obj]: objects)
		obj->render();
}

Scene3D::Scene3D(Scene3D& other, usize const& layer, bool const& manual): Collection(layer, manual) {
	extend(other);
}

void Scene3D::extend(Scene3D& other) {
	for(auto& [name, obj]: other.objects) {
		auto [_, nobj] = createObject(name);
		bool wasBaked = obj->baked;
		if(!wasBaked)
			obj->bake();
		nobj->extend(obj->vertices, obj->vertexCount);
		if(!wasBaked)
			obj->unbake();
		nobj->trans = obj->trans;
		nobj->material = obj->material;
		nobj->material.texture.image.makeUnique();
		nobj->material.normalMap.image.makeUnique();
		nobj->material.emission.image.makeUnique();
		nobj->material.warp.image.makeUnique();
	}
	camera	= other.camera;
	world	= other.world;
}

Scene3D::Scene3D(usize const& layer, String const& path, bool manual): Collection(layer, manual) {
	extendFromSceneFile(path);
}

void Scene3D::extendFromSceneFile(String const& path) {
	extendFromDefinition(File::getJSONFile(path), FileSystem::getDirectoryFromPath(path));
}

void Scene3D::saveToSceneFile(
	String const& folder,
	String const& name,
	bool const& integratedObjects,
	bool const& integratedObjectBinaries,
	bool const& integratedObjectTextures,
	bool const& pretty

) {
	JSON::JSONData file = getSceneDefinition(integratedObjects, integratedObjectBinaries, integratedObjectTextures);
	List<JSON::JSONData> objpaths;
	FileSystem::makeDirectory(folder);
	for (auto& [objname, obj]: getObjects()) {
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
			objpaths.push_back(JSON::JSONType{
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
				auto mdef = file["data"][objname]["material"];
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
				mdef["warp"]["trans"] = JSON::JSONType{
					{"position",	{mat.warp.trans.position.x,	mat.warp.trans.position.y	}	},
					{"rotation",	mat.warp.trans.rotation										},
					{"scale",		{mat.warp.trans.scale.x,	mat.warp.trans.scale.y		}	}
				};
			}
		}
	}
	if (!objpaths.empty()) {
		file["data"] = {{"path", {}}};
		usize i = 0;
		for (JSON::JSONData o: objpaths)
			file["data"]["path"][i] = o.value();
	}
	// convert to text
	auto contents = file.toString(pretty ? 1 : -1);
	// Save definition file
	FileLoader::saveTextFile(FileSystem::concatenatePath(folder, name) + ".msd", contents);
}

void Scene3D::extendFromDefinition(
	JSON::JSONData const& def,
	String const& sourcepath
) {
	if (def.has("version") && def["version"].isNumber()) {
		// Do stuff for versions
		switch (def["version"].get<usize>()) {
			default:
			case 0: extendFromDefinitionV0(def, sourcepath); break;
		}
	} else extendFromDefinitionV0(def, sourcepath);
}

void Scene3D::extendFromDefinitionV0(JSON::JSONData def, String const& sourcepath) {
	try {
		Camera3D				cam;
		Material::WorldMaterial	mat;
		// Get camera data
		{
			auto dcam = def["camera"];
			String camType = "DEFAULT";
			if (dcam["type"].isString()) camType = dcam["type"].get<String>();
			if (camType == "DEFAULT") {
				Camera3D cam;
				cam.eye		= fromJSONArrayV3(dcam["eye"]);
				cam.at		= fromJSONArrayV3(dcam["at"]);
				cam.up		= fromJSONArrayV3(dcam["up"]);
				if (dcam["relativeToEye"].isBool())
					cam.relativeToEye	= dcam["relativeToEye"].get<bool>();
				camera.fromCamera3D(cam);
			} else if (camType == "GIMBAL") {
				camera.position	= fromJSONArrayV3(dcam["position"]);
				camera.rotation	= fromJSONArrayV3(dcam["rotation"]);
			}
			camera.aspect	= fromJSONArrayV2(dcam["aspect"]);
			camera.fov		= dcam["fov"].get<float>();
			camera.zNear	= dcam["zNear"].get<float>();
			camera.zFar		= dcam["zFar"].get<float>();
			if (dcam["ortho"].isObject()) {
				camera.ortho.strength	= dcam["ortho"]["strength"].get<float>();
				camera.ortho.origin		= fromJSONArrayV2(dcam["ortho"]["origin"]);
				camera.ortho.size		= fromJSONArrayV2(dcam["ortho"]["origin"]);
			}
		}
		// Get world data
		{
			auto dmat = def["world"];
			#define _SET_FOG_PROPERTY(FOG_TYPE)\
				if (dmat[#FOG_TYPE].isObject()) {\
					mat.FOG_TYPE.enabled	= dmat[#FOG_TYPE]["enabled"].get<bool>();\
					mat.FOG_TYPE.start		= dmat[#FOG_TYPE]["start"].get<float>();\
					mat.FOG_TYPE.stop		= dmat[#FOG_TYPE]["stop"].get<float>();\
					mat.FOG_TYPE.color		= Color::fromJSON(dmat[#FOG_TYPE]["color"].get<String>());\
					mat.FOG_TYPE.strength	= dmat[#FOG_TYPE]["strength"].get<float>();\
				}
			_SET_FOG_PROPERTY(nearFog)
			_SET_FOG_PROPERTY(farFog)
			#undef _SET_FOG_PROPERTY
			if (dmat["ambient"].isObject()) {
				mat.ambient.color = Color::fromJSON(dmat["ambient"]["color"].get<String>()).xyz();
				mat.ambient.strength = dmat["ambient"]["strength"].get<float>();
			}
			world = mat;
		}
		// Get objects data
		{
			if (def["path"].isObject()) {
				for(auto& obj: def["data"]["path"].get<List<JSON::JSONType>>()) {
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
			} else if (def["data"].isArray()) {
				for(auto& obj: def["data"].get<List<JSON::JSONType>>()) {
					auto r = createObject().second;
					r->extendFromDefinition(
						obj,
						FileSystem::concatenatePath(sourcepath, obj)
					);
					r->bake();
				}
			} else if (def["data"].isObject()) {
				for(auto& [name, obj]: def["data"].json().items()) {
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
	} catch (std::exception const& e) {
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

JSON::JSONData Scene3D::getSceneDefinition(
	bool const& integratedObjects,
	bool const& integratedObjectBinaries,
	bool const& integratedObjectTextures
) {
	JSON::JSONData def;
	def["version"] = VERSION;
	if (integratedObjects)
		for (auto& [name, obj]: getObjects())
			def["data"][name] = obj->getObjectDefinition("base64", integratedObjectBinaries, integratedObjectTextures);
	Camera3D cam = camera;
	def["camera"] = JSON::JSONType{
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
	def["world"] = JSON::JSONType{
		_FOG_JSON_VALUE(nearFog),
		_FOG_JSON_VALUE(farFog),
		{"ambient", {
			{"color", Color::toHexCodeString(world.ambient.color, true, true)},
			{"strength", world.ambient.strength}
		}}
	};
	#undef _FOG_JSON_VALUE
	return def;
}
