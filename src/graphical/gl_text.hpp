enum class HAlign {
	LEFT,
	RIGHT,
	CENTER
};

enum class VAlign {
	TOP,
	BOTTOM,
	CENTER
};

struct TextRect {
	size_t h = 0, v = 0;
};

struct AlignRect {
	HAlign h = HAlign::LEFT;
	VAlign v = VAlign::TOP;
};

struct FontData {
	Texture2D*	face		= nullptr;
	Vector2		size		= Vector2(16);
	Vector2		spacing		= Vector2(1);
};

struct TextData {
	String		content		= "Hello\nWorld!";
	TextRect	rect		= {40, 100};
	AlignRect	textAlign	= {};
	AlignRect	rectAlign	= {};
	Vector2		spacing		= Vector2(0);
};

namespace {
	bool isTextDataEqual(TextData& a, TextData& b) {
		return	a.content	== b.content
			&&	a.rect.h	== b.rect.h
			&&	a.rect.v	== b.rect.v
			&&	a.textAlign.h	== b.textAlign.h
			&&	a.textAlign.v	== b.textAlign.v
			&&	a.rectAlign.h	== b.rectAlign.h
			&&	a.rectAlign.v	== b.rectAlign.v
			&&	a.spacing	== b.spacing;
	}
}

vector<float> getTextLineStarts(TextData& text, FontData& font) {
	vector<float> result;
	StringList	lines;
	String		line;
	// Separate text by newline characters
	std::istringstream content(text.content);
	while (std::getline(content, line, '\n'))
		lines.push_back(line);
	// Calculate starting points
	for (String& l : lines) {
		size_t lineSize		= l.size();
		size_t lastLineSize	= Math::wmax(lineSize, text.rect.h);
		if (lineSize > text.rect.h) {
			for (size_t i = 0; i < (lineSize - lastLineSize) / text.rect.h; i++)
				result.push_back(0);
		}
		result.push_back(
			(text.rect.h - lastLineSize)
		*	(text.spacing.x + font.spacing.x)
		/	(text.textAlign.h == HAlign::CENTER ? 2.0 : 1.0)
		);
	}
	// Return result
	return result;
}

Vector2 getTextRectStart(TextData& text, FontData& font) {
	Vector2 rectPos =
		Vector2(
			(text.rectAlign.h != HAlign::LEFT)	? text.rect.h : 0,
			(text.rectAlign.v != VAlign::TOP)	? text.rect.v : 0
		)
	;
	rectPos *= Vector2(
		(text.rectAlign.h == HAlign::CENTER) ? 0.5 : 1.0,
		(text.rectAlign.v == VAlign::CENTER) ? 0.5 : 1.0
	);
	rectPos *= (text.spacing + font.spacing);
	return rectPos;
}

class Label: public Base::DrawableObject {
public:
	Label(size_t layer = 0, bool manual = false): DrawableObject(layer, manual) {}

	virtual ~Label() {
		$debug("Text!");
	}

	FontData*		font = nullptr;
	TextData		text;

private:
	vector<RawVertex> vertices;

	TextData last = {"",{0,0}};

	void draw() override {
		// If no font face exists return
		if (!(font && font->face)) return;
		// If text changed, update label
		if (!isTextDataEqual(text, last)) {
			last = text;
			update();
		}
		// If no vertices, return
		if (!vertices.size()) return;
		// Set shader data
		material.texture = {true, font->face, material.texture.alphaClip};
		setDefaultShader();
		// Display to screen
		display(&vertices[0], vertices.size());
	}


	// TODO: Proper text alignment
	#define CHAR_VERTEX(POS, UV) RawVertex{(POS).x,(POS).y,0,(UV).x,(UV).y}
	void update() {
		// If font doesn't exist, return
		// Clear previous characters
		vertices.clear();
		// The current character's position
		Vector2		cursor;
		TextRect	chrRect = {0,0};
		Vector2		rectStart = getTextRectStart(text, *font);
		// The current character's top left UV index
		Vector2 uv;
		unsigned char index;
		// Horizontal alignment flags
		bool
			alignLeft	= text.textAlign.h == HAlign::LEFT,
			alignTop	= text.textAlign.v == VAlign::TOP;
		bool
			hCenter		= text.textAlign.h == HAlign::CENTER,
			vCenter		= text.textAlign.v == VAlign::CENTER;
		// The lines' starting positions (if applicable)
		vector<float> lineStart;
		if (!alignLeft || hCenter) {
			lineStart	= getTextLineStarts(text, *font);
			cursor.x	= lineStart[0];
		}
		if (!alignTop || vCenter)
			cursor.y = text.rect.v * (text.spacing.y + font->spacing.y);
		if (vCenter) cursor.y /= 2.0;
		cursor.y += rectStart.y;
		size_t curLine = 0;
		// Loop through each character and...
		for (char c: text.content) {
			// Check if character is newline
			bool newline = c == '\n';
			// If cursor has reached the rect's horizontal limit or newline, move to new line
			if((chrRect.h >= text.rect.h) || newline) {
				cursor.x = ((alignLeft && !hCenter) ? 0 : lineStart[++curLine]) - rectStart.x;
				cursor.y -= (text.spacing.y + font->spacing.y) * (alignTop ? +1 : -1);
				chrRect.h = 0;
				chrRect.v++;
				// If newline, move on to next character
				if (newline) continue;
			}
			// If cursor has reach the rect's vertical limit, break
			if(chrRect.v >= text.rect.v)
				break;
			// Get character index
			index = Math::max((int)c - 0x20, 0);
			// Get character's top left UV index in the font texture
			uv = Vector2(
				(int)Math::wmax((float)index, font->size.x),
				(int)(index / font->size.x)
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
				uv / font->size,
				(uv + Vector2(1,0)) / font->size,
				(uv + Vector2(0,1))  / font->size,
				(uv + Vector2(1,1)) / font->size,
			};
			// Nightmare
			vertices.push_back(CHAR_VERTEX(pos[0], uvs[0]));
			vertices.push_back(CHAR_VERTEX(pos[1], uvs[1]));
			vertices.push_back(CHAR_VERTEX(pos[2], uvs[2]));
			vertices.push_back(CHAR_VERTEX(pos[1], uvs[1]));
			vertices.push_back(CHAR_VERTEX(pos[2], uvs[2]));
			vertices.push_back(CHAR_VERTEX(pos[3], uvs[3]));
			// Increment cursor
			cursor.x += text.spacing.x + font->spacing.x;
			chrRect.h++;
		}
	}
	#undef CHAR_VERTEX
};

#define $txt $rdt Text::

#if defined(_GRAPHICAL_TEXT_DEBUG_) && !defined(_DEBUG_OUTPUT_)
Label $_DEBUG(Math::Max::SIZET_V);
#define $debug(TEXT)	$txt $_DEBUG.text.content = (TEXT) + "\n"
#define $debugp(TEXT)	$txt $_DEBUG.text.content += (TEXT)
#define $flush()		$txt $_DEBUG.text.content = ""
#endif

