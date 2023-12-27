struct TextRect {
	size_t h = 0, v = 0;
};

struct FontData {
	Texture2D*	face		= nullptr;
	Vector2		size		= Vector2(16);
	Vector2		spacing		= Vector2(1);
};

enum class LineBreak {
	LB_CHARACTER,
	LB_FULL_WORD,
	LB_HYPHEN_WORD
};

struct TextData {
	String		content		= "Hello\nWorld!";
	TextRect	rect		= {40, 100};
	Vector2		textAlign	= 0;
	Vector2		rectAlign	= 0;
	Vector2		spacing		= 0;
	long		maxChars	= -1;
	LineBreak	lineBreak	= LineBreak::LB_CHARACTER;
};

namespace {
	constexpr bool isTextDataEqual(TextData const& a, TextData const& b) {
		return Fold::land(
			a.content		== b.content
		,	a.rect.h		== b.rect.h
		,	a.rect.v		== b.rect.v
		,	a.textAlign.x	== b.textAlign.x
		,	a.textAlign.y	== b.textAlign.y
		,	a.rectAlign.x	== b.rectAlign.x
		,	a.rectAlign.y	== b.rectAlign.y
		,	a.spacing		== b.spacing
		,	a.maxChars		== b.maxChars
		);
	}
}

List<float> getTextLineStarts(TextData& text, FontData& font) {
	List<float> result;
	StringList	lines;
	String		line;
	// Separate text by newline characters
	std::istringstream content(text.content);
	while (std::getline(content, line, '\n'))
		lines.push_back(line);
	// Calculate starting points
	for (String& l : lines) {
		size_t lineSize		= l.size();
		size_t lastLineSize	= Math::wmax(lineSize, text.rect.h+1);
		if (lineSize > text.rect.h) {
			for (size_t i = 0; i < (lineSize - lastLineSize) / text.rect.h; i++)
				result.push_back(0);
		}
		result.push_back(
			((float)text.rect.h - (float)lastLineSize)
		*	(text.spacing.x + font.spacing.x)
		*	text.textAlign.x
		+	(
				(text.spacing.x + font.spacing.x)
			*	text.textAlign.x
			*	text.textAlign.x
			*	text.textAlign.x
			)
		);
	}
	// Return result
	return result;
}

Vector2 getTextRectStart(TextData& text, FontData& font) {
	Vector2 rectPos = Vector2(text.rect.h, text.rect.v) * text.rectAlign;
	rectPos += (text.spacing + font.spacing) * text.rectAlign * Vector2(1, -1);
	rectPos *= (text.spacing + font.spacing);
	return rectPos;
}

namespace {
	constexpr List<size_t> calculateIndices(StringList const& words, TextRect const& rect) {
		List<size_t> indices;
		size_t
			ls = 0,
			sc = 0
		;
		for (String const& w: words) {
			if ((ls + sc + w.size()) > (rect.h-1)) {
				indices.push_back(ls + sc - 1);
				ls = w.size();
				sc = 1;
			} else {
				ls += w.size();
				sc++;
			}
		}
		indices.push_back(ls + sc - 1);
		return indices;
	}
}

// TODO: figure out how the fuck to deal with line breaks
List<size_t> getTextLineBreakIndices(TextData& text) {
	List<size_t>	indices;
	switch (text.lineBreak) {
	case LineBreak::LB_CHARACTER:
		break;
	case LineBreak::LB_HYPHEN_WORD: {
			StringList words, buf = Helper::splitString(text.content, ' ');
			for (String const& w: buf) {
				StringList buf2 = Helper::splitString(
					regexReplace(
						regexReplace(
							w,
							"([\\x01-\\x1f])",
							""
						),
						"~",
						"-"
					),
					'-'
				);
				for(String const& w2: buf2)
					words.push_back(w2);
			}
			indices = calculateIndices(words, text.rect);
		}
		break;
	case LineBreak::LB_FULL_WORD: {
			indices = calculateIndices(
				Helper::splitString(
					regexReplace(
						text.content,
						"([\\x01-\\x1f])",
						""
					),
					' '
				),
				text.rect
			);
		}
		break;
	}
	return indices;
}

class Label: public Base::DrawableObject {
public:
	Label(size_t layer = 0, bool manual = false): DrawableObject(layer, manual) {}

	virtual ~Label() {
		DEBUGLN("Text!");
	}

	FontData*		font = nullptr;
	TextData		text;

private:
	List<RawVertex> vertices;

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
		// The lines' starting positions (if applicable)
		List<float>		lineStart = getTextLineStarts(text, *font);
		List<size_t>	lineBreak = getTextLineBreakIndices(text);
		cursor.x	= lineStart[0];
		cursor.y 	= text.rect.v * (text.spacing.y + font->spacing.y) * text.textAlign.y;
		cursor -= rectStart * Vector2(1,-1);
		// The current line and current character
		size_t curLine = 0;
		size_t curChar = 0;
		// Loop through each character and...
		for (char c: text.content) {
			// Check if max characters hasn't been reached
			if ((curChar > (text.maxChars-1)) && (text.maxChars > -1)) break;
			else curChar++;
			// Check if character is newline
			bool
				newline = (c == '\n'),
				endOfWordLine = false
			;
			// Check if should break line
			if (text.lineBreak != LineBreak::LB_CHARACTER && text.content.size() >= text.rect.h)
				endOfWordLine = chrRect.h > lineBreak[curLine];
			// If cursor has reached the rect's horizontal limit or newline, move to new line
			if((chrRect.h >= text.rect.h) || newline || endOfWordLine) {
				// If cursor has reach the rect's vertical limit, break
				if(chrRect.v >= text.rect.v) break;
				cursor.x = (lineStart[++curLine]) - rectStart.x;
				cursor.y -= (text.spacing.y + font->spacing.y) * 1.0;
				chrRect.h = 0;
				chrRect.v++;
				// If newline, move on to next character
				if (newline) continue;
			}
			// If cursor has reach the rect's vertical limit, break
			if(chrRect.v >= text.rect.v) break;
			// If character is a control character, skip
			if (c < 0x20) continue;
			// Get character index
			index = Math::max<int>(c - 0x20, 0);
			// Get character's top left UV index in the font texture
			uv = Vector2(
				(int)Math::wmax<float>(index, font->size.x),
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

