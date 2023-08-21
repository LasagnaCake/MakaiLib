class Drawable {
public:
	Drawable(size_t layer = 0, bool manual = false) {
		if(!manual) setAuto(layer);
		manualMode = manual;
	}

	virtual ~Drawable() {
		$debug("Removing from rendering layers...");
		if(!manualMode)
			Drawer::layers.removeFromAll(&render);
		$debug("Finalizing...\n");
	}

	/// Called before rendering to screen.
	virtual void onDrawBegin() {}
	/// Called after rendering to screen.
	virtual void onDrawEnd() {};

	Drawable* setManual() {
		if(!manualMode)
			Drawer::layers.removeFromAll(&render);
		manualMode = true;
		return this;
	}

	Drawable* setAuto(size_t renderLayer) {
		if(manualMode)
			Drawer::layers.addObject(&render, renderLayer);
		manualMode = false;
		return this;
	}

	Drawable* setRenderLayer(size_t renderLayer) {
		Drawer::layers.removeFromAll(&render);
		Drawer::layers.addObject(&render, renderLayer);
		manualMode = false;
		return this;
	}

	Drawable* addToRenderLayer(size_t renderLayer) {
		Drawer::layers.addObject(&render, renderLayer);
		manualMode = false;
		return this;
	}

	Drawable* removeFromRenderLayer(size_t renderLayer) {
		Drawer::layers.removeFromGroup(&render, renderLayer);
		if (!Drawer::layers.getGroups(&render).size())
			manualMode = true;
		return this;
	}

	/// Renders the object to the screen.
	DrawFunc render = $func() {
		// If not active, return
		if (!active) return;
		// Call onDrawBegin function
		onDrawBegin();
		// Draw object
		draw();
		// Call onDrawEnd function
		onDrawEnd();
	};

	bool active	= true;

protected:
	virtual void draw() {}

private:

	/// Whether manually rendering or not.
	bool manualMode = false;
};

class DrawableObject: public Drawable {
public:
	DrawableObject(size_t layer = 0, bool manual = false): Drawable(layer, manual){
		$debug("Drawable object created!");
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
	}

	virtual ~DrawableObject() {
		$debug("Deleting buffers...");
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}

	virtual void draw() {}

	void clearInstances() {
		material.instances.clear();
		material.instances.push_back(Vector3(0, 0, 0));
	}

	$mat ObjectMaterial	material;
	Transform3D			trans;

protected:
	void display(RawVertex* vertices, size_t count, GLuint mode = GL_TRIANGLES) {
		// If no instances, add one
		if (material.instances.empty())
			material.instances.push_back(Vector3(0, 0, 0));
		// Set VBO as active
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		// Copy IVB to VBO
		glBufferData(
			GL_ARRAY_BUFFER,
			count * RAW_VERTEX_BYTE_SIZE,
			vertices,
			GL_STATIC_DRAW
		);
		// Set VAO as active
		glBindVertexArray(vao);
		// Define vertex data in VBO
		Drawer::setVertexAttributes();
		// Set VAO as active
		glBindVertexArray(vao);
		// Enable attribute pointers
		Drawer::enableVertexAttributes();
		// If face culling is enabled...
		if (material.culling != GL_FRONT_AND_BACK) {
			// Set culling face
			glEnable(GL_CULL_FACE);
			glCullFace(material.culling);
		}
		// Draw object to screen
		glPolygonMode(GL_FRONT_AND_BACK, material.fill);
		glDrawArraysInstanced(mode, 0, count, material.instances.size());
		// Disable culling
		glDisable(GL_CULL_FACE);
		// Disable attributes
		Drawer::disableVertexAttributes();
		// Unbind vertex array
		glBindVertexArray(0);
	}

	void setDefaultShader() {
		// Render with basic shader
		$mainshader();
		// Get transformation matrices
		glm::mat4
			actor		= VecMath::asGLMMatrix(trans),
			camera		= Scene::camera.matrix(),
			projection	= Scene::camera.perspective();
		// Set shader data
		$mat setMaterial($mainshader, material);
		// Set transformation matrices
		$mainshader["world"](Scene::world);
		$mainshader["camera"](camera);
		$mainshader["projection"](projection);
		$mainshader["actor"](actor);
	}

	GLuint vao, vbo;
};
