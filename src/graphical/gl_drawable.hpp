class Drawable {
public:
	Drawable(size_t layer = 0, bool manual = false) {
		if(!manual)
			Drawer::layers.addObject(&render, layer);
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

// TODO: Somewhere this class is used causes 'Exit Status 0xC0000005'. Potentially related to 'Renderable' class
class DrawableObject: public Drawable {
public:
	DrawableObject(size_t layer = 0, bool manual = false): Drawable(layer, manual){
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
	}

	virtual ~DrawableObject() {
		$debug("Deleting buffers...");
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}

	virtual void draw() {}

	$mat ObjectMaterial	material;
	Transform3D			trans;

protected:
	void display(RawVertex* vertices, size_t count, GLuint mode = GL_TRIANGLES) {
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
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		// Set polygon rendering mode
		glPolygonMode(material.culling, material.fill);
		// Draw object to screen
		glDrawArrays(mode, 0, count);
		// Disable attributes
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
		// Unbind vertex array
		glBindVertexArray(0);
	}

	void setDefaultShader() {
		// Get transformation matrix
		actorMatrix = VecMath::asGLMMatrix(trans);
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
	}

	glm::mat4x4	actorMatrix;
	GLuint		vao, vbo;
};
