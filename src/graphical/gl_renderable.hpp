/*
Do not touch this class. Please.
*/

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
		if (locked) return;
		auto tris = ref->getBoundTriangles();
		triangles.erase(
			std::remove_if(
				triangles.begin(),
				triangles.end(),
				[=](Triangle* e){return (e == tris[0]) || (e == tris[1]);}
			),
			triangles.end()
		);
		unbindReference<T>(ref);
	}

	template <Reference::TrigonType T>
	void removeReference(T* ref) {
		if (locked) return;
		auto tris = ref->getBoundTriangles();
		triangles.erase(
			std::remove_if(
				triangles.begin(),
				triangles.end(),
				[=](Triangle* e){return (e == tris[0]);}
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
		delete ref;
	}

	template <Reference::TrigonType T>
	void unbindReference(T* ref) {
		if (locked) return;
		auto& rp = references.trigon;
		rp.erase(
			std::remove_if(
				rp.begin(),
				rp.end(),
				[&](Reference::Trigon* e){return e == ref;}
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
		auto data = $fld loadBinaryFile(path);
		if (!data.size()) throw Error::FailedAction("File does not exist or is empty! (" + path + ")!");
		extend((RawVertex*)&data[0], data.size() / sizeof(RawVertex));
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

	void saveToFile(std::string const& path) {
		bakeAndLock();
		$fld saveBinaryFile(path, vertices, vertexCount);
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

	void saveToFile(std::string const& path) {
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
	auto data = FileLoader::loadBinaryFile(path);/*
	if (data.empty()) {
		data = FileLoader::loadBinaryFile("sys/models/error.msbo");
		if(data.empty())
			throw Error::FailedAction("File does not exist or is empty! (" + path + ")!");
	}*/
	if (data.empty()) throw Error::FailedAction("File does not exist or is empty! (" + path + ")!");
	return new Renderable((RawVertex*)&data[0], data.size() / sizeof(RawVertex));
}

#warning Unimplemented Function: 'loadObjectFromGLTFFile'
[[unavailable("Unimplemented!")]]
Renderable* loadObjectFromGLTFFile(string const& path) {return nullptr;}

#warning Unfinished Function: 'loadObjectFromDefinitionFile'
[[unavailable("Unfinished!")]]
Renderable* loadObjectDefinitionFile(string const& path) {
	// Vertex & Component data strings
	String vertString, compString;
	// Do file processing here...
	// Check if important data is not empty
	if (vertString.empty()) throw Error::InvalidValue("Missing vertex data!");
	if (compString.empty()) throw Error::InvalidValue("Missing component data!");
	// Vertex map
	Drawer::VertexMap vm;
	// Component list in order they appear
	vector<String> components = Helper::splitString(compString, ',');
	// Decoded vertex data
	String vdata = Decoder::fromBase64(vertString);
	// Check if there are no missing vertices
	if ((vdata.size() / (sizeof(float) * components.size())) % 3 != 0)
		throw Error::InvalidValue("Improper/incomplete vertex data!");
	// Get pointer to data
	float* rawdata = (float*)vdata.data();
	// Current vertex and component being accessed
	size_t vertex = 0, component = 0;
	// Resulting vertices
	vector<RawVertex> vertices;
	// Loop time
	while (component < vertices.size() / sizeof(float)) {
		vm = Drawer::baseVertexMap;
		for (auto& c : components)
			vm[c] = rawdata[component++];
		vertices.push_back(Drawer::toRawVertex(vm));
	}
	// Return new renderable object
	return new Renderable(vertices.data(), vertices.size());
}
