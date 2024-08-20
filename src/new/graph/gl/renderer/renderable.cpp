#include "renderable.hpp"

using namespace Makai::Graph;

using namespace Material;

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

ObjectMaterial fromDefinition(JSON::JSONData def, String const& definitionFolder) {
	ObjectMaterial mat;
	Texture2D& texture		= mat.texture.image;
	Texture2D& normalMap	= mat.normalMap.image;
	Texture2D& emission		= mat.emission.image;
	Texture2D& warp			= mat.warp.image;
	try {
		auto& dmat = def;
		// Set color
		mat.color = Drawer::colorFromJSON(dmat["color"]);
		// Set color & shading params
		#define _SET_BOOL_PARAM(PARAM) if(dmat[#PARAM].isBoolean()) mat.PARAM = dmat[#PARAM].get<bool>()
		_SET_BOOL_PARAM(shaded);
		_SET_BOOL_PARAM(illuminated);
		#undef _SET_BOOL_PARAM
		#define _SET_FLOAT_PARAM(PARAM) if(dmat[#PARAM].isNumber()) mat.PARAM = dmat[#PARAM].get<float>()
		_SET_FLOAT_PARAM(hue);
		_SET_FLOAT_PARAM(saturation);
		_SET_FLOAT_PARAM(luminosity);
		_SET_FLOAT_PARAM(brightness);
		_SET_FLOAT_PARAM(contrast);
		#undef _SET_FLOAT_PARAM
		// Set UV shift
		if(dmat["uvShift"].isArray()) {
			mat.uvShift.x = dmat["uvShift"][0].get<float>();
			mat.uvShift.y = dmat["uvShift"][1].get<float>();
		}
		// Set texture
		if (dmat["texture"].isObject()) {
			auto fx = loadImageEffect(dmat["texture"], definitionFolder, texture);
			mat.texture.enabled	= fx.enabled;
			mat.texture.image	= fx.image;
			if (dmat["texture"]["alphaClip"].isNumber())
				mat.texture.alphaClip	= dmat["texture"]["alphaClip"].get<float>();
		}
		// Set normal map texture
		if (dmat["normalMap"].isObject()) {
			auto fx = loadImageEffect(dmat["normalMap"], definitionFolder, normalMap);
			mat.normalMap.enabled	= fx.enabled;
			mat.normalMap.image		= fx.image;
			if (dmat["normalMap"]["strength"].isNumber())
				mat.normalMap.strength	= dmat["normalMap"]["strength"].get<float>();
		}
		// Set emission texture
		if (dmat["emission"].isObject()) {
			auto fx = loadImageEffect(dmat["emission"], definitionFolder, emission);
			mat.emission.enabled	= fx.enabled;
			mat.emission.image		= fx.image;
			if (dmat["emission"]["strength"].isNumber())
				mat.emission.strength	= dmat["emission"]["strength"].get<float>();
		}
		// Set warp texture
		if (dmat["warp"].isObject()) {
			auto fx = loadImageEffect(dmat["warp"], definitionFolder, warp);
			mat.warp.enabled	= fx.enabled;
			mat.warp.image		= fx.image;
			{
				auto mwtrans = dmat["warp"]["trans"];
				mat.warp.trans.position = VecMath::fromJSONArrayV2(mwtrans["position"]);
				mat.warp.trans.rotation = mwtrans["rotation"].get<float>();
				mat.warp.trans.scale = VecMath::fromJSONArrayV2(mwtrans["scale"], 1);
			}
			mat.warp.channelX = dmat["warp"]["channelX"];
			mat.warp.channelY = dmat["warp"]["channelY"];
		}
		// Set negative
		if (dmat["negative"].isObject()) {
			mat.negative.enabled	= dmat["negative"]["enabled"].get<bool>();
			mat.negative.strength	= dmat["negative"]["strength"].get<float>();
		}
		// Set gradient
		if (dmat["gradient"].isObject()) {
			mat.gradient.enabled	= dmat["gradient"]["enabled"].get<bool>();
			mat.gradient.channel	= dmat["gradient"]["channel"].get<unsigned int>();
			auto dgbegin	= dmat["gradient"]["begin"];
			auto dgend		= dmat["gradient"]["end"];
			mat.gradient.begin	= Color::fromJSON(dgbegin);
			mat.gradient.end	= Color::fromJSON(dgend);
			mat.gradient.invert	= dmat["gradient"]["invert"].get<bool>();
		}
		// Set instances
		if (dmat["instances"].isArray()) {
			mat.instances.clear();
			for(auto& inst: dmat["instances"])
				mat.instances.push_back(VecMath::fromJSONArrayV3(inst));
		}
		// Set culling, fill & view
		if (dmat["culling"].isNumber())	mat.culling	= (ObjectCulling)dmat["culling"].get<unsigned int>();
		if (dmat["fill"].isNumber())	mat.fill	= (ObjectFill)dmat["fill"].get<unsigned int>();
		if (dmat["debug"].isNumber())	mat.debug	= (ObjectDebugView)dmat["debug"].get<unsigned int>();
	} catch (std::exception const& e) {
		throw Error::FailedAction(
			"Failed at getting material values!",
			__FILE__,
			toString(__LINE__),
			"extendFromDefinition",
			e.what(),
			"Please check to see if values are correct!"
		);
	}
	return mat;
}

