#include "../../../file/json.hpp"

#include "text.hpp"

using namespace Makai::Graph;

namespace JSON = Makai::JSON;

inline Vector2 fromJSONArrayV2(JSON::JSONData const& json, Vector2 const& defaultValue = 0) {
	try {
		if (json.isArray())
			return Vector2(
				json[0].get<float>(),
				json[1].get<float>()
			);
		else if (json.isNumber())
			return json.get<float>();
		else return defaultValue;
	} catch (std::exception const& e) {
		return defaultValue;
	}
}

inline Vector3 fromJSONArrayV3(JSON::JSONData const& json, Vector3 const& defaultValue = 0) {
	try {
		if (json.isArray())
			return Vector3(
				json[0].get<float>(),
				json[1].get<float>(),
				json[2].get<float>()
			);
		else if (json.isNumber())
			return json.get<float>();
		else return defaultValue;
	} catch (std::exception const& e) {
		return defaultValue;
	}
}

inline Vector4 fromJSONArrayV4(JSON::JSONData const& json, Vector4 const& defaultValue = 0) {
	try {
		if (json.isArray())
			return Vector4(
				json[0].get<float>(),
				json[1].get<float>(),
				json[2].get<float>(),
				json[3].get<float>()
			);
		else if (json.isNumber())
			return json.get<float>();
		else return defaultValue;
	} catch (std::exception const& e) {
		return defaultValue;
	}
}

FontFace::FontFace(FontData const& font): FontFace() {
	instance->image		= font.image;
	instance->size		= font.size;
	instance->spacing	= font.spacing;
}

FontFace::FontFace(String const& path): FontFace() {
	JSON::JSONData tx	= File::getJSON(path);
	instance->image		= Texture2D::fromJSON(tx["image"], FileSystem::getDirectoryFromPath(path));
	instance->size		= fromJSONArrayV2(tx["size"], 16);
	instance->spacing	= fromJSONArrayV2(tx["spacing"], 1);
}

FontFace& FontFace::operator=(FontFace const& other) {
	instance = other.instance;
	return *this;
}

FontFace& FontFace::operator=(FontData const& font) {
	instance = new FontData();
	instance->image		= font.image;
	instance->size		= font.size;
	instance->spacing	= font.spacing;
	return *this;
}

FontData& FontFace::operator*() const	{return data();		}
FontData* FontFace::operator->() const	{return &data();	}

FontData& FontFace::data() const {return *instance; }

bool FontFace::exists() const	{return instance.exists() && instance->image.exists();	}
FontFace::operator bool() const	{return exists();										}

constexpr bool isTextDataEqual(TextData const& a, TextData const& b) {
	return Fold::land(
		a.content	== b.content
	,	a.rect.h	== b.rect.h
	,	a.rect.v	== b.rect.v
	,	a.textAlign	== b.textAlign
	,	a.rectAlign	== b.rectAlign
	,	a.spacing	== b.spacing
	,	a.maxChars	== b.maxChars
	,	a.lineWrap	== b.lineWrap
	);
}

