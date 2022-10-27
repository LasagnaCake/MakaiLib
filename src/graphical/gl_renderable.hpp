/*
Do not touch this class.

If your triangles are not rendering properly, restart your program.
1: If it changes, that's normal. It just does that from time to time.
2: If it stays the same, probably a transparency issue.
*/

class Renderable {
public:
	Renderable(size_t layer = 0, bool manual = false) {
		if(!manual)
			Drawer::layers.addObject(&render, layer);
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		onCreate();
		manualMode = manual;
	}

	Renderable(vector<Triangle*> triangles, size_t layer = 0, bool manual = false)
	: Renderable(layer, manual) {
		this->triangles = triangles;
	}

	~Renderable() {
		onDelete();
		$debug("Removing from render layers...");
		if(!manualMode)
			Drawer::layers.removeFromAll(&render);
		$debug("Deleting buffers...");
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
		$debug("Deleting references...");
		for (auto pr: references.plane)
			delete pr;
		references.plane.clear();
		$debug("Deleting triangles...");
		for (auto t: triangles) {
			// This causes an error for some reason
			delete t;
		}
		triangles.clear();
		$debug("Killing renderable object...");
	}

	/// Called on creation.
	virtual void onCreate()	{}
	/// Called before rendering to screen.
	virtual void onDrawBegin()	{}
	/// Called after rendering to screen.
	virtual void onDrawEnd()	{}
	/// Called on deletion.
	virtual void onDelete()	{}

	Renderable* setManual() {
		if(!manualMode)
			Drawer::layers.removeFromAll(&render);
		manualMode = true;
		return this;
	}

	Renderable* setAuto(size_t renderLayer) {
		if(manualMode)
			Drawer::layers.addObject(&render, renderLayer);
		manualMode = false;
		return this;
	}

	Renderable* setRenderLayer(size_t renderLayer) {
		Drawer::layers.removeFromAll(&render);
		Drawer::layers.addObject(&render, renderLayer);
		manualMode = false;
		return this;
	}

	Renderable* addToRenderLayer(size_t renderLayer) {
		Drawer::layers.addObject(&render, renderLayer);
		manualMode = false;
		return this;
	}

	Renderable* removeFromRenderLayer(size_t renderLayer) {
		Drawer::layers.removeFromGroup(&render, renderLayer);
		if (!Drawer::layers.getGroups(&render).size())
			manualMode = true;
		return this;
	}

	/// Creates a reference and binds it to this object.
	template <
		class T,
		class = $isderivedof(T, Reference::Plane)
	>
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
		// return plane
		return plane;
	}

	/// Gets a reference bound to this object by index.
	template <
		class T,
		class = $isderivedof(T, Reference::Plane)
	>
	inline T* getReference(size_t index) {
		return (T*)references.plane[index];
	}

	/**
	* Deletes a reference bound to this object.
	*
	* More specifically, it removes the reference
	* and the triangles associated to it.
	* It also deletes the reference.
	*/
	template <
		class T,
		class = $isderivedof(T, Reference::Plane)
	>
	void removeReference(T* ref) {
		auto tris = ref->getBoundTriangles();
		triangles.erase(
			std::remove_if(
				triangles.begin(),
				triangles.end(),
				[&](T* e){return (e == tris[0]) || (e == tris[1]);}
			),
			triangles.end()
		);
		dereference(ref);
	}

	/**
	* Unbinds a reference bound to this object.
	*
	* More specifically, it removes the reference,
	* but keeps the triangles associated to it.
	* It also deletes the reference.
	*/
	template <
		class T,
		class = $isderivedof(T, Reference::Plane)
	>
	void unbindReference(T* ref) {
		auto& rp = references.plane;
		rp.erase(
			std::remove_if(
				rp.begin(),
				rp.end(),
				[&](T* e){return e == ref;}
			),
			rp.end()
		);
		delete ref;
	}

	vector<Triangle*> getSortedTriangles() {
		vector<Triangle*> tris = triangles;
		if (!params.textured) {
			tris.clear();
			vector<Triangle*> solid;
			vector<Triangle*> translucent;
			// Separate solids from translucents
			for $eachif(t, triangles, t->isSolid())
				solid.push_back(t);
			else
				translucent.push_back(t);
			//
			//std::reverse(translucent.begin(), translucent.end());
			//std::reverse(solid.begin(), solid.end());
			// Join back together, with translucents last
			tris = solid;
			for $each(t, translucent)
				tris.push_back(t);
		}
		return tris;
	}

	/// Renders the object to the screen.
	DrawFunc render = $func() {
		// If not active, return
		if (!params.active) return;
		// If no triangles exist, return
		if (!triangles.size()) return;
		// Call onDrawBegin function
		onDrawBegin();
		vector<Triangle*> tris = triangles;
		// If auto sort triangles, then do so
		if (params.autoSort) {
			if ((vertexCount != (triangles.size() * 3)) || !vertexCount)
				sortedTriangles = getSortedTriangles();
			tris = sortedTriangles;
		}
		// Transform references (if applicable)
		for (auto plane: references.plane) plane->transform();
		// Get vertex count
		vertexCount = tris.size() * 3;
		// Create Intermediary Vertex Buffer (IVB) to be displayed on screen
		RawVertex* verts = new RawVertex[(vertexCount)];
		// Get transformation matrix
		actorMatrix = VecMath::asGLMMatrix(trans);
		// Copy data to IVB
		size_t i = 0;
		for (auto t: tris) {
			verts[i]	= toRawVertex(t->verts[0]);
			verts[i+1]	= toRawVertex(t->verts[1]);
			verts[i+2]	= toRawVertex(t->verts[2]);
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
		for (auto plane: references.plane) plane->reset();
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
		draw();
		// Disable attributes
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
		// Unbind vertex array
		glBindVertexArray(0);
		// Call onDrawEnd function
		onDrawEnd();
	};

	vector<Triangle*> triangles;

	struct {
		bool active			= true;
		bool textured		= false;
		bool autoSort		= false;
		GLubyte texture		= 0;
		Texture2D* image	= nullptr;
		GLuint culling		= GL_FRONT_AND_BACK;
		GLuint fill			= GL_FILL;
	} params;

	Transform3D trans;
private:
	/// List of references linked to this object.
	struct {
		vector<Reference::Plane*> plane;
	} references;

	vector<Triangle*> sortedTriangles;

	glm::mat4x4 actorMatrix;

	/// Whether manually rendering or not.
	bool manualMode = false;

	/// The object's Vertex Array Object (VAO).
	GLuint vao;

	/// The object's Vertex Buffer Object (VBO).
	GLuint vbo;

	/// The amount of vertices this object has.
	size_t vertexCount = 0;

	void draw() {
		// Render with basic shader
		Shader::defaultShader();
		glm::mat4 camera = Scene::camera.matrix();
		glm::mat4 projection = Scene::camera.perspective();
		$mainshader["textured"](params.textured);
		$mainshader["texture2D"](params.texture);
		if (params.image)
			params.image->enable(params.texture);
		$mainshader["world"](Scene::world);
		$mainshader["camera"](camera);
		$mainshader["projection"](projection);
		$mainshader["actor"](actorMatrix);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	};
};