JSON::JSONData toDefinition(
	ObjectMaterial& mat,
	String const& definitionFolder,
	String const& texturesFolder,
	bool integratedTextures = false
) {
	JSON::JSONData def;
	// Define object
	def = {
		{"color", Color::toHexCodeString(mat.color, false, true)},
		{"shaded", mat.shaded},
		{"illuminated", mat.illuminated},
		{"hue", mat.hue},
		{"saturation", mat.saturation},
		{"luminosity", mat.luminosity},
		{"brightness", mat.brightness},
		{"contrast", mat.contrast},
		{"uvShift", {mat.uvShift.x, mat.uvShift.y}},
		{"negative", {
			{"enabled", mat.negative.enabled},
			{"strength", mat.negative.strength}
		}},
		{"gradient", {
			{"enabled", mat.gradient.enabled},
			{"channel", mat.gradient.channel},
			{"begin", Color::toHexCodeString(mat.gradient.begin, false, true)},
			{"end", Color::toHexCodeString(mat.gradient.end, false, true)},
			{"invert", mat.gradient.invert}
		}},
		{"debugView", (uint)mat.debug}
	};
	// Copy instances
	def["instances"] = JSON::array();
	usize idx = 0;
	for (Vector3& inst: mat.instances) {
		def["instances"][idx] = {inst.x, inst.y, inst.z};
	}
	// Set cull & fill
	def["material"]["fill"]		= (uint)(mat.fill);
	def["material"]["culling"]	= (uint)(mat.culling);
	// Save image texture
	if (!integratedTextures) {
		def["warp"]		= saveImageEffect(mat.warp, definitionFolder, texturesFolder + "/warp.tga");
		def["texture"]	= saveImageEffect(mat.texture, definitionFolder, texturesFolder + "/texture.tga");
		def["emission"]	= saveImageEffect(mat.emission, definitionFolder, texturesFolder + "/emission.tga");
	} else {
		// TODO: integrated textures
	}
	// Set image parameters
	def["texture"]["alphaClip"] = mat.texture.alphaClip;
	def["emission"]["strength"] = mat.emission.strength;
	def["warp"]["channelX"] = mat.warp.channelX;
	def["warp"]["channelY"] = mat.warp.channelY;
	def["warp"]["trans"] = {
		{"position",	{mat.warp.trans.position.x,	mat.warp.trans.position.y	}	},
		{"rotation",	mat.warp.trans.rotation										},
		{"scale",		{mat.warp.trans.scale.x,	mat.warp.trans.scale.y		}	}
	};
	// Return definition
	return def;
}

Renderable::Renderable(size_t layer = 0, bool manual = false):
DrawableObject(layer, manual) {
	material.texture.image	= &texture;
	material.emission.image	= &emission;
	material.warp.image		= &warp;
}

Renderable::Renderable(
	List<Triangle*> triangles,
	size_t layer = 0,
	bool manual = false
): DrawableObject(layer, manual) {
	this->triangles = triangles;
}

Renderable::Renderable(
	Vertex* vertices,
	size_t count,
	size_t layer = 0,
	bool manual = false
): DrawableObject(layer, manual) {
	extend(vertices, count);
}

Renderable::Renderable(
	Renderable& other,
	size_t layer = 0,
	bool manual = false
): DrawableObject(layer, manual) {
	extend(other);
}

virtual Renderable::~Renderable() {
	locked = false;
	DEBUGLN("Renderable!");
	DEBUGLN("Deleting references...");
	clearData();
	DEBUGLN("Killing renderable object...");
}

void Renderable::bakeAndLock() {
	if (locked) return;
	bake();
	locked = true;
	clearData();
}