List<float> getTextLineStarts(TextData const& text, FontData const& font, List<size_t> const& breaks) {
	List<float> result;
	switch (text.lineWrap) {
	case LineWrap::LW_CHARACTER: {
			// Separate text by newline characters
			StringList	lines = Helper::splitString(text.content, '\n');
			// Calculate starting points
			for (String& l : lines) {
				size_t lineSize		= l.size();
				size_t lastLineSize	= lineSize % (text.rect.h+1);
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
		}
	case LineWrap::LW_FULL_WORD:
	case LineWrap::LW_HYPHEN_WORD: {
			for (size_t const& lb: breaks) {
				result.push_back(
					((float)text.rect.h - (float)lb)
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
		}
	}
	// Return result
	return result;
}

Vector2 getTextRectStart(TextData const& text, FontData const& font) {
	Vector2 rectPos = Vector2(text.rect.h, text.rect.v) * text.rectAlign;
	rectPos += (text.spacing + font.spacing) * text.rectAlign * Vector2(1, -1);
	rectPos *= (text.spacing + font.spacing);
	return rectPos;
}

constexpr List<size_t> calculateIndices(StringList const& words, TextRect const& rect) {
	List<size_t> indices;
	/*size_t
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
	indices.push_back(ls + sc - 1);*/
	size_t
		lastBreak	= 0,
		curWord		= 0
	;
	for (String const& word: words) {
		for (char const& c: word) {
			if (c == '\n') {
				indices.push_back(curWord+lastBreak-1);
				lastBreak = 0;
			} else if ((lastBreak + (++curWord)) > (rect.h-1)) {
				indices.push_back(lastBreak-1);
				lastBreak = 0;
			}
		}
		lastBreak += ++curWord;
		curWord = 0;
	}
	indices.push_back(lastBreak-1);
	return indices;
}

List<size_t> getTextLineWrapIndices(TextData& text) {
	List<size_t>	indices;
	switch (text.lineWrap) {
	case LineWrap::LW_CHARACTER:
		break;
	case LineWrap::LW_HYPHEN_WORD: {
			indices = calculateIndices(
				Helper::splitString(
					text.content,
					{' ', '~', '\t', '-'}
				),
				text.rect
			);
		}
		break;
	case LineWrap::LW_FULL_WORD: {
			indices = calculateIndices(
				Helper::splitString(
					text.content,
					{' ', '~', '\t'}
				),
				text.rect
			);
		}
		break;
	}
	return indices;
}

void Label::draw() {
	// If text changed, update label
	if (!isTextDataEqual(text, last)) {
		last = text;
		update();
	}
	// If no vertices, return
	if (!vertices.size()) return;
	// Set shader data
	material.texture = {true, font->image, material.texture.alphaClip};
	prepare();
	material.use(shader);
	// Display to screen
	display(
		&vertices[0], vertices.size(),
		material.culling,
		material.fill,
		DisplayMode::ODM_TRIS,
		material.instances.size()
	);
}

void Label::update() {
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
	List<size_t>	lineEnd = getTextLineWrapIndices(text);
	List<float>		lineStart = getTextLineStarts(text, *font, lineEnd);
	cursor.x	= lineStart[0];
	cursor.y 	= text.rect.v - Math::min<size_t>(lineEnd.size(), text.rect.v);
	cursor.y	*= (text.spacing.y + font->spacing.y) * -text.textAlign.y;
	cursor -= rectStart * Vector2(1,-1);
	// The current line and current character
	usize curLine = 0;
	usize curChar = 0;
	// Loop through each character and...
	for (char c: text.content) {
		// Check if max characters hasn't been reached
		if (text.maxChars == 0 || ((llong(curChar) > llong(text.maxChars-1)) && (text.maxChars > -1))) break;
		else curChar++;
		// Check if character is newline
		bool
			newline = (c == '\n'),
			endOfWordLine = false
		;
		// Check if should break line
		if (text.lineWrap != LineWrap::LW_CHARACTER && text.content.size() >= text.rect.h)
			endOfWordLine = chrRect.h > lineEnd[curLine];
		// If cursor has reached the rect's horizontal limit or newline, move to new line
		if((chrRect.h >= text.rect.h) || newline || endOfWordLine) {
			// If cursor has reach the rect's vertical limit, break
			if(chrRect.v >= text.rect.v) break;
			cursor.x = (lineStart[++curLine]) - rectStart.x;
			cursor.y -= (text.spacing.y + font->spacing.y) * 1.0;
			chrRect.h = 0;
			chrRect.v++;
			// If newline, move on to next character
			if (newline) {
				if (endOfWordLine) chrRect.v--;
				continue;
			}
		}
		// If cursor has reach the rect's vertical limit, break
		if(chrRect.v >= text.rect.v) break;
		// If character is a control character, skip
		if (c < 0x20) continue;
		// Get character index
		index = Math::max<int>(c - 0x20, 0);
		// Get character's top left UV index in the font texture
		uv = Vector2(
			(int)(index % int(font->size.x)),
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
		vertices.push_back(Vertex(pos[0], uv[0]));
		vertices.push_back(Vertex(pos[1], uv[1]));
		vertices.push_back(Vertex(pos[2], uv[2]));
		vertices.push_back(Vertex(pos[1], uv[1]));
		vertices.push_back(Vertex(pos[2], uv[2]));
		vertices.push_back(Vertex(pos[3], uv[3]));
		// Increment cursor
		cursor.x += text.spacing.x + font->spacing.x;
		chrRect.h++;
	}
}
