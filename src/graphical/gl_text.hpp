struct FontData {
	Texture2D*	texture = nullptr;
	Vector2		frame = Vector2(16);
};

struct TextData {
	std::string	text;
	Vector4		color = Color::WHITE;
	size_t		columns = 40;
};

typedef std::function<Vertex(Vertex,size_t,size_t,char)> TextEffectFunc;

#define $teffect(aA,aB,aC,aD)	(Vertex aA, size_t aB, size_t aC, char aD) -> Vertex
#define $texteffect				$teffect(v, lidx, vidx, c)

struct TextEffect {
	TextEffectFunc effect = [] $texteffect {return v;};
	bool enabled = false;
};
// TODO: Make this work
class NumberLabel {
public:
	NumberLabel(size_t layer = 0, bool manual = false) {
		if(!manual)
			Drawer::layers.addObject(&render, layer);
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		onCreate();
		manualMode = manual;
	}

	~NumberLabel() {
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

	NumberLabel* setManual() {
		if(!manualMode)
			Drawer::layers.removeFromAll(&render);
		manualMode = true;
		return this;
	}

	NumberLabel* setAuto(size_t renderLayer) {
		if(manualMode)
			Drawer::layers.addObject(&render, renderLayer);
		manualMode = false;
		return this;
	}

	NumberLabel* setRenderLayer(size_t renderLayer) {
		Drawer::layers.removeFromAll(&render);
		Drawer::layers.addObject(&render, renderLayer);
		manualMode = false;
		return this;
	}

	NumberLabel* addToRenderLayer(size_t renderLayer) {
		Drawer::layers.addObject(&render, renderLayer);
		manualMode = false;
		return this;
	}

	NumberLabel* removeFromRenderLayer(size_t renderLayer) {
		Drawer::layers.removeFromGroup(&render, renderLayer);
		if (!Drawer::layers.getGroups(&render).size())
			manualMode = true;
		return this;
	}
	#define CHARACTER_START 0x20
	#define CHARACTER_END	0xFF
	#define CHARACTER_RANGE	CHARACTER_END - CHARACTER_START
	/// Renders the object to the screen.
	DrawFunc render = $func() {
		// If no font selected or not active, return
		if (!(font.texture && active)) return;
		// Call onDrawBegin function
		onDrawBegin();
		// Get character count
		size_t charCount = Math::digitCount(number);
		// Create Intermediary Vertex Buffer (IVB) to be displayed on screen
		RawVertex* verts = new RawVertex[charCount * 6];
		// Get transformation matrix
		actorMatrix = VecMath::asGLMMatrix(local);
		// Text Character Vertex
		Vertex letter[4] = {
			Vertex(Vector3(0, 0, 0)),
			Vertex(Vector3(1, 0, 0)),
			Vertex(Vector3(0, -1, 0)),
			Vertex(Vector3(1, -1, 0))
		};
		// Text Character UV
		Vector2 luv[4] = {
			Vector2(0,1) / font.frame,
			Vector2(1,1) / font.frame,
			Vector2(0,2) / font.frame,
			Vector2(1,2) / font.frame,
		};
		// Set text color
//		#pragma GCC unroll 4
		for $each(v, letter) {
			v.color = color;
		}/*
		// Copy data to IVB
		size_t i = 0;
		for $ssrange(n, 0, charCount) {
			unsigned int uvi = 0;
			unsigned int lidx = Math::digit(number, charCount-n);
			// Loop through each vertex and..
			for $each(v, letter) {
				// Increment letter's position
				v.position.x += 1;
				// Decipher which digit to use
				v.uv = luv[uvi] + Vector2(lidx / font.frame.x, 0);
				// Increment UV index
				uvi++;
			}
			if (textEffect.enabled) {
				verts[i+0] = toRawVertex(textEffect.effect(letter[0],i/6,0,'0'+lidx));
				verts[i+1] = toRawVertex(textEffect.effect(letter[1],i/6,1,'0'+lidx));
				verts[i+2] = toRawVertex(textEffect.effect(letter[2],i/6,2,'0'+lidx));
				verts[i+3] = toRawVertex(textEffect.effect(letter[1],i/6,1,'0'+lidx));
				verts[i+4] = toRawVertex(textEffect.effect(letter[2],i/6,2,'0'+lidx));
				verts[i+5] = toRawVertex(textEffect.effect(letter[3],i/6,3,'0'+lidx));
			} else {
				verts[i+0] = toRawVertex(letter[0]);
				verts[i+1] = toRawVertex(letter[1]);
				verts[i+2] = toRawVertex(letter[2]);
				verts[i+3] = toRawVertex(letter[1]);
				verts[i+4] = toRawVertex(letter[2]);
				verts[i+5] = toRawVertex(letter[3]);
			}
			i += 6;
		}*/
		verts[0] = toRawVertex(letter[0]);
		verts[1] = toRawVertex(letter[1]);
		verts[2] = toRawVertex(letter[2]);
		verts[3] = toRawVertex(letter[1]);
		verts[4] = toRawVertex(letter[2]);
		verts[5] = toRawVertex(letter[3]);
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

	FontData	font;
	TextEffect	textEffect;
	size_t		number;
	Vector4		color = Color::WHITE;

	Transform3D local;
private:
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
		//$mainshader["texture2D"](29);
		//font.texture->enable(29);
		$mainshader["world"](Scene::world);
		$mainshader["camera"](camera);
		$mainshader["projection"](projection);
		$mainshader["actor"](actorMatrix);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	};
};

/*

class Label {
public:
	Label(size_t layer = 0, bool manual = false) {
		if(!manual)
			Drawer::layers.addObject(&render, layer);
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		onCreate();
		manualMode = manual;
	}

	~Label() {
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

	Label* setManual() {
		if(!manualMode)
			Drawer::layers.removeFromAll(&render);
		manualMode = true;
		return this;
	}

	Label* setAuto(size_t renderLayer) {
		if(manualMode)
			Drawer::layers.addObject(&render, renderLayer);
		manualMode = false;
		return this;
	}

	Label* setRenderLayer(size_t renderLayer) {
		Drawer::layers.removeFromAll(&render);
		Drawer::layers.addObject(&render, renderLayer);
		manualMode = false;
		return this;
	}

	Label* addToRenderLayer(size_t renderLayer) {
		Drawer::layers.addObject(&render, renderLayer);
		manualMode = false;
		return this;
	}

	Label* removeFromRenderLayer(size_t renderLayer) {
		Drawer::layers.removeFromGroup(&render, renderLayer);
		if (!Drawer::layers.getGroups(&render).size())
			manualMode = true;
		return this;
	}
	#define CHARACTER_START 0x20
	#define CHARACTER_END	0xFF
	#define CHARACTER_RANGE	CHARACTER_END - CHARACTER_START
	/// Renders the object to the screen.
	DrawFunc render = $func() {
		// If no font selected or not active, return
		if (!(font.texture && active)) return;
		// Call onDrawBegin function
		onDrawBegin();
		// Get character count
		size_t charCount = text.text.size();
		// Create Intermediary Vertex Buffer (IVB) to be displayed on screen
		RawVertex* verts = new RawVertex[charCount * 6];
		// Get transformation matrix
		actorMatrix = VecMath::asGLMMatrix(local);
		// Text Character Vertex
		Vertex letter[4] = {
			Vertex(Vector3(0)),
			Vertex(Vector3(font.scale.x, 0)),
			Vertex(Vector3(0, font.scale.y)),
			Vertex(Vector3(font.scale.x, font.scale.y))
		};
		// Text Character UV
		Vector2 luv[4] = {
			Vector2(0),
			Vector2(1,0) / font.frame,
			Vector2(0,1) / font.frame,
			Vector2(1) / font.frame,
		};
		// Set text color
//		#pragma GCC unroll 4
		for $each(v, letter) {
			v.color = text.color;
		}
		// Copy data to IVB
		size_t i = 0;
		for (char c: text.text) {
			// If text end, break
			if (c == '\0') break;
			// Get letter index
			unsigned char lidx = (int)c;
			// If past visible character range, skip letter;
			if (c > CHARACTER_END)		{i += 6; continue;}
			if (c < CHARACTER_START)	{i += 6; continue;}
			lidx -= CHARACTER_START;
			// The UV index used
			unsigned char uvi = 0;
			// Loop through each vertex and..
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
			if (textEffect.enabled) {
				verts[i+0] = toRawVertex(textEffect.effect(letter[0],i/6,0,c));
				verts[i+1] = toRawVertex(textEffect.effect(letter[1],i/6,1,c));
				verts[i+2] = toRawVertex(textEffect.effect(letter[2],i/6,2,c));
				verts[i+3] = toRawVertex(textEffect.effect(letter[1],i/6,1,c));
				verts[i+4] = toRawVertex(textEffect.effect(letter[2],i/6,2,c));
				verts[i+5] = toRawVertex(textEffect.effect(letter[3],i/6,3,c));
			} else {
				verts[i+0] = toRawVertex(letter[0]);
				verts[i+1] = toRawVertex(letter[1]);
				verts[i+2] = toRawVertex(letter[2]);
				verts[i+3] = toRawVertex(letter[1]);
				verts[i+4] = toRawVertex(letter[2]);
				verts[i+5] = toRawVertex(letter[3]);
			}
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

	FontData	font;
	TextData	text;
	TextEffect	textEffect;

	Transform3D local;
private:
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
*/
#define $txt $rdt Text::