void Renderable::extend(Vertex* vertices, size_t size) {
	if (locked) return;
	if (vertices == nullptr || size == 0)
		throw Error::InvalidValue("No vertices were provided!");
	if (size % 3 != 0)
		throw Error::InvalidValue("Vertex amount is not a multiple of 3!");
	const size_t arrEnd = triangles.size();
	triangles.resize(triangles.size() + (size / 3));
	if (this->vertices)
		delete[] this->vertices;
	for RANGE(i, 0, size, 3) {
		triangles[arrEnd + (i / 3)] = (
			new Triangle{
				vertices[i],
				vertices[i+1],
				vertices[i+2]
			}
		);
	}
}

void Renderable::extend(Renderable& other) {
	if (locked) return;
	if (!other.baked) {
		other.bake();
		extend(other.vertices, other.vertexCount);
		other.unbake();
	} else extend(other.vertices, other.vertexCount);
}

void Renderable::extend(List<Renderable*> const& parts) {
	for (Renderable* p: parts)
		extend(*p);
}

void Renderable::extendFromBinaryFile(String const& path) {
	auto data = FileLoader::getBinaryFile(path);
	if (!data.size()) throw FileLoader::FileLoadError("File does not exist or is empty! (" + path + ")!");
	extend((RawVertex*)&data[0], data.size() / sizeof(Vertex));
	DEBUG("Vertices: ");
	DEBUGLN(data.size() / sizeof(Vertex));
}

inline void Renderable::extendFromDefinitionFile(String const& path) {
	extendFromDefinition(FileLoader::getJSON(path), FileSystem::getDirectoryFromPath(path));
}

void Renderable::bake() {
	if (baked || locked) return;
	baked = true;
	//Bake vertices
	copyVertices();
}

void Renderable::unbake() {
	if (!baked || locked) return;
	baked = false;
	// Clear vertex buffer
	delete[] vertices;
	vertices = nullptr;
}

void Renderable::clearData() {
	if (vertices && !locked)
		delete [] vertices;
	if (!references.shape.empty())
		for (auto sr: references.shape)
			delete sr;
	references.shape.clear();
	if (!triangles.empty())
		for (auto t: triangles)
			delete t;
	triangles.clear();
}

void Renderable::saveToBinaryFile(String const& path) {
	bake();
	FileLoader::saveBinaryFile(path, vertices, vertexCount);
}

void Renderable::saveToDefinitionFile(
	String const& folder,
	String const& name = "object",
	String const& texturesFolder = "tx",
	bool const& integratedBinary	= false,
	bool const& integratedTextures	= false,
	bool const& pretty = false
) {
	DEBUGLN("Saving object '" + name + "'...");
	// Get paths
	String binpath		= folder + "/" + name + ".mesh";
	DEBUGLN(binpath);
	DEBUGLN(folder + "/" + name + ".mrod");
	FileSystem::makeDirectory(FileSystem::concatenatePath(folder, texturesFolder));
	// Get object definition
	JSON::JSONData file = getObjectDefinition("base64", integratedBinary, integratedTextures);
	// If binary is in a different location, save there
	if (!integratedBinary) {
		FileLoader::saveBinaryFile(binpath, vertices, vertexCount);
		file["mesh"]["data"] = {{"path", name + ".mesh"}};
	}
	// Get material data
	file["material"] = toDefinition(material, folder, texturesFolder, integratedTextures);
	// convert to text
	auto contents = file.dump(pretty ? 1 : -1, '\t', false, JSON::error_handler_t::replace);
	// Save definition file
	FileLoader::saveTextFile(folder + "/" + name + ".mrod", contents);
}

void Renderable::copyVertices() {
	// If no triangles exist, return
	if (!triangles.size()) return;
	// Transform references (if applicable)
	for (auto& shape: references.shape)	shape->transform();
	// Copy data to vertex buffer
	// Get vertex count
	vertexCount = triangles.size() * 3;
	// Copy data to vertex buffer
	if (vertices) delete[] vertices;
	vertices = new RawVertex[(vertexCount)];
	// Copy data to IVB
	size_t i = 0;
	for (auto& t: triangles) {
		// Check if not null
		if (!t) continue;
		// Oh, hey, C! haven't seen you in a while!
		*(Triangle*)&vertices[i] = (*t);
		i += 3;
	}
	// De-transform references (if applicable)
	for (auto& shape: references.shape)	shape->reset();
}

