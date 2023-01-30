/*
Do not touch this class. Please.
*/

class Renderable: public Base::DrawableObject {
public:
	Renderable(size_t layer = 0, bool manual = false): DrawableObject(layer, manual){}

	Renderable(
		vector<Triangle*> triangles,
		size_t layer = 0,
		bool manual = false
	): DrawableObject(layer, manual) {
		this->triangles = triangles;
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
	template <$derived(Reference::Plane) T>
	T* createReference() {
		if (locked) throw std::runtime_error("Renderable object is locked!");
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
	template <$derived(Reference::Trigon) T>
	T* createReference() {
		if (locked) throw std::runtime_error("Renderable object is locked!");
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
	template <$derived(Reference::Plane) T>
	inline T* getReference(size_t index) {
		if (locked) throw std::runtime_error("Renderable object is locked!");
		return (T*)references.plane[index];
	}

	template <$derived(Reference::Trigon) T>
	inline T* getReference(size_t index) {
		if (locked) throw std::runtime_error("Renderable object is locked!");
		return (T*)references.trigon[index];
	}

	/**
	* Deletes a reference bound to this object.
	*
	* More specifically, it removes the reference
	* and the triangles associated to it.
	* It also deletes the reference.
	*/
	template <$derived(Reference::Plane) T>
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

	template <$derived(Reference::Trigon) T>
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
	template <$derived(Reference::Plane) T>
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

	template <$derived(Reference::Trigon) T>
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

	void bakeAndLock(RawVertex* vertices, size_t size) {
		if (locked) return;
		locked	= true;
		baked	= true;
		if (vertices == nullptr || size == 0)
			throw std::runtime_error("No vertices were provided!");
		if (this->vertices)
			delete[] this->vertices;
		this->vertices = new RawVertex[size];
		for $ssrange(i, 0, size) {
			this->vertices[i] = vertices[i];
		}
		vertexCount = size;
		clearData();
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

	void saveToFile(std::string path) {
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
		#ifdef $_PARALLEL_RENDERING
		if (!references.plane.empty())
			$peach(plane, references.plane, references.plane.size(), {plane->transform();});
		if (!references.trigon.empty())
			$peach(tg, references.trigon, references.trigon.size(), {tg->transform();});
		if(references.plane.size() == 1) references.plane[0] -> transform();
		if(references.trigon.size() == 1) references.trigon[0] -> transform();
		#else
		for (auto& plane: references.plane)	plane->transform();
		for (auto& tg: references.trigon)	tg->transform();
		#endif
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
		#ifdef $_PARALLEL_RENDERING
		if (!references.plane.empty())
			$peach(plane, references.plane, references.plane.size(), {plane->reset();});
		if (!references.trigon.empty())
			$peach(tg, references.trigon, references.trigon.size(), {tg->reset();});
		if(references.plane.size() == 1) references.plane[0] -> reset();
		if(references.trigon.size() == 1) references.trigon[0] -> reset();
		#else
		for (auto& plane: references.plane)	plane->reset();
		for (auto& tg: references.trigon)	tg->reset();
		#endif
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

Renderable* loadObjectFromBinaryFile(string path) {
	auto* object = new Renderable();
	auto data = $fld loadBinaryFile(path);
	if (!data.size()) throw runtime_error("File does not exist or is empty! (" + path + ")!");
	object->bakeAndLock((RawVertex*)&data[0], data.size() / sizeof(RawVertex));
	return object;
}

#warning Unimplemented Function: 'loadObjectFromGLTFFile'
Renderable* loadObjectFromGLTFFile(string path) {
	throw runtime_error("Unimplemented function 'loadObjectFromGLTFFile' !");
}
