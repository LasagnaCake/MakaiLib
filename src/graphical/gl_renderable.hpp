/*
Do not touch this class. Please.
*/

namespace {
	using std::string;
}

#define ENCDEC_CASE(T, F) if (encoding == T) return F(data)
vector<ubyte> decodeData(string const& data, string const& encoding) {
	ENCDEC_CASE	("base32",	cppcodec::base32_rfc4648::decode);
	ENCDEC_CASE	("base64",	cppcodec::base64_rfc4648::decode);
	throw Error::InvalidValue(
		"Invalid encoding: " + encoding,
		"gl_renderable",
		"unspecified",
		"decodeData"
	);
}

string encodeData(vector<ubyte> const& data, string const& encoding) {
	ENCDEC_CASE	("base32",	cppcodec::base32_rfc4648::encode);
	ENCDEC_CASE	("base64",	cppcodec::base64_rfc4648::encode);
	throw Error::InvalidValue(
		"Invalid encoding: " + encoding,
		"gl_renderable",
		"unspecified",
		"decodeData"
	);
}
#undef ENCDEC_CASE

class Renderable: public Base::DrawableObject {
public:
	Renderable(size_t layer = 0, bool manual = false): DrawableObject(layer, manual) {}

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
		$debug("Renderable!");
		$debug(references.plane.size());
		$debug("Deleting references...");
		clearData();
		$debug("Killing renderable object...");
	}

	/// Creates a reference and binds it to this object.
	template <Reference::PlaneType T>
	T* createReference() {
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
	T* createReference() {
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
		triangles.erase(
			std::remove_if(
				triangles.begin(),
				triangles.end(),
				[=](Triangle* e){
					if (e == tris[0]) {delete tris[0]; return true;}
					if (e == tris[1]) {delete tris[1]; return true;}
					return false;
				}
			),
			triangles.end()
		);
		unbindReference<T>(ref);
	}

	template <Reference::TrigonType T>
	void removeReference(T* ref) {
		if (!ref) return;
		if (locked) return;
		auto tris = ref->getBoundTriangles();
		triangles.erase(
			std::remove_if(
				triangles.begin(),
				triangles.end(),
				[=](Triangle* e){
					if (e == tris[0]) {delete tris[0]; return true;}
					return false;
				}
			),
			triangles.end()
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
		rp.erase(
			std::remove_if(
				rp.begin(),
				rp.end(),
				[&](Reference::Plane* e){return e == ref;}
			),
			rp.end()
		);
	}

	template <Reference::TrigonType T>
	void unbindReference(T* ref) {
		if (!ref) return;
		if (locked) return;
		auto& rp = references.trigon;
		rp.erase(
			std::remove_if(
				rp.begin(),
				rp.end(),
				[&](Reference::Plane* e){return e == ref;}
			),
			rp.end()
		);
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
		if (this->vertices)
			delete[] this->vertices;
		for $range(i, 0, size, 3) {
			triangles.push_back(
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
		for $each(p, parts)
			extend(p);
	}

	void extendFromBinaryFile(string const& path) {
		auto data = FileLoader::loadBinaryFile(path);
		if (!data.size()) throw Error::FailedAction("File does not exist or is empty! (" + path + ")!");
		extend((RawVertex*)&data[0], data.size() / RAW_VERTEX_BYTE_SIZE);
		$debugp("Vertices: ");
		$debug(data.size() / RAW_VERTEX_BYTE_SIZE);
	}

	void extendFromDefinition(nlohmann::json def) {
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
				vdata			= FileLoader::loadBinaryFile(data["path"].get<string>());
			}
			componentData		= mesh["components"].get<string>();
		} catch (nlohmann::json::exception e) {
			throw Error::FailedAction(
				"Failed at getting mesh values!",
				"gl_renderable",
				"unspecified",
				"extendFromDefinition",
				e.what(),
				"Please check to see if values are correct!"
			);
		}
		// Check if important data is not empty
		{
			string error = "";
			if (vdata.empty())			error += ("Missing mesh's vertex data!\n");
			if (componentData.empty())	error += ("Missing mesh's component data!\n");
			if (!error.empty()) throw Error::InvalidValue(
				"Missing mesh data!\n\n" + error,
				"gl_renderable",
				"unspecified",
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
				if(c.empty()) indexes += std::to_string(i) + " ";
				i++;
			}
			if (!indexes.empty()) {
				throw Error::InvalidValue(
					"Malformed component data!\n\nIndex(es): [ " + indexes + "]",
					"gl_renderable",
					"unspecified",
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
					"gl_renderable",
					"unspecified",
					"extendFromDefinition",
					(
						"Vertex data size is "
					+	std::to_string(vds)
					+	" ("
					+	std::to_string(vdata.size())
					+	" bytes).\nExpected size is "
					+	std::to_string(expected)
					+	" ("
					+	std::to_string(expected * vsize)
					+	" bytes)."
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
			for (auto& c : components)
				vm[c] = rawdata[component++];
			vertices.push_back(Drawer::toRawVertex(vm));
		}
		// Create renderable object
		extend(vertices.data(), vertices.size());
		#define SET_PARAM(PARAM)\
			if (dtrans[#PARAM].is_array())\
				trans.PARAM = Vector3(\
					dtrans[#PARAM][0].get<float>(),\
					dtrans[#PARAM][1].get<float>(),\
					dtrans[#PARAM][2].get<float>()\
				)
		// check for optional transform
		if (def["trans"].is_object()) {
			auto& dtrans = def["trans"];
			try {
				SET_PARAM(position);
				SET_PARAM(rotation);
				SET_PARAM(scale);
			} catch (nlohmann::json::exception e) {
				throw Error::FailedAction(
					"Failed at getting transformation values!",
					"gl_renderable",
					"unspecified",
					"extendFromDefinition",
					e.what(),
					"Please check to see if values are correct!"
				);
			}
		}
		#undef SET_PARAM
		// Set material data
		// TODO: Add support for materials
		// Return new renderable object
	}

	inline void extendFromDefinitionFile(string const& path) {
		extendFromDefinition(FileLoader::loadJSON(path));
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

	void saveToDefinitionFile(string const& path, string const& encoding = "base64", bool pretty = false, string const& binpath = "") {
		// Get object definition
		nlohmann::json file = getObjectDefinition(encoding, binpath.empty());
		// convert to text
		auto contents = file.dump(pretty ? 1 : -1, '\t', false, nlohmann::json::error_handler_t::replace);
		// If binary is in a different location, save there
		if (!binpath.empty()) {
			FileLoader::saveBinaryFile(binpath, vertices, vertexCount);
			file["mesh"]["path"] = binpath;
		}
		// Save definition file
		FileLoader::saveTextFile(path, contents);
	};

	nlohmann::json getObjectDefinition(string const& encoding = "base64", bool integratedBinary = true) {
		// Bake object
		bool wasBaked = baked;
		if (!wasBaked) bake();
		// check if there is any data
		if (vertices == nullptr || vertexCount == 0)
			throw Error::InvalidValue("Renderable object is empty!");
		// Create definition
		nlohmann::json def;
		// Save mesh components
		def["mesh"] = {
			{"components", RAW_VERTEX_COMPONENTS}
		};
		// If data is to be integrated into the JSON object, do so
		if (integratedBinary) {
			// Allocate data buffer
			ubyte* vertEnd = (ubyte*)(&vertices[vertexCount-1]);
			vector<ubyte> data((ubyte*)vertices, (ubyte*)(vertEnd + RAW_VERTEX_BYTE_SIZE));
			// Save mesh data
			def["mesh"] = {
				{"data",		encodeData(data, encoding)	},
				{"encoding",	encoding					},
			};
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
		// Save material data
		// TODO: material data
		// Unbake object if applicable
		if (!wasBaked) unbake();
		// Return definition
		return def;
	}

	vector<Triangle*> triangles;

private:
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
			// Oh, hey, C! haven't seen you in a while!
			*(Triangle*)&vertices[i]	= (*t);
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
		setDefaultShader();
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
};

enum class LineType {
	SEGMENTS,
	LOOPED,
	LINKED
};

class LineRenderable: public Base::DrawableObject {
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
		$debug("LineRenderable!");
		$debug("Killing LineRenderable object...");
	}

	void extend(RawVertex* points, size_t size) {
		if (points == nullptr || size == 0)
			throw Error::InvalidValue("No vertices were provided!");
		for $ssrange(i, 0, size) {
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
		for $each(p, parts)
			extend(p);
	}

	void extendFromBinaryFile(string const& path) {
		auto data = $fld loadBinaryFile(path);
		if (!data.size()) throw Error::FailedAction("File does not exist or is empty! (" + path + ")!");
		extend((RawVertex*)&data[0], data.size() / sizeof(RawVertex));
	}

	void saveToFile(std::string const& path) {
		$fld saveBinaryFile(path, points.data(), points.size());
	}

	vector<RawVertex> points;

	LineType type = LineType::SEGMENTS;

private:
	void draw() override {
		// If no points, return
		if (points.empty()) return;
		// Set shader data
		setDefaultShader();
		// Present to screen
		display(points.data(), points.size(), GL_LINES + (unsigned int)type);
	}
};

class PointRenderable: public Base::DrawableObject {
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
		$debug("PointRenderable!");
		$debug("Killing PointRenderable object...");
	}

	void extend(RawVertex* points, size_t size) {
		if (points == nullptr || size == 0)
			throw Error::InvalidValue("No vertices were provided!");
		for $ssrange(i, 0, size) {
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
		for $each(p, parts)
			extend(p);
	}

	void extendFromBinaryFile(string const& path) {
		auto data = $fld loadBinaryFile(path);
		if (!data.size()) throw Error::FailedAction("File does not exist or is empty! (" + path + ")!");
		extend((RawVertex*)&data[0], data.size() / sizeof(RawVertex));
	}

	void saveToFile(string const& path) {
		$fld saveBinaryFile(path, points.data(), points.size());
	}

	vector<RawVertex> points;

private:
	void draw() override {
		// If no points, return
		if (points.empty()) return;
		// Set shader data
		setDefaultShader();
		// Present to screen
		display(points.data(), points.size(), GL_POINT);
	}
};

Renderable* loadObjectFromBinaryFile(string const& path) {
	auto data = FileLoader::loadBinaryFile(path);
	if (data.empty()) throw Error::FailedAction("File does not exist or is empty! (" + path + ")!");
	return new Renderable((RawVertex*)&data[0], data.size() / sizeof(RawVertex));
}

[[unavailable("Unimplemented!")]]
Renderable* loadObjectFromGLTFFile(string const& path) {return nullptr;}

// TODO: Test this
Renderable* loadObjectFromDefinition(nlohmann::json def) {
	// Create object
	Renderable* r = new Renderable();
	// Load data
	r->extendFromDefinition(def);
	// Return object
	return r;
}

Renderable* loadObjectFromDefinitionFile(string const& path) {
	// Load object file
	nlohmann::json file = FileLoader::loadJSON(path);
	// Return new renderable object
	return loadObjectFromDefinition(file);
}
