#ifndef MAKAILIB_GRAPH_RENDERER_TEXT_H
#define MAKAILIB_GRAPH_RENDERER_TEXT_H

#include "../../../compat/ctl.hpp"
#include "../texture.hpp"
#include "drawable.hpp"

namespace Makai::Graph {
	using Makai::Graph::Texture2D;

	struct TextRect {
		usize h = 0, v = 0;
	};

	struct FontData {
		Texture2D	image;
		Vector2		size	= Vector2(16);
		Vector2		spacing	= Vector2(1);
	};

	struct FontFace {
	public:
		using FontInstance = Instance<FontData>;

		// This needs to be initialized this specific way, else it breaks somehow???
		FontFace(): instance(new FontData()) {}

		FontFace(FontData const& font);

		FontFace(String const& path);

		FontFace& operator=(FontFace const& other);

		FontFace& operator=(FontData const& font);

		FontData& operator*() const;
		FontData* operator->() const;

		FontData& data() const;

		bool exists() const;
		explicit operator bool() const;

	private:
		FontInstance instance;
	};

	enum class LineWrap {
		LW_CHARACTER,
		LW_FULL_WORD,
		LW_HYPHEN_WORD
	};

	struct TextData {
		String		content		= "Hello\nWorld!";
		TextRect	rect		= {40, 100};
		Vector2		textAlign	= 0;
		Vector2		rectAlign	= 0;
		Vector2		spacing		= 0;
		long		maxChars	= -1;
		LineWrap	lineWrap	= LineWrap::LW_CHARACTER;
	};

	class Label: public DrawableObject {
	public:
		Label(size_t const& layer = 0, bool const manual = false): DrawableObject(layer, manual) {}

		virtual ~Label() {}

		FontFace					font;
		TextData					text;
		Material::ObjectMaterial	material;

	private:
		List<Vertex> vertices;

		TextData last = {"",{0,0}};

		void draw() override;

		void update();
	};
}

#endif // MAKAILIB_GRAPH_RENDERER_TEXT_H
