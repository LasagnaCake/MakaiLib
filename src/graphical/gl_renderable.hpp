/*
Do not touch this class. Please.
*/

namespace {
	using std::string;
}

class Renderable: public Base::DrawableObject<Material::ObjectMaterial> {
public:
	constexpr static size_t version = 0;

	Renderable(size_t layer = 0, bool manual = false):
	DrawableObject(layer, manual) {
		material.texture.image	= &texture;
		material.emission.image	= &emission;
		material.warp.image		= &warp;
	}

	Renderable(
		vector<Triangle*> triangles,
		size_t layer = 0,
		bool manual = false
	): DrawableObject(layer, manual) {
		this->triangles = triangles;
	}

	Renderable(
		RawVertex* vertices,
		size_t count,
		size_t layer = 0,
		bool manual = false
	): DrawableObject(layer, manual) {
		extend(vertices, count);
	}

	Renderable(
		Renderable* other,
		size_t layer = 0,
		bool manual = false
	): DrawableObject(layer, manual) {
		extend(other);
	}

	virtual ~Renderable() {
		locked = false;
		DEBUGLN("Renderable!");
		DEBUGLN(references.plane.size());
		DEBUGLN("Deleting references...");
		clearData();
		DEBUGLN("Killing renderable object...");
	}

	/// Creates a reference and binds it to this object.
	template <Reference::PlaneType T>
	[[nodiscard]] T* createReference() {
		if (locked) throw Error::InvalidAction("Renderable object is locked!");
		Triangle* tris[2] = {
			new Triangle(),
			new Triangle()
		};
		// Add triangles
		triangles.push_back(tris[0]);
		triangles.push_back(tris[1]);
		// Create reference
		T* plane = new T(tris);
		// Setup plane
		plane->setOrigin(
			Vector3(-1.0, +1.0, 0.0),
			Vector3(+1.0, +1.0, 0.0),
			Vector3(-1.0, -1.0, 0.0),
			Vector3(+1.0, -1.0, 0.0)
		);
		plane->setUV(
			Vector2(+0.0, +1.0),
			Vector2(+1.0, +1.0),
			Vector2(+0.0, +0.0),
			Vector2(+1.0, +0.0)
		);
		plane->setColor();
		plane->setNormal(
			Vector3(+0.0, +0.0, -1.0)
		);
		// Add to reference list
		references.plane.push_back(plane);
		// Set destructor function
		plane->onDestroy =	[this, plane](){this->removeReference<T>(plane);};
		plane->onUnbind =	[this, plane](){this->unbindReference<T>(plane);};
		// return plane
		return plane;
	}

	template <Reference::TrigonType T>
	[[nodiscard]] T* createReference() {
		if (locked) throw Error::InvalidAction("Renderable object is locked!");
		Triangle* tris[1] = {new Triangle()};
		// Add triangles
		triangles.push_back(tris[0]);
		// Create reference
		T* tg = new T(tris);
		// Setup trigon
		tg->setOrigin(
			Vector3(-0.0, +1.0, 0.0),
			Vector3(-1.0, -1.0, 0.0),
			Vector3(+1.0, -1.0, 0.0)
		);
		tg->setUV(
			Vector2(+0.5, +1.0),
			Vector2(+0.0, +0.0),
			Vector2(+1.0, +0.0)
		);
		tg->setColor();
		tg->setNormal(
			Vector3(+0.0, +0.0, -1.0)
		);
		// Add to reference list
		references.trigon.push_back(tg);
		// Set destructor function
		tg->onDestroy =	[this, tg](){this->removeReference<T>(tg);};
		tg->onUnbind =	[this, tg](){this->unbindReference<T>(tg);};
		// return trigon
		return tg;
	}

	/// Gets a reference bound to this object by index.
	template <Reference::PlaneType T>
	inline T* getReference(size_t index) {
		if (locked) throw Error::InvalidAction("Renderable object is locked!");
		return (T*)references.plane[index];
	}

	template <Reference::TrigonType T>
	inline T* getReference(size_t index) {
		if (locked) throw Error::InvalidAction("Renderable object is locked!");
		return (T*)references.trigon[index];
	}

