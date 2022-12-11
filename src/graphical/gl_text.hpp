/*class Font {
	Font() {}

	Font(String path) {
	}

	void create(String path) {
		if (created) return;
		created = true;
	}

	void destroy() {
		if (!created) return;
		created = false;
	}

private:
	bool created = false;
};*/

enum TextAlign {
	ALIGN_LEFT,
	ALIGN_RIGHT,
	ALIGN_CENTER
};

struct TextRect {
	size_t h = 0, v = 0;
};

struct FontData {
	Texture2D*	face		= nullptr;
	Vector2		size		= Vector2(16);
	Vector2		kerning		= Vector2(1);
};

struct TextData {
	String		content		= "Hello\nWorld!";
	TextRect	rect		= {20, 100};
	TextAlign	alignment	= ALIGN_LEFT;
};

class Label: public Base::Drawable {
public:
	Label(size_t layer = 0, bool manual = false)
	: Drawable(layer, manual) {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		//material.adjustAlpha = false;
		material.texture.alphaClip = 0.25;
	}

	virtual ~Label() {
		Drawable::~Drawable();
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}

	FontData		font;
	TextData		text;

	Material::ObjectMaterial	material;

	Transform3D trans;

private:
	vector<RawVertex> vertices;

	glm::mat4x4 actorMatrix;

	/// The object's Vertex Array Object (VAO).
	GLuint vao;

	/// The object's Vertex Buffer Object (VBO).
	GLuint vbo;

	TextData last = {"",{0,0}};

	void draw() override {
		// If text changed, update label
		if (
				text.content	!= last.content
			||	text.rect.h		!= last.rect.h
			||	text.rect.v		!= last.rect.v
			||	text.alignment	!= last.alignment
		) {
			last = text;
			update();
		}
		// If no vertices or no font face exists, return
		if (!(vertices.size() && font.face)) return;
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

	void display() {
		// Render with basic shader
		Shader::defaultShader();
		glm::mat4 camera = Scene::camera.matrix();
		glm::mat4 projection = Scene::camera.perspective();
		// Set shader data
		material.texture = {true, font.face, material.texture.alphaClip};
		$mat setMaterial($mainshader, material);
		// Matrices
		$mainshader["world"](Scene::world);
		$mainshader["camera"](camera);
		$mainshader["projection"](projection);
		$mainshader["actor"](actorMatrix);
		// Rendering
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	};

	#define CHAR_VERTEX(POS, UV) {(POS).x,(POS).y,0,(UV).x,(UV).y,1,1,1,1}
	void update() {
		// Clear previous characters
		vertices.clear();
		// The current character's position
		Vector2 cursor;
		// The current character's top left UV index
		Vector2 uv;
		unsigned char index;
		for (char c: text.content) {
			// If cursor has reached the rect's horizontal limit, move to new line
			if(cursor.x > text.rect.h) {
				cursor.x = 0;
				cursor.y -= font.kerning.y;
			}
			// If newline, then reset cursor and continue to next character
			if (c == '\n') {
				cursor.x = 0;
				cursor.y -= font.kerning.y;
				continue;
			}
			// If cursor has reach the rect's vertical limit, break
			if(cursor.y > text.rect.v)
				break;
			// Get character index
			index = Math::max((int)c - 0x20, 0);
			// Get character's top left UV index in the font texture
			uv = Vector2(
				(int)Math::wmax((float)index, font.size.x),
				(int)(index / font.size.x)
			);
			// Get vertex positions
			Vector2 pos[4] = {
				cursor,
				cursor + Vector2(1,0),
				cursor + Vector2(0,-1),
				cursor + Vector2(1,-1),
			};
			// Get UV positions
			Vector2 uvs[4] = {
				uv / font.size,
				(uv + Vector2(1,0)) / font.size,
				(uv + Vector2(0,1))  / font.size,
				(uv + Vector2(1,1)) / font.size,
			};
			// Nightmare
			vertices.push_back(CHAR_VERTEX(pos[0], uvs[0]));
			vertices.push_back(CHAR_VERTEX(pos[1], uvs[1]));
			vertices.push_back(CHAR_VERTEX(pos[2], uvs[2]));
			vertices.push_back(CHAR_VERTEX(pos[1], uvs[1]));
			vertices.push_back(CHAR_VERTEX(pos[2], uvs[2]));
			vertices.push_back(CHAR_VERTEX(pos[3], uvs[3]));
			// Increment cursor
			cursor.x += font.kerning.x;
		}
	}
	#undef CHAR_VERTEX
};

#define $txt $rdt Text::

#if defined(_GRAPHICAL_TEXT_DEBUG_) && !defined(_DEBUG_OUTPUT_)
Label $_DEBUG(Math::maxSizeT);
#define $debug(TEXT)	$txt $_DEBUG.text.content = (TEXT) + "\n"
#define $debugp(TEXT)	$txt $_DEBUG.text.content += (TEXT)
#define $flush()		$txt $_DEBUG.text.content = ""
#endif