void Renderable::draw() override {
	// If object's vertices are not "baked" (i.e. finalized), copy them
	if (!baked && !locked) copyVertices();
	// If no vertices, return
	if (!vertices) return;
	// Set shader data
	prepare();
	material.use(shader);
	// Present to screen
	display(vertices, vertexCount);
}

void Renderable::extendFromDefinition(
	JSON::JSONData def,
	String const& sourcepath
) {
	if (def.contains("number") && def["version"].isNumber()) {
		// Do stuff for versions
		switch (def["version"].get<size_t>()) {
			default:
			case 0: extendFromDefinitionV0(def, sourcepath); break;
		}
	} else extendFromDefinitionV0(def, sourcepath);
}

void Renderable::extendFromDefinitionV0(
	JSON::JSONData def,
	String const& sourcepath
) {
	// Component data
	String componentData;
	// Vertex data
	List<ubyte> vdata;
	// Get mesh data
	try {
		auto& mesh	= def["mesh"];
		auto& data	= mesh["data"];
		if (data.is_String()) {
			String encoding	= mesh["encoding"].get<String>();
			vdata			= decodeData(data.get<String>(), encoding);
		} else if (data.isObject()) {
			vdata			= FileLoader::getBinaryFile(FileSystem::concatenatePath(sourcepath, data["path"].get<String>()));
		}
		componentData		= mesh["components"].get<String>();
	} catch (std::exception const& e) {
		throw Error::FailedAction(
			"Failed at getting mesh values!",
			__FILE__,
			toString(__LINE__),
			"extendFromDefinition",
			e.what(),
			"Please check to see if values are correct!"
		);
	}
	DEBUGLN(componentData);
	// Check if important data is not empty
	{
		String error = "";
		if (vdata.empty())			error += ("Missing mesh's vertex data!\n");
		if (componentData.empty())	error += ("Missing mesh's component data!\n");
		if (!error.empty()) throw Error::InvalidValue(
			"Missing mesh data!\n\n" + error,
			__FILE__,
			toString(__LINE__),
			"extendFromDefinition"
		);
	}
	// Vertex map
	Drawer::VertexMap vm;
	// Component list in order they appear
	List<String> components = Helper::splitString(componentData, ',');
	// Check if valid component data
	{
		String indexes = "";
		size_t i = 0;
		for (auto& c: components) {
			if(c.empty()) indexes += toString(i, " ");
			i++;
		}
		if (!indexes.empty()) {
			throw Error::InvalidValue(
				"Malformed component data!\n\nIndex(es): [ " + indexes + "]",
				__FILE__,
				toString(__LINE__),
				"extendFromDefinition"
			);
		}
	}
	// Check if there are no missing vertices
	{
		const size_t vsize = (sizeof(float) * components.size());
		const size_t vds = (vdata.size() / vsize);
		const size_t expected = Math::ceil(vds / 3.0) * 3.0;
		if (vds % 3 != 0)
			throw Error::InvalidValue(
				"Improper/incomplete vertex data!",
				__FILE__,
				toString(__LINE__),
				"extendFromDefinition",
				toString(
					"Vertex data size is ",
					vds, " (", vdata.size(), " bytes).\nExpected size is ",
					expected, " (", expected * vsize, " bytes)."
				),
				"You either have extra data, or missing data."
			);
	}
	// Get pointer to data
	float* rawdata = (float*)vdata.data();
	// Current vertex component being accessed
	size_t component = 0;
	// Resulting vertices
	List<RawVertex> vertices;
	// Loop time
	while (component < vdata.size() / sizeof(float)) {
		vm = Drawer::baseVertexMap;
		for (auto& c: components)
			vm[c] = rawdata[component++];
		vertices.push_back(Drawer::toRawVertex(vm));
	}
	// Check if data is OK
	if (vertices.size() % 3 != 0)
		throw Error::InvalidValue(
			"Improper/incomplete vertex data!",
			__FILE__,
			toString(__LINE__),
			"extendFromDefinition",
			(
				"Total vertex count is "
			+	std::to_String(vertices.size())
			+	" .\nExpected size is "
			+	std::to_String(size_t(Math::ceil(vertices.size() / 3.0) * 3.0))
			+	"."
			),
			"You either have extra data, or missing data."
		);
	// Create renderable object
	extend(vertices.data(), vertices.size());
	// check for optional transform
	if (def["trans"].isObject()) {
		auto& dtrans = def["trans"];
		try {
			trans.position	= fromJSONArrayV3(dtrans["position"]);
			trans.rotation	= fromJSONArrayV3(dtrans["rotation"]);
			trans.scale		= fromJSONArrayV3(dtrans["scale"], 1);
		} catch (std::exception const& e) {
			throw Error::FailedAction(
				"Failed at getting transformation values!",
				__FILE__,
				toString(__LINE__),
				"extendFromDefinition",
				e.what(),
				"Please check to see if values are correct!"
			);
		}
	}
	// Set material data
	if (def["material"].isObject()) {
		material = fromDefinition(def["material"], sourcepath);
	}
	// Set blend data
	if (def["blend"].isObject()) {
		try {
			JSON::JSONData bfun	= def["blend"]["function"];
			JSON::JSONData beq	= def["blend"]["equation"];
			if (bfun.isNumber()) {
				GLenum bv = getBlendFunction(bfun.get<uint>());
				blend.func = {bv, bv, bv, bv};
			} else if (bfun.isObject()) {
				if (bfun["src"].isNumber()) {
					blend.func.srcColor = blend.func.srcAlpha = (BlendFunction)bfun["src"].get<uint>();
				} else {
					blend.func.srcColor = (BlendFunction)bfun["srcColor"].get<uint>();
					blend.func.srcAlpha = (BlendFunction)bfun["srcAlpha"].get<uint>();
				}
				if (bfun["dst"].isNumber()) {
					blend.func.dstColor = blend.func.dstAlpha = (BlendFunction)bfun["dst"].get<uint>();
				} else {
					blend.func.dstColor = (BlendFunction)bfun["dstColor"].get<uint>();
					blend.func.dstAlpha = (BlendFunction)bfun["dstAlpha"].get<uint>();
				}
			}
			if (beq.isNumber()) {
				BlendEquation bv = (BlendEquation)beq.get<uint>();
				blend.eq = {bv, bv};
			} else if (beq.isObject()) {
					blend.eq.color = (BlendEquation)beq["color"].get<uint>();
					blend.eq.alpha = (BlendEquation)beq["alpha"].get<uint>();
			}
		} catch (std::exception const& e) {
			throw Error::FailedAction(
				"Failed at getting blending values!",
				__FILE__,
				toString(__LINE__),
				"extendFromDefinition",
				e.what(),
				"Please check to see if values are correct!"
			);
		}
	}
	if (def["active"].isBoolean())
		active = def["active"].get<bool>();
}

