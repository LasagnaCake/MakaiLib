/*
Do not touch this class. Please.
*/

class Renderable: public Base::Drawable {
public:
	Renderable(size_t layer = 0, bool manual = false): Drawable(layer, manual){
		$debug("\nNew Renderable!");
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
	}

	Renderable(vector<RawVertex> vertices, size_t layer = 0, bool manual = false)
	: Renderable(layer, manual) {
		this->vertices = vertices;
	}

	virtual ~Renderable() {
		Drawable::~Drawable();
		$debug(references.plane.size());
		$debug("Deleting buffers...");
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
		$debug("Deleting references...");
		if (!references.plane.empty())
			for (auto pr: references.plane)
				delete pr;
		if (!references.triangle.empty())
			for (auto pr: references.triangle)
				delete pr;
		references.plane.clear();
		references.triangle.clear();
		$debug("Deleting vertices...");
		vertices.clear();
		$debug("Killing renderable object...");
	}

	/// Creates a reference and binds it to this object.
	template <$derived(Reference::Plane) T>
	T* createReference() {
		for (auto i = 0; i < 6; i++)
			vertices.push_back(RawVertex{});
		// Create reference
		T* plane = new T(vertices, vertices.size()-6);
		/*T* plane = new T(
			new RawVertex{},
			new RawVertex{},
			new RawVertex{},
			new RawVertex{},
			new RawVertex{},
			new RawVertex{}
		);
		$debugp("Planes: ");
		$debug(references.plane.size());
		$debugp("Vertices: ");
		$debug(vertices.size());
		$debug("");*/
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
		// Add to reference list
		references.plane.push_back(plane);
		// Set destructor function
		plane->onDestroy	= [this, plane](){this->removeReference<T>(plane);};
		plane->onUnbind		= [this, plane](){this->unbindReference<T>(plane);};
		// return plane
		return plane;
	}
	template <$derived(Reference::Triangle) T>
	T* createReference() {
		for (auto i = 0; i < 3; i++)
			vertices.push_back(RawVertex{});
		// Create reference
		T* tg = new T(vertices, vertices.size()-3);
		// Setup plane
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
		// Add to reference list
		references.triangle.push_back(tg);
		// Set destructor function
		tg->onDestroy	= [this, tg](){this->removeReference<T>(tg);};
		tg->onUnbind	= [this, tg](){this->unbindReference<T>(tg);};
		// return triangle
		return tg;
	}

	/// Gets a reference bound to this object by index.
	template <$derived(Reference::Plane) T>
	inline T* getReference(size_t index) {
		return (T*)references.plane[index];
	}

	template <$derived(Reference::Triangle) T>
	inline T* getReference(size_t index) {
		return (T*)references.triangle[index];
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
		auto verts = ref->getBoundVertices();
		vertices.erase(
			std::remove_if(
				vertices.begin(),
				vertices.end(),
				[=](RawVertex& e){
					return
						(&e == verts[0])
					||	(&e == verts[1])
					||	(&e == verts[2])
					||	(&e == verts[3])
					||	(&e == verts[4])
					||	(&e == verts[5]);
				}
			),
			vertices.end()
		);
		unbindReference<T>(ref);
	}

	template <$derived(Reference::Triangle) T>
	void removeReference(T* ref) {
		auto verts = ref->getBoundVertices();
		vertices.erase(
			std::remove_if(
				vertices.begin(),
				vertices.end(),
				[=](RawVertex& e){
					return
						(&e == verts[0])
					||	(&e == verts[1])
					||	(&e == verts[2]);
				}
			),
			vertices.end()
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

	template <$derived(Reference::Triangle) T>
	void unbindReference(T* ref) {
		auto& rt = references.triangle;
		rt.erase(
			std::remove_if(
				rt.begin(),
				rt.end(),
				[&](Reference::Triangle* e){return e == ref;}
			),
			rt.end()
		);
		delete ref;
	}

	vector<RawVertex> vertices;

	Material::ObjectMaterial material;

	Transform3D trans;

private:
	void draw() override {
		// If no vertices exist, return
		if (!vertices.size()) return;
		// Transform references (if applicable)
		for (auto& plane: references.plane)	plane->transform();
		for (auto& tg: references.triangle)	tg->transform();
		//$debug("Transformed!");
		// Get transformation matrix
		actorMatrix = VecMath::asGLMMatrix(trans);
		// Set VBO as active
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		// Copy vertices to VBO
		glBufferData(
			GL_ARRAY_BUFFER,
			vertices.size() * RAW_VERTEX_BYTE_SIZE,
			vertices.data(),
			GL_STATIC_DRAW
		);
		// De-transform references (if applicable)
		for (auto& plane: references.plane)	plane->reset();
		for (auto& tg: references.triangle)	tg->reset();
		// Set VAO as active
		glBindVertexArray(vao);
		// Define vertex data in VBO
		Drawer::setVertexAttributes();
		// Set VAO as active
		glBindVertexArray(vao);
		// Enable attribute pointers
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		// Set polygon rendering mode
		glPolygonMode(material.culling, material.fill);
		// Draw object to screen
		display();
		// Disable attributes
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
		// Unbind vertex array
		glBindVertexArray(0);
	}

	/// List of references linked to this object.
	struct {
		vector<Reference::Plane*>		plane;
		vector<Reference::Triangle*>	triangle;
	} references;

	glm::mat4x4 actorMatrix;

	/// The object's Vertex Array Object (VAO).
	GLuint vao;

	/// The object's Vertex Buffer Object (VBO).
	GLuint vbo;

	/// The amount of vertices this object has.
	size_t vertexCount = 0;

	void display() {
		// Render with basic shader
		Shader::defaultShader();
		glm::mat4 camera = Scene::camera.matrix();
		glm::mat4 projection = Scene::camera.perspective();
		// Set shader data
		$mat setMaterial($mainshader, material);
		// Matrices
		$mainshader["world"](Scene::world);
		$mainshader["camera"](camera);
		$mainshader["projection"](projection);
		$mainshader["actor"](actorMatrix);
		// Rendering
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	};
};
