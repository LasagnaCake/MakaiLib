/*
Do not touch this class. Please.
*/

class Renderable: public Base::Drawable {
public:
	Renderable(size_t layer = 0, bool manual = false): Drawable(layer, manual){
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
	}

	Renderable(vector<Triangle*> triangles, size_t layer = 0, bool manual = false)
	: Renderable(layer, manual) {
		this->triangles = triangles;
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
		if (!references.trigon.empty())
			for (auto pr: references.trigon)
				delete pr;
		references.plane.clear();
		references.trigon.clear();
		$debug("Deleting triangles...");
		for (auto t: triangles) delete t;
		triangles.clear();
		$debug("Killing renderable object...");
	}

	/// Creates a reference and binds it to this object.
	template <$derived(Reference::Plane) T>
	T* createReference() {
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
		Triangle* tris[1] = {new Triangle()};
		// Add triangles
		triangles.push_back(tris[0]);
		// Create reference
		T* tg = new T(tris);
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
		return (T*)references.plane[index];
	}

	template <$derived(Reference::Trigon) T>
	inline T* getReference(size_t index) {
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

	vector<Triangle*> triangles;

	struct {
		struct {
			bool enabled		= false;
			Texture2D* image	= nullptr;
			GLubyte id			= 0;
		} texture;
		GLuint culling		= GL_FRONT_AND_BACK;
		GLuint fill			= GL_FILL;
	} params;

	Transform3D trans;

private:
	void draw() override {
		// If no triangles exist, return
		if (!triangles.size()) return;
		// Transform references (if applicable)
		for (auto& plane: references.plane)	plane->transform();
		for (auto& tg: references.trigon)	tg->transform();
		// Get vertex count
		vertexCount = triangles.size() * 3;
		// Create Intermediary Vertex Buffer (IVB) to be displayed on screen
		RawVertex* verts = new RawVertex[(vertexCount)];
		// Get transformation matrix
		actorMatrix = VecMath::asGLMMatrix(trans);
		// Copy data to IVB
		size_t i = 0;
		for (auto& t: triangles) {
			verts[i]	= t->verts[0];
			verts[i+1]	= t->verts[1];
			verts[i+2]	= t->verts[2];
			i += 3;
		}
		// Set VBO as active
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		// Copy IVB to VBO
		glBufferData(
			GL_ARRAY_BUFFER,
			vertexCount * RAW_VERTEX_BYTE_SIZE,
			verts,
			GL_STATIC_DRAW
		);
		// Delete IVB, since it is no longer necessary
		delete [] verts;
		// De-transform references (if applicable)
		for (auto& plane: references.plane)	plane->reset();
		for (auto& tg: references.trigon)	tg->reset();
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
		glPolygonMode(params.culling, params.fill);
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
		vector<Reference::Plane*>	plane;
		vector<Reference::Trigon*>	trigon;
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
		if (params.texture.image && params.texture.enabled) {
			$mainshader["textured"](params.texture.enabled);
			$mainshader["texture2D"](params.texture.id);
			params.texture.image->enable(params.texture.id);
		}
		$mainshader["world"](Scene::world);
		$mainshader["camera"](camera);
		$mainshader["projection"](projection);
		$mainshader["actor"](actorMatrix);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	};
};
