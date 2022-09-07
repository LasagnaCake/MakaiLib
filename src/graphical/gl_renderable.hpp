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
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
		for (auto t: triangles)
			delete t;
		for (auto pr: references.plane)
			delete pr;
		if(!manualMode)
			Drawer::layers.removeFromAll(&render);
	}

	/// Called on creation.
	virtual void onCreate()	{}
	/// Called before rendering to screen.
	virtual void onRender()	{}
	/// Called on deletion.
	virtual void onDelete()	{}

	void setManual() {
		if(!manualMode)
			Drawer::layers.removeFromAll(&render);
		manualMode = true;
	}

	void setAuto(size_t renderLayer) {
		if(manualMode)
			Drawer::layers.addObject(&render, renderLayer);
		manualMode = false;
	}

	void setRenderLayer(size_t renderLayer) {
		Drawer::layers.removeFromAll(&render);
		Drawer::layers.addObject(&render, renderLayer);
		manualMode = false;
	}

	PlaneReference* createPlaneReference() {
		Triangle* tris = new Triangle[2];
		// Add triangles
		triangles.push_back(&tris[0]);
		triangles.push_back(&tris[1]);
		// Create reference
		PlaneReference* plane = new PlaneReference(tris);
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

	AnimatedPlaneReference* createAnimatedPlaneReference() {
		Triangle* tris = new Triangle[2];
		// Add triangles
		triangles.push_back(&tris[0]);
		triangles.push_back(&tris[1]);
		// Create reference
		AnimatedPlaneReference* plane = new AnimatedPlaneReference(tris);
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

	/// Renders the object to the screen.
	DrawFunc render = $func() {
		// If not active, return
		if (!params.active) return;
		// If no triangles exist, return
		if (!triangles.size()) return;
		// Call onRender function
		onRender();
		// Transform references (if applicable)
		for (auto plane: references.plane) plane->transform();
		// Get vertex count
		vertexCount = triangles.size() * 3;
		// Create Intermediary Vertex Buffer (IVB) to be displayed on screen
		RawVertex* verts = new RawVertex[(vertexCount)];
		// Get transformation
		Transform3D absolute(
			SRP_TRANSFORM(transform.local.position, transform.global),
			transform.local.rotation + transform.global.rotation,
			transform.local.scale * transform.global.scale
		);
		// Copy data to IVB
		size_t i = 0;
		for (auto t: triangles) {
			auto tri = (*t)
				.transformed(absolute);
			verts[i]	= toRawVertex(tri.verts[0]);
			verts[i+1]	= toRawVertex(tri.verts[1]);
			verts[i+2]	= toRawVertex(tri.verts[2]);
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
	};

	Shader::ShaderList shaders;

	vector<Triangle*> triangles;

	struct {
		bool active		= true;
		bool textured	= false;
		GLuint culling	= GL_FRONT_AND_BACK;
		GLuint fill		= GL_FILL;
	} params;

	struct {
		Transform3D global;
		Transform3D local;
	} transform;
private:
	/// List of references linked to this object.
	struct {
		vector<PlaneReference*> plane;
	} references;

	/// Whether manually rendering or not.
	bool manualMode = false;

	/// The object's Vertex Array Object (VAO).
	GLuint vao;

	/// The object's Vertex Buffer Object (VBO).
	GLuint vbo;

	/// The amount of vertices this object has.
	size_t vertexCount;

	void draw() {
		// Render with basic shader
		Shader::defaultShader();
		glm::mat4 camera = Scene::camera.matrix();
		glm::mat4 projection = Scene::camera.perspective();
		Shader::defaultShader["textured"](params.textured);
		Shader::defaultShader["world"](Scene::world);
		Shader::defaultShader["camera"](camera);
		Shader::defaultShader["projection"](projection);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		// Render object passes, if any
		if(shaders.size())
			for (auto s : shaders) {
				// Enable shader
				auto shader = (*s);
				shader();
				// Set prerequisites
				shader["textured"](params.textured);
				shader["world"](Scene::world);
				shader["camera"](camera);
				shader["projection"](projection);
				// Draw object
				glDrawArrays(GL_TRIANGLES, 0, vertexCount);
			}
	};
};
