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
	String		content	= "Hello\nWorld!";
	TextRect	rect	= {40, 100};
	AlignRect	align	= {};
	Vector2		spacing	= Vector2(0);
};

namespace {
	bool isTextDataEqual(TextData& a, TextData& b) {
		return	a.content	== b.content
			&&	a.rect.h	== b.rect.h
			&&	a.rect.v	== b.rect.v
			&&	a.align.h	== b.align.h
			&&	a.align.v	== b.align.v
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
		);
	}
	// Return result
	return result;
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
		// If text changed, update label
		if (!isTextDataEqual(text, last)) {
			last = text;
			update();
		}
		// If no vertices or no font face exists, return
		if (!(vertices.size() && (font || font->face))) return;
		// Set shader data
		material.texture = {true, font->face, material.texture.alphaClip};
		setDefaultShader();
		// Display to screen
		display(&vertices[0], vertices.size());
	}


	// TODO: Proper text alignment
	#define CHAR_VERTEX(POS, UV) RawVertex{(POS).x,(POS).y,0,(UV).x,(UV).y}
	void update() {
		// Clear previous characters
		vertices.clear();
		// The current character's position
		Vector2		cursor;
		TextRect	chrRect = {0,0};
		// The current character's top left UV index
		Vector2 uv;
		unsigned char index;
		// Horizontal alignment flags
		bool
			alignLeft	= text.align.h == HAlign::LEFT,
			alignTop	= text.align.v == VAlign::TOP;
		bool
			hCenter		= text.align.h == HAlign::CENTER,
			vCenter		= text.align.v == VAlign::CENTER;
		// The lines' starting positions (if applicable)
		vector<float> lineStart;
		if (!alignLeft || hCenter) {
			lineStart	= getTextLineStarts(text, *font);
			cursor.x	= lineStart[0];
		}
		if (!alignTop || vCenter)
			cursor.y = text.rect.v * (text.spacing.y + font->spacing.y);
		if (hCenter) cursor.x /= 2;
		if (vCenter) cursor.y /= 2;
		size_t curLine = 0;
		// Loop through each character and...
		for (char c: text.content) {
			// Check if character is newline
			bool newline = c == '\n';
			// If cursor has reached the rect's horizontal limit or newline, move to new line
			if((chrRect.h >= text.rect.h) || newline) {
				cursor.x = (alignLeft && !hCenter) ? 0 : lineStart[++curLine];
				if (hCenter)
					cursor.x /= 2.0;
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
Label $_DEBUG(Math::maxSizeT);
#define $debug(TEXT)	$txt $_DEBUG.text.content = (TEXT) + "\n"
#define $debugp(TEXT)	$txt $_DEBUG.text.content += (TEXT)
#define $flush()		$txt $_DEBUG.text.content = ""
#endif

