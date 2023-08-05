/*
Do not touch this class. Please.
*/

namespace {
	using std::string;
}

#define _ENCDEC_CASE(T, F) if (encoding == T) return F(data)
vector<ubyte> decodeData(string const& data, string const& encoding) {
	_ENCDEC_CASE	("base32",	cppcodec::base32_rfc4648::decode);
	_ENCDEC_CASE	("base64",	cppcodec::base64_rfc4648::decode);
	throw Error::InvalidValue(
		"Invalid encoding: " + encoding,
		__FILE__,
		toString(__LINE__),
		"decodeData"
	);
}

string encodeData(vector<ubyte> const& data, string const& encoding) {
	_ENCDEC_CASE	("base32",	cppcodec::base32_rfc4648::encode);
	_ENCDEC_CASE	("base64",	cppcodec::base64_rfc4648::encode);
	throw Error::InvalidValue(
		"Invalid encoding: " + encoding,
		__FILE__,
		toString(__LINE__),
		"decodeData"
	);
}
#undef _ENCDEC_CASE

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
		$eraseif(rp, elem == ref);
		delete ref;
	}

	template <Reference::TrigonType T>
	void unbindReference(T* ref) {
		if (!ref) return;
		if (locked) return;
		auto& rp = references.trigon;
		$eraseif(rp, elem == ref);
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

	inline void extendFromDefinitionFile(
		string const& path,
		Texture2D* texture	= nullptr,
		Texture2D* emission	= nullptr,
		Texture2D* warp		= nullptr
	) {
		extendFromDefinition(FileLoader::loadJSON(path), FileSystem::getDirectoryFromPath(path), texture, emission, warp);
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
		$debug("Saving object '" + name + "'...");
		// Get paths
		string binpath		= folder + "/" + name + ".mesh";
		$debug(binpath);
		$debug(folder + "/" + name + ".mrod");
		FileSystem::makeDirectory(FileSystem::concatenatePath(folder, texturesFolder));
		// Get object definition
		nlohmann::json file = getObjectDefinition("base64", integratedBinary, integratedTextures);
		// If binary is in a different location, save there
		if (!integratedBinary) {
			FileLoader::saveBinaryFile(binpath, vertices, vertexCount);
			file["mesh"]["data"] = {{"path", name + ".mesh"}};
		}
		Material::ObjectMaterial& mat = material;
		auto& mdef = file["material"];
		// Save image texture
		if (!integratedTextures)
			mdef["texture"] = saveImageEffect(mat.texture, folder, texturesFolder + "/texture.tga");
		mdef["texture"]["alphaClip"] = mat.texture.alphaClip;
		// Save emission texture
		if (!integratedTextures)
			mdef["emission"] = saveImageEffect(mat.emission, folder, texturesFolder + "/emission.tga");
		mdef["texture"]["alphaClip"] = mat.emission.alphaClip;
		// Save warp texture
		if (!integratedTextures)
			mdef["warp"] = saveImageEffect(mat.warp, folder, texturesFolder + "/warp.tga");
		mdef["warp"]["channelX"] = mat.warp.channelX;
		mdef["warp"]["channelY"] = mat.warp.channelY;
		mdef["warp"]["trans"] = {
			{"position",	{mat.warp.trans.position.x,	mat.warp.trans.position.y	}	},
			{"rotation",	mat.warp.trans.rotation										},
			{"scale",		{mat.warp.trans.scale.x,	mat.warp.trans.scale.y		}	}
		};
		// convert to text
		auto contents = file.dump(pretty ? 1 : -1, '\t', false, nlohmann::json::error_handler_t::replace);
		// Save definition file
		FileLoader::saveTextFile(folder + "/" + name + ".mrod", contents);
	}

	vector<Triangle*> triangles;

private:
	friend class Scene3D;

	nlohmann::json saveImageEffect(Material::ImageEffect& effect, string const& folder, string const& path) {
		nlohmann::json def;
		def["enabled"] = effect.enabled;
		if (effect.image && effect.image->exists()) {
			effect.image->saveToFile(folder + "/" + path);
			def["image"] = {
				{"path", path},
				{"minFilter", effect.image->getTextureMinFilter()},
				{"magFilter", effect.image->getTextureMagFilter()}
			};
		} else def["enabled"] = false;
		return def;
	}

	Material::ImageEffect loadImageEffect(nlohmann::json& effect, string const& sourcepath, Texture2D* texture = nullptr) {
		try {
			Material::ImageEffect fx;
			fx.enabled = effect["enabled"].get<bool>();
			auto& img = effect["image"];
			if (!texture)
				texture = new Texture2D();
			fx.image = texture;
			if (img["data"].is_object() && img["data"]["path"].is_string() && !img["data"]["path"].get<string>().empty()) {
				texture->create(FileSystem::concatenatePath(sourcepath, img["path"].get<string>()));
				texture->setTextureFilterMode(
					img["minFilter"].get<unsigned int>(),
					img["magFilter"].get<unsigned int>()
				);
			} else if (img["data"].is_string() && !img["data"].get<string>().empty()) {
				vector<ubyte> data = decodeData(img["data"].get<string>(), img["encoding"]);
				int w, h, nc;
				uchar* imgdat = stbi_load_from_memory(
					data.data(),
					data.size(),
					&w,
					&h,
					&nc,
					4
				);
				if (imgdat) {
					texture->create(
						w,
						h,
						GL_UNSIGNED_BYTE,
						GL_RGBA,
						img["magFilter"].get<unsigned int>(),
						img["minFilter"].get<unsigned int>(),
						imgdat
					);
					stbi_image_free(imgdat);
				} else throw Error::FailedAction(
						"Failed at getting image effect!",
						__FILE__,
						toString(__LINE__),
						"extendFromDefinition",
						"Could not decode embedded image data!",
						"Please check to see if values are correct!"
					);
			} else fx.enabled = false;
			return fx;
		} catch (nlohmann::json::exception e) {
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

	void extendFromDefinition(
		nlohmann::json def,
		string const& sourcepath,
		Texture2D* const& texture	= nullptr,
		Texture2D* const& emission	= nullptr,
		Texture2D* const& warp		= nullptr
	) {
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
				vdata			= FileLoader::loadBinaryFile(FileSystem::concatenatePath(sourcepath, data["path"].get<string>()));
			}
			componentData		= mesh["components"].get<string>();
		} catch (nlohmann::json::exception e) {
			throw Error::FailedAction(
				"Failed at getting mesh values!",
				__FILE__,
				toString(__LINE__),
				"extendFromDefinition",
				e.what(),
				"Please check to see if values are correct!"
			);
		}
		$debug(componentData);
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
				if(c.empty()) indexes += std::to_string(i) + " ";
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
		#define _SET_PARAM(PARAM)\
			trans.PARAM = Vector3(\
				dtrans[#PARAM][0].get<float>(),\
				dtrans[#PARAM][1].get<float>(),\
				dtrans[#PARAM][2].get<float>()\
			)
		// check for optional transform
		if (def["trans"].is_object()) {
			auto& dtrans = def["trans"];
			try {
				_SET_PARAM(position);
				_SET_PARAM(rotation);
				_SET_PARAM(scale);
			} catch (nlohmann::json::exception e) {
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
		#undef _SET_PARAM
		// Set material data
		if (def["material"].is_object()) {
			try {
				auto& dmat = def["material"];
				// Set color
				if(dmat["color"].is_array()) {
					material.color.x = dmat["color"][0].get<float>();
					material.color.y = dmat["color"][1].get<float>();
					material.color.z = dmat["color"][2].get<float>();
					material.color.w = dmat["color"][3].get<float>();
				}
				// Set color & shading params
				#define _SET_BOOL_PARAM(PARAM) if(dmat[#PARAM].is_boolean()) material.PARAM = dmat[#PARAM].get<bool>()
				_SET_BOOL_PARAM(shaded);
				_SET_BOOL_PARAM(illuminated);
				#undef _SET_BOOL_PARAM
				#define _SET_FLOAT_PARAM(PARAM) if(dmat[#PARAM].is_number()) material.PARAM = dmat[#PARAM].get<float>()
				_SET_FLOAT_PARAM(hue);
				_SET_FLOAT_PARAM(saturation);
				_SET_FLOAT_PARAM(luminosity);
				_SET_FLOAT_PARAM(brightness);
				_SET_FLOAT_PARAM(contrast);
				#undef _SET_FLOAT_PARAM
				// Set UV shift
				if(dmat["uvShift"].is_array()) {
					material.uvShift.x = dmat["uvShift"][0].get<float>();
					material.uvShift.y = dmat["uvShift"][1].get<float>();
				}
				// Set texture
				if (dmat["texture"].is_object()) {
					auto fx = loadImageEffect(dmat["texture"], sourcepath, texture);
					material.texture.enabled	= fx.enabled;
					material.texture.image		= fx.image;
					if (dmat["texture"]["alphaClip"].is_boolean())
						material.texture.alphaClip	= dmat["texture"]["alphaClip"].get<float>();
				}
				// Set emission texture
				if (dmat["emission"].is_object()) {
					auto fx = loadImageEffect(dmat["emission"], sourcepath, emission);
					material.emission.enabled	= fx.enabled;
					material.emission.image		= fx.image;
					if (dmat["emission"]["alphaClip"].is_boolean())
						material.emission.alphaClip	= dmat["emission"]["alphaClip"].get<float>();
				}
				// Set warp texture
				if (dmat["warp"].is_object()) {
					auto fx = loadImageEffect(dmat["warp"], sourcepath, warp);
					material.warp.enabled	= fx.enabled;
					material.warp.image		= fx.image;
					{
						auto& mwtrans = dmat["warp"]["trans"];
						material.warp.trans.position = Vector2(
							mwtrans["position"][0].get<float>(),
							mwtrans["position"][1].get<float>()
						);
						material.warp.trans.rotation = mwtrans["rotation"].get<float>();
						material.warp.trans.scale = Vector2(
							mwtrans["scale"][0].get<float>(),
							mwtrans["scale"][1].get<float>()
						);
					}
					material.warp.channelX = dmat["warp"]["channelX"];
					material.warp.channelY = dmat["warp"]["channelY"];
				}
				// Set negative
				if (dmat["negative"].is_object()) {
					material.negative.enabled	= dmat["negative"]["enabled"].get<bool>();
					material.negative.strength	= dmat["negative"]["strength"].get<float>();
				}
				// Set gradient
				if (dmat["gradient"].is_object()) {
					material.gradient.enabled	= dmat["gradient"]["enabled"].get<bool>();
					material.gradient.channel	= dmat["gradient"]["channel"].get<unsigned int>();
					auto& dgbegin	= dmat["gradient"]["begin"];
					auto& dgend		= dmat["gradient"]["end"];
					material.gradient.begin		= Vector4(
						dgbegin[0].get<float>(),
						dgbegin[1].get<float>(),
						dgbegin[2].get<float>(),
						dgbegin[3].get<float>()
					);
					material.gradient.end		= Vector4(
						dgend[0].get<float>(),
						dgend[1].get<float>(),
						dgend[2].get<float>(),
						dgend[3].get<float>()
					);
					material.gradient.invert	= dmat["gradient"]["invert"].get<bool>();
				}
				// Set instances
				if (dmat["instances"].is_array()) {
					material.instances.clear();
					for(auto& inst: dmat["instances"])
						material.instances.push_back(
							Vector3(
								inst[0].get<float>(),
								inst[1].get<float>(),
								inst[2].get<float>()
							)
						);
				}
				// Set culling, fill & view
				if (dmat["culling"].is_number()) {
					switch (dmat["culling"].get<unsigned int>()) {
						default:
						case 0: material.culling = GL_FRONT_AND_BACK;	break;
						case 1: material.culling = GL_FRONT;			break;
						case 2: material.culling = GL_BACK;				break;
					}
				}
				if (dmat["fill"].is_number()) {
					switch (dmat["fill"].get<unsigned int>()) {
						default:
						case 0: material.fill = GL_FILL;	break;
						case 1: material.fill = GL_LINE;	break;
						case 2: material.fill = GL_POINT;	break;
					}
				}
				if (dmat["debug"].is_number())
					material.debug = (Material::ObjectDebugView)dmat["debug"].get<unsigned int>();
			} catch (nlohmann::json::exception e) {
				throw Error::FailedAction(
					"Failed at getting material values!",
					__FILE__,
					toString(__LINE__),
					"extendFromDefinition",
					e.what(),
					"Please check to see if values are correct!"
				);
			}
		}
	}

	nlohmann::json getObjectDefinition(string const& encoding = "base64", bool integratedBinary = true, bool integratedTextures = true) {
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
		// Get material
		Material::ObjectMaterial& mat = material;
		// Copy instances
		vector<nlohmann::json> instanceData;
		for (Vector3& inst: mat.instances) {
			instanceData.push_back({inst.x, inst.y, inst.z});
		}
		// Save material data
		def["material"] = {
			{"color", {mat.color.x, mat.color.y, mat.color.z, mat.color.w}},
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
				{"begin", {mat.gradient.begin.x, mat.gradient.begin.y, mat.gradient.begin.z, mat.gradient.begin.w}},
				{"end", {mat.gradient.end.x, mat.gradient.end.y, mat.gradient.end.z, mat.gradient.end.w}},
				{"invert", mat.gradient.invert}
			}},
			{"instances", instanceData},
			{"debugView", (unsigned int)mat.debug}
		};
		switch (mat.fill) {
			case GL_FILL:	def["material"]["fill"] = 0; break;
			case GL_LINE:	def["material"]["fill"] = 1; break;
			case GL_POINT:	def["material"]["fill"] = 2; break;
		}
		switch (mat.culling) {
			case GL_FRONT_AND_BACK:	def["material"]["culling"] = 0; break;
			case GL_FRONT:			def["material"]["culling"] = 1; break;
			case GL_BACK:			def["material"]["culling"] = 2; break;
		}
		if (integratedTextures) {
			// TODO: Do integrated textures
		}
		// Unbake object if applicable
		if (!wasBaked) unbake();
		// Return definition
		return def;
	}
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

Renderable* loadObjectFromDefinitionFile(string const& path) {
	// Create object
	Renderable* r = new Renderable();
	// Load data
	r->extendFromDefinitionFile(path);
	// Return object
	return r;
}

typedef vector<Renderable*> RenderableList;
