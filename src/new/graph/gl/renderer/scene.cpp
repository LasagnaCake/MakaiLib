#include "scene.hpp"

#include "../color.hpp"

using namespace Makai::Graph;
using Scene3D::BaseType;

Scene3D::Scene3D(Scene3D& other, usize const& layer, bool const& manual = false): BaseType(layer, manual) {
	extend(other);
}

void Scene3D::extend(Scene3D& other) {
	if (!other) return;
	for(auto& [name, obj]: other.objects) {
		auto [_, nobj] = createObject(name);
		bool wasBaked = obj->baked;
		if(!wasBaked)
			obj->bake();
		nobj->extend(obj->vertices, obj->vertexCount);
		if(!wasBaked)
			obj->unbake();
		nobj->trans = obj.trans;
		nobj->material = obj.material;
		nobj->material.texture.image.makeUnique();
		nobj->material.normalMap.image.makeUnique();
		nobj->material.emission.image.makeUnique();
		nobj->material.warp.image.makeUnique();
	}
	camera	= other.camera;
	world	= other.world;
}

Scene3D::Scene3D(size_t layer, String const& path, bool manual = false): BaseScene(layer, manual) {
	extendFromSceneFile(path);
}

void Scene3D::extendFromSceneFile(String const& path) {
	extendFromDefinition(FileLoader::getJSON(path), FileSystem::getDirectoryFromPath(path));
}

void Scene3D::saveToSceneFile(
	String const& folder,
	String const& name,
	bool const& integratedObjects			= false,
	bool const& integratedObjectBinaries	= false,
	bool const& integratedObjectTextures	= false,
	bool const& pretty = false

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
	if (def.contains("number") && def["version"].is_number()) {
		// Do stuff for versions
		switch (def["version"].get<size_t>()) {
			default:
			case 0: extendFromDefinitionV0(def, sourcepath); break;
		}
	} else extendFromDefinitionV0(def, sourcepath);
}

void extendFromDefinitionV0(JSON::JSONData def, String const& sourcepath) {
	try {
		Camera::Camera3D		cam;
		Material::WorldMaterial	mat;
		// Get camera data
		{
			auto& dcam = def["camera"];
			String camType = "DEFAULT";
			if (dcam["type"].isString()) camType = dcam["type"].get<String>();
			if (camType == "DEFAULT") {
				Camera::Camera3D cam;
				cam.eye		= VecMath::fromJSONArrayV3(dcam["eye"]);
				cam.at		= VecMath::fromJSONArrayV3(dcam["at"]);
				cam.up		= VecMath::fromJSONArrayV3(dcam["up"]);
				if (dcam["relativeToEye"].isBoolean())
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
			if (dcam["ortho"].isObject()) {
				camera.ortho.strength	= dcam["ortho"]["strength"].get<float>();
				camera.ortho.origin		= VecMath::fromJSONArrayV2(dcam["ortho"]["origin"]);
				camera.ortho.size		= VecMath::fromJSONArrayV2(dcam["ortho"]["origin"]);
			}
		}
		// Get world data
		{
			auto& dmat = def["world"];
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
			} else if (def["data"].isObject()) {
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
