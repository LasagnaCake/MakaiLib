struct FontData {
	Texture2D*	texture = nullptr;
	Vector2		frame;
	Vector2		scale;
};

struct TextData {
	std::string	text;
	Vector4		color;
	size_t		columns;
};

class RenderableText {
public:
	RenderableText(size_t layer = 0, bool manual = false) {
		if(!manual)
			Drawer::layers.addObject(&render, layer);
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		onCreate();
		manualMode = manual;
	}

	~RenderableText() {
		onDelete();
		if(!manualMode)
			Drawer::layers.removeFromAll(&render);
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}

	/// Called on creation.
	virtual void onCreate()	{}
	/// Called before rendering to screen.
	virtual void onDrawBegin()	{}
	/// Called after rendering to screen.
	virtual void onDrawEnd()	{}
	/// Called on deletion.
	virtual void onDelete()	{}

	RenderableText* setManual() {
		if(!manualMode)
			Drawer::layers.removeFromAll(&render);
		manualMode = true;
		return this;
	}

	RenderableText* setAuto(size_t renderLayer) {
		if(manualMode)
			Drawer::layers.addObject(&render, renderLayer);
		manualMode = false;
		return this;
	}

	RenderableText* setRenderLayer(size_t renderLayer) {
		Drawer::layers.removeFromAll(&render);
		Drawer::layers.addObject(&render, renderLayer);
		manualMode = false;
		return this;
	}

	RenderableText* addToRenderLayer(size_t renderLayer) {
		Drawer::layers.addObject(&render, renderLayer);
		manualMode = false;
		return this;
	}

	RenderableText* removeFromRenderLayer(size_t renderLayer) {
		Drawer::layers.removeFromGroup(&render, renderLayer);
		if (!Drawer::layers.getGroups(&render).size())
			manualMode = true;
		return this;
	}
	#define CHARACTER_START 0x20
	#define CHARACTER_END	0x7F
	#define CHARACTER_RANGE	CHARACTER_END - CHARACTER_START
	/// Renders the object to the screen.
	DrawFunc render = $func() {
		// If no font selected or not active, return
		if (!(font.texture && active)) return;
		// Call onDrawBegin function
		onDrawBegin();
		// Create Intermediary Vertex Buffer (IVB) to be displayed on screen
		RawVertex* verts = new RawVertex[text.text.size() * 6];
		// Get transformation matrix
		actorMatrix = VecMath::asGLMMatrix(local);
		// Text Character Vertex
		Vertex letter[4] = {
			Vertex(Vector3(0)),
			Vertex(Vector3(font.scale.x, 0)),
			Vertex(Vector3(0, -font.scale.y)),
			Vertex(Vector3(font.scale.x, -font.scale.y))
		};
		// Text Character UV
		Vector2 luv[4] = {
			Vector2(0),
			Vector2(1,0) / font.frame,
			Vector2(0,1) / font.frame,
			Vector2(1) / font.frame,
		};
		// Set text color
		for $each(v, letter) {
			v.color = text.color;
		}
		// Copy data to IVB
		size_t i = 0;
		for (char c: text.text) {
			// If text end, break
			if (c == '\0') break;
			// Get letter index
			unsigned char lidx = (unsigned char)c - CHARACTER_START;
			if (c > CHARACTER_RANGE) {
				i += 6;continue;
			}
			// The UV index used
			unsigned char uvi = 0;
			// for each vertex...
			for $each(v, letter) {
				// If carriage return, newline and do next
				if (c == '\n') {
					v.position.x = 0;
					v.position.y -= font.scale.y;
					continue;
				}
				// Increment letter's position
				v.position.x += font.scale.x;
				// Decipher which letter to use
				v.uv = luv[uvi] + Vector2(
					Math::wmax((float)lidx, font.frame.x),
					lidx / (int)font.frame.y
				);
				// If past column size, carriage return
				if ((letter[0].position.x / font.scale.x) > text.columns) {
					v.position.x = 0;
					v.position.y -= font.scale.y;
				}
				// Increment UV index
				uvi++;
			}
			verts[i+0]	= toRawVertex(letter[0]);
			verts[i+1]	= toRawVertex(letter[1]);
			verts[i+2]	= toRawVertex(letter[2]);
			verts[i+3]	= toRawVertex(letter[1]);
			verts[i+4]	= toRawVertex(letter[2]);
			verts[i+5]	= toRawVertex(letter[3]);
			i += 6;
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
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

	bool active	= true;

	FontData font;
	TextData text;

	Transform3D local;
private:
	/// List of references linked to this object.
	struct {
		vector<Reference::Plane*> plane;
	} references;

	glm::mat4x4 actorMatrix;

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
		$mainshader["textured"](true);
		$mainshader["texture2D"](30);
		font.texture->enable(30);
		$mainshader["world"](Scene::world);
		$mainshader["camera"](camera);
		$mainshader["projection"](projection);
		$mainshader["actor"](actorMatrix);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	};
};