JSON::JSONData getObjectDefinition(
	String const& encoding = "base64",
	bool const& integratedBinary = true,
	bool const& integratedTextures = true
) {
	// Bake object
	bool wasBaked = baked;
	if (!wasBaked) bake();
	// check if there is any data
	if (vertices == nullptr || vertexCount == 0)
		throw Error::InvalidValue("Renderable object is empty!");
	// Create definition
	JSON::JSONData def;
	// Save mesh components
	def["mesh"] = {
		{"components", "x,y,z,u,v,r,g,b,a,nx,ny,nz"}
	};
	def["version"] = version;
	// If data is to be integrated into the JSON object, do so
	if (integratedBinary) {
		// Allocate data buffer
		ubyte* vertEnd = (ubyte*)(&vertices[vertexCount-1]);
		Data::BinaryData data((ubyte*)vertices, (ubyte*)(vertEnd + sizeof(vertex)));
		// Save mesh data
		def["mesh"]["data"]		= Data::encode(data, Data::fromString(encoding));
		def["mesh"]["encoding"]	= encoding;
		// Clear data buffer
		data.clear();
	}
	// Save transform data
	def["trans"] = {
		{"position",	{trans.position.x,	trans.position.y,	trans.position.z}	},
		{"rotation",	{trans.rotation.x,	trans.rotation.y,	trans.rotation.z}	},
		{"scale",		{trans.scale.x,		trans.scale.y,		trans.scale.z}		}
	};
	// Set active data
	def["active"] = active;
	// Set blend data
	def["blend"] = {
		{"function", {
			{"srcColor", uint(blend.func.srcColor)},
			{"dstColor", uint(blend.func.dstColor)},
			{"srcAlpha", uint(blend.func.srcAlpha)},
			{"dstAlpha", uint(blend.func.dstAlpha)}
		}},
		{"equation", {
			{"color", uint(blend.eq.color)},
			{"alpha", uint(blend.eq.alpha)}
		}}
	};
	// Unbake object if applicable
	if (!wasBaked) unbake();
	// Return definition
	return def;
}