	/**
	* Deletes a reference bound to this object.
	*
	* More specifically, it removes the reference
	* and the triangles associated to it.
	* It also deletes the reference.
	*/
	template <Reference::PlaneType T>
	void removeReference(T* ref) {
		if (!ref) return;
		if (locked) return;
		auto tris = ref->getBoundTriangles();
		if (!triangles.empty()) std::erase_if(
			triangles,
			[=](Triangle* e){
				if (e == tris[0]) {delete tris[0]; return true;}
				if (e == tris[1]) {delete tris[1]; return true;}
				return false;
			}
		);
		unbindReference<T>(ref);
	}

	template <Reference::TrigonType T>
	void removeReference(T* ref) {
		if (!ref) return;
		if (locked) return;
		auto tris = ref->getBoundTriangles();
		if (!triangles.empty())std::erase_if(
			triangles,
			[=](Triangle* e){
				if (e == tris[0]) {delete tris[0]; return true;}
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
	template <Reference::PlaneType T>
	void unbindReference(T* ref) {
		if (!ref) return;
		if (locked) return;
		auto& rp = references.plane;
		ERASE_IF(rp, elem == ref);
		delete ref;
	}

	template <Reference::TrigonType T>
	void unbindReference(T* ref) {
		if (!ref) return;
		if (locked) return;
		auto& rp = references.trigon;
		ERASE_IF(rp, elem == ref);
		delete ref;
	}

	/// IRREVERSIBLE.
	void bakeAndLock() {
		if (locked) return;
		bake();
		locked = true;
		clearData();
	}

	void extend(RawVertex* vertices, size_t size) {
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

	void extend(Renderable* other) {
		if (locked) return;
		if (!other->baked) {
			other->bake();
			extend(other->vertices, other->vertexCount);
			other->unbake();
		} else extend(other->vertices, other->vertexCount);
	}

	void extend(vector<Renderable*> parts) {
		for EACH(p, parts)
			extend(p);
	}

	void extendFromBinaryFile(string const& path) {
		auto data = FileLoader::getBinaryFile(path);
		if (!data.size()) throw FileLoader::FileLoadError("File does not exist or is empty! (" + path + ")!");
		extend((RawVertex*)&data[0], data.size() / RAW_VERTEX_BYTE_SIZE);
		DEBUG("Vertices: ");
		DEBUGLN(data.size() / RAW_VERTEX_BYTE_SIZE);
	}

	inline void extendFromDefinitionFile(
		string const& path,
		Texture2D* const& texture	= nullptr,
		Texture2D* const& emission	= nullptr,
		Texture2D* const& warp		= nullptr
	) {
		extendFromDefinition(FileLoader::getJSON(path), FileSystem::getDirectoryFromPath(path), texture, emission, warp);
	}

	void bake() {
		if (baked || locked) return;
		baked = true;
		//Bake vertices
		copyVertices();
	}

	void unbake() {
		if (!baked || locked) return;
		baked = false;
		// Clear vertex buffer
		delete [] vertices;
		vertices = nullptr;
	}

	void clearData() {
		if (vertices && !locked)
			delete [] vertices;
		if (!references.plane.empty())
			for (auto pr: references.plane)
				delete pr;
		if (!references.trigon.empty())
			for (auto pr: references.trigon)
				delete pr;
		references.plane.clear();
		references.trigon.clear();
		if (!triangles.empty())
			for (auto t: triangles)
				delete t;
		triangles.clear();
	}

	void saveToBinaryFile(string const& path) {
		bake();
		FileLoader::saveBinaryFile(path, vertices, vertexCount);
	}

	void saveToDefinitionFile(
		string folder,
		string name = "object",
		string texturesFolder = "tx",
		bool integratedBinary	= false,
		bool integratedTextures	= false,
		bool pretty = false
	) {
		DEBUGLN("Saving object '" + name + "'...");
		// Get paths
		string binpath		= folder + "/" + name + ".mesh";
		DEBUGLN(binpath);
		DEBUGLN(folder + "/" + name + ".mrod");
		FileSystem::makeDirectory(FileSystem::concatenatePath(folder, texturesFolder));
		// Get object definition
		JSONData file = getObjectDefinition("base64", integratedBinary, integratedTextures);
		// If binary is in a different location, save there
		if (!integratedBinary) {
			FileLoader::saveBinaryFile(binpath, vertices, vertexCount);
			file["mesh"]["data"] = {{"path", name + ".mesh"}};
		}
		// Get material data
		file["material"] = getMaterialDefinition(material, folder, texturesFolder, integratedTextures);
		// convert to text
		auto contents = file.dump(pretty ? 1 : -1, '\t', false, JSON::error_handler_t::replace);
		// Save definition file
		FileLoader::saveTextFile(folder + "/" + name + ".mrod", contents);
	}

	vector<Triangle*> triangles;

	Texture2D texture, emission, warp;

private:
	friend class Scene3D;

	RawVertex* vertices = nullptr;

	bool
		baked	= false,
		locked	= false;

	void copyVertices() {
		// If no triangles exist, return
		if (!triangles.size()) return;
		// Transform references (if applicable)
		GRAPHICAL_PARALLEL_FOR
		for (auto& plane: references.plane)	plane->transform();
		GRAPHICAL_PARALLEL_FOR
		for (auto& tg: references.trigon)	tg->transform();
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
		GRAPHICAL_PARALLEL_FOR
		for (auto& plane: references.plane)	plane->reset();
		GRAPHICAL_PARALLEL_FOR
		for (auto& tg: references.trigon)	tg->reset();
	}

	void draw() override {
		// If object's vertices are not "baked" (i.e. finalized), copy them
		if (!baked && !locked) copyVertices();
		// If no vertices, return
		if (!vertices) return;
		// Set shader data
		setShaderData();
		Material::setMaterial(shader, material);
		// Present to screen
		display(vertices, vertexCount);
	}

	/// List of references linked to this object.
	struct {
		vector<Reference::Plane*>	plane;
		vector<Reference::Trigon*>	trigon;
	} references;

	/// The amount of vertices this object has.
	size_t vertexCount = 0;

	void extendFromDefinition(
		JSONData def,
		string const& sourcepath,
		Texture2D* const& texture	= nullptr,
		Texture2D* const& emission	= nullptr,
		Texture2D* const& warp		= nullptr
	) {
		if (def["version"].is_number()) {
			// Do stuff for versions
			switch (def["version"].get<size_t>()) {
				default:
				case 0: extendFromDefinitionV0(def, sourcepath, texture, emission, warp); break;
			}
		} else extendFromDefinitionV0(def, sourcepath, texture, emission, warp);
	}

	void extendFromDefinitionV0(
		JSONData def,
		string const& sourcepath,
		Texture2D* texture	= nullptr,
		Texture2D* emission	= nullptr,
		Texture2D* warp		= nullptr
	) {
		if (!texture)	texture = &this->texture;
		if (!emission)	texture = &this->emission;
		if (!warp)		texture = &this->warp;
		// Component data
		string componentData;
		// Vertex data
		vector<ubyte> vdata;
		// Get mesh data
		try {
			auto& mesh	= def["mesh"];
			auto& data	= mesh["data"];
			if (data.is_string()) {
				string encoding	= mesh["encoding"].get<string>();
				vdata			= decodeData(data.get<string>(), encoding);
			} else if (data.is_object()) {
				vdata			= FileLoader::getBinaryFile(FileSystem::concatenatePath(sourcepath, data["path"].get<string>()));
			}
			componentData		= mesh["components"].get<string>();
		} catch (JSON::exception const& e) {
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
			string error = "";
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
		vector<string> components = Helper::splitString(componentData, ',');
		// Check if valid component data
		{
			string indexes = "";
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
		vector<RawVertex> vertices;
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
				+	std::to_string(vertices.size())
				+	" .\nExpected size is "
				+	std::to_string(size_t(Math::ceil(vertices.size() / 3.0) * 3.0))
				+	"."
				),
				"You either have extra data, or missing data."
			);
		// Create renderable object
		extend(vertices.data(), vertices.size());
		// check for optional transform
		if (def["trans"].is_object()) {
			auto& dtrans = def["trans"];
			try {
				trans.position	= VecMath::fromJSONArrayV3(dtrans["position"]);
				trans.rotation	= VecMath::fromJSONArrayV3(dtrans["rotation"]);
				trans.scale		= VecMath::fromJSONArrayV3(dtrans["scale"]);
			} catch (JSON::exception const& e) {
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
		if (def["material"].is_object()) {
			material = Material::fromObjectMaterialDefinition(def["material"], sourcepath, texture, emission, warp);
		}
		// Set blend data
		if (def["blend"].is_object()) {
			JSONData& bfun	= def["blend"]["function"];
			JSONData& beq	= def["blend"]["equation"];
			if (bfun.is_number()) {
				GLenum bv = getBlendFunction(bfun.get<uint>());
				blend.func = {bv, bv, bv, bv};
			} else if (bfun.is_object()) {
				if (bfun["src"].is_number()) {
					blend.func.srcColor = blend.func.srcAlpha = getBlendFunction(bfun["src"].get<uint>());
				} else {
					setBlendFunctionFromJSON(blend.func.srcColor, bfun["srcColor"]);
					setBlendFunctionFromJSON(blend.func.srcAlpha, bfun["srcAlpha"]);
				}
				if (bfun["dst"].is_number()) {
					blend.func.dstColor = blend.func.dstAlpha = getBlendFunction(bfun["dst"].get<uint>());
				} else {
					setBlendFunctionFromJSON(blend.func.dstColor, bfun["dstColor"]);
					setBlendFunctionFromJSON(blend.func.dstAlpha, bfun["dstAlpha"]);
				}
			}
			if (beq.is_number()) {
				GLenum bv = getBlendFunction(beq.get<uint>());
				blend.eq = {bv, bv};
			} else if (beq.is_object()) {
				setBlendEquationFromJSON(blend.eq.color, beq["color"]);
				setBlendEquationFromJSON(blend.eq.alpha, beq["alpha"]);
			}
		}
		if (def["active"].is_boolean())
			active = def["active"].get<bool>();
	}

	JSONData getObjectDefinition(string const& encoding = "base64", bool integratedBinary = true, bool integratedTextures = true) {
		// Bake object
		bool wasBaked = baked;
		if (!wasBaked) bake();
		// check if there is any data
		if (vertices == nullptr || vertexCount == 0)
			throw Error::InvalidValue("Renderable object is empty!");
		// Create definition
		JSONData def;
		// Save mesh components
		def["mesh"] = {
			{"components", RAW_VERTEX_COMPONENTS}
		};
		def["version"] = version;
		// If data is to be integrated into the JSON object, do so
		if (integratedBinary) {
			// Allocate data buffer
			ubyte* vertEnd = (ubyte*)(&vertices[vertexCount-1]);
			vector<ubyte> data((ubyte*)vertices, (ubyte*)(vertEnd + RAW_VERTEX_BYTE_SIZE));
			// Save mesh data
			def["mesh"]["data"]		= encodeData(data, encoding);
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
				{"srcColor", blendFunctionToJSON(blend.func.srcColor)},
				{"dstColor", blendFunctionToJSON(blend.func.dstColor)},
				{"srcAlpha", blendFunctionToJSON(blend.func.srcAlpha)},
				{"dstAlpha", blendFunctionToJSON(blend.func.dstAlpha)}
			}},
			{"equation", {
				{"color", blendEquationToJSON(blend.eq.color)},
				{"alpha", blendEquationToJSON(blend.eq.alpha)}
			}}
		};
		// Unbake object if applicable
		if (!wasBaked) unbake();
		// Return definition
		return def;
	}

	void setBlendFunctionFromJSON(GLenum& param, JSONData& value) {
		if (!value.is_number()) return;
		param = getBlendFunction(value.get<GLuint>());
	}

	void setBlendEquationFromJSON(GLenum& param, JSONData& value) {
		if (!value.is_number()) return;
		param = getBlendEquation(value.get<GLuint>());
	}

	uint blendFunctionToJSON(GLenum const& value) {
		switch (value) {
			case GL_ZERO:						return 0x00;
			case GL_ONE:						return 0x01;
			case GL_SRC_COLOR:					return 0x02;
			case GL_ONE_MINUS_SRC_COLOR:		return 0x03;
			case GL_DST_COLOR:					return 0x04;
			case GL_ONE_MINUS_DST_COLOR:		return 0x05;
			case GL_SRC_ALPHA:					return 0x06;
			case GL_ONE_MINUS_SRC_ALPHA:		return 0x07;
			case GL_DST_ALPHA:					return 0x08;
			case GL_ONE_MINUS_DST_ALPHA:		return 0x09;
			case GL_CONSTANT_COLOR:				return 0x0A;
			case GL_ONE_MINUS_CONSTANT_COLOR:	return 0x0B;
			case GL_CONSTANT_ALPHA:				return 0x0C;
			case GL_ONE_MINUS_CONSTANT_ALPHA:	return 0x0D;
			case GL_SRC_ALPHA_SATURATE:			return 0x0E;
			/*
			case GL_SRC1_COLOR:					return 0x0F;
			case GL_ONE_MINUS_SRC1_COLOR:		return 0x10;
			case GL_SRC1_ALPHA:					return 0x11;
			case GL_ONE_MINUS_SRC1_ALPHA:		return 0x12;
			*/
			default: return 0x1; break;
		}
	}

	uint blendEquationToJSON(GLenum const& value) {
		switch (value) {
			case GL_ZERO:					return 0x00;
			case GL_ONE:					return 0x01;
			case GL_FUNC_ADD:				return 0x02;
			case GL_FUNC_SUBTRACT:			return 0x03;
			case GL_FUNC_REVERSE_SUBTRACT:	return 0x04;
			case GL_MIN:					return 0x05;
			case GL_MAX:					return 0x06;
			default: return 0x2;
		}
	}

	GLenum getBlendFunction(uint const& value) {
		switch (value) {
			case 0x00: return GL_ZERO;
			case 0x01: return GL_ONE;
			case 0x02: return GL_SRC_COLOR;
			case 0x03: return GL_ONE_MINUS_SRC_COLOR;
			case 0x04: return GL_DST_COLOR;
			case 0x05: return GL_ONE_MINUS_DST_COLOR;
			case 0x06: return GL_SRC_ALPHA;
			case 0x07: return GL_ONE_MINUS_SRC_ALPHA;
			case 0x08: return GL_DST_ALPHA;
			case 0x09: return GL_ONE_MINUS_DST_ALPHA;
			case 0x0A: return GL_CONSTANT_COLOR;
			case 0x0B: return GL_ONE_MINUS_CONSTANT_COLOR;
			case 0x0C: return GL_CONSTANT_ALPHA;
			case 0x0D: return GL_ONE_MINUS_CONSTANT_ALPHA;
			case 0x0E: return GL_SRC_ALPHA_SATURATE;
			/*
			case 0x0F: return GL_SRC1_COLOR;
			case 0x10: return GL_ONE_MINUS_SRC1_COLOR;
			case 0x11: return GL_SRC1_ALPHA;
			case 0x12: return GL_ONE_MINUS_SRC1_ALPHA;
			*/
			default: return GL_ONE;
		}
	}

	GLenum getBlendEquation(uint const& value) {
		switch (value) {
			case 0x00: return GL_ZERO;
			case 0x01: return GL_ONE;
			case 0x02: return GL_FUNC_ADD;
			case 0x03: return GL_FUNC_SUBTRACT;
			case 0x04: return GL_FUNC_REVERSE_SUBTRACT;
			case 0x05: return GL_MIN;
			case 0x06: return GL_MAX;
			default: return GL_FUNC_ADD;
		}
	}
};



enum class LineType {
	SEGMENTS,
	LOOPED,
	LINKED
};

class LineRenderable: public Base::DrawableObject<Material::ObjectMaterial> {
public:
	LineRenderable(size_t layer = 0, bool manual = false): DrawableObject(layer, manual) {}

	LineRenderable(
		RawVertex* points,
		size_t count,
		size_t layer = 0,
		bool manual = false
	): DrawableObject(layer, manual) {
		extend(points, count);
	}

	LineRenderable(
		LineRenderable* other,
		size_t layer = 0,
		bool manual = false
	): DrawableObject(layer, manual) {
		extend(other);
	}

	virtual ~LineRenderable() {
		DEBUGLN("LineRenderable!");
		DEBUGLN("Killing LineRenderable object...");
	}

	void extend(RawVertex* points, size_t size) {
		if (points == nullptr || size == 0)
			throw Error::InvalidValue("No vertices were provided!");
		for SSRANGE(i, 0, size) {
			this->points.push_back(points[i]);
		}
	}

	void extend(vector<RawVertex> points) {
		extend(points.data(), points.size());
	}

	void extend(LineRenderable* other) {
		extend(other->points);
	}

	void extend(vector<LineRenderable*> parts) {
		for EACH(p, parts)
			extend(p);
	}

	void extendFromBinaryFile(string const& path) {
		auto data = FileLoader::getBinaryFile(path);
		if (!data.size()) throw FileLoader::FileLoadError("File does not exist or is empty! (" + path + ")!");
		extend((RawVertex*)&data[0], data.size() / sizeof(RawVertex));
	}

	void saveToFile(std::string const& path) {
		FileLoader::saveBinaryFile(path, points.data(), points.size());
	}

	vector<RawVertex> points;

	LineType type = LineType::SEGMENTS;

private:
	void draw() override {
		// If no points, return
		if (points.empty()) return;
		// Set shader data
		setShaderData();
		Material::setMaterial(shader, material);
		// Present to screen
		display(points.data(), points.size(), GL_LINES + (unsigned int)type);
	}
};

class PointRenderable: public Base::DrawableObject<Material::ObjectMaterial> {
public:
	PointRenderable(size_t layer = 0, bool manual = false): DrawableObject(layer, manual) {}

	PointRenderable(
		RawVertex* points,
		size_t count,
		size_t layer = 0,
		bool manual = false
	): DrawableObject(layer, manual) {
		extend(points, count);
	}

	PointRenderable(
		PointRenderable* other,
		size_t layer = 0,
		bool manual = false
	): DrawableObject(layer, manual) {
		extend(other);
	}

	virtual ~PointRenderable() {
		DEBUGLN("PointRenderable!");
		DEBUGLN("Killing PointRenderable object...");
	}

	void extend(RawVertex* points, size_t size) {
		if (points == nullptr || size == 0)
			throw Error::InvalidValue("No vertices were provided!");
		for SSRANGE(i, 0, size) {
			this->points.push_back(points[i]);
		}
	}

	void extend(vector<RawVertex> points) {
		extend(points.data(), points.size());
	}

	void extend(PointRenderable* other) {
		extend(other->points);
	}

	void extend(vector<PointRenderable*> parts) {
		for EACH(p, parts)
			extend(p);
	}

	void extendFromBinaryFile(string const& path) {
		auto data = FileLoader::getBinaryFile(path);
		if (!data.size()) throw FileLoader::FileLoadError("File does not exist or is empty! (" + path + ")!");
		extend((RawVertex*)&data[0], data.size() / sizeof(RawVertex));
	}

	void saveToFile(string const& path) {
		FileLoader::saveBinaryFile(path, points.data(), points.size());
	}

	vector<RawVertex> points;

private:
	void draw() override {
		// If no points, return
		if (points.empty()) return;
		// Set shader data
		setShaderData();
		Material::setMaterial(shader, material);
		// Present to screen
		display(points.data(), points.size(), GL_POINT);
	}
};

Renderable* loadObjectFromBinaryFile(string const& path) {
	auto data = FileLoader::getBinaryFile(path);
	if (data.empty()) throw FileLoader::FileLoadError("File does not exist or is empty! (" + path + ")!");
	return new Renderable((RawVertex*)&data[0], data.size() / sizeof(RawVertex));
}

Renderable* loadObjectFromDefinitionFile(string const& path) {
	// Create object
	Renderable* r = new Renderable();
	// Load data
	r->extendFromDefinitionFile(path);
	// Return object
	return r;
}

typedef vector<Renderable*> RenderableList;
