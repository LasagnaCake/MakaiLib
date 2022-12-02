class Font {
	Font() {}

	Font(SDL_Renderer* renderer) {
		this->renderer = renderer;
	}

	Font(SDL_Renderer* renderer, String path) {
		this->renderer = renderer;
	}

	void create(String path) {
		if (created) return;
		created = true;
		resource = FC_CreateFont();
	}

	void destroy() {
		if (!created) return;
		created = false;
		FC_FreeFont(resource);
	}

	FC_Font* getResource() {
		return resource;
	}

private:
	bool created = false;

	FC_Font*		resource;
	SDL_Renderer*	renderer;
};

struct TextData {
	String			content;
	FC_AlignEnum	align;
	Vector2			scale = Vector2(1);
	Vector4			color = Color::WHITE;
	size_t			columns = 40;
};

// TODO: Make this work
/* NOTE: It'll probably be done using SDL_FontCache.
* Seriously, I NEED to do this. How else would the player know its score?
*/
class Label: Base::Drawable {
public:
	Label(size_t layer = 0, bool manual = false)
	: Drawable(layer, manual) {
	}

	virtual ~Label() {
		Drawable::~Drawable();
	}

	Font*		font;
	TextData	text;

	Transform2D trans;

private:
	void draw() override {
	};
};

#define $txt $rdt Text::

#if defined(_GRAPHICAL_TEXT_DEBUG_) && !defined(_DEBUG_OUTPUT_)
Label $_DEBUG(Math::maxSizeT);
#define $debug(TEXT)	$txt $_DEBUG.text.content = (TEXT) + "\n"
#define $debugp(TEXT)	$txt $_DEBUG.text.content += (TEXT)
#define $flush()		$txt $_DEBUG.text.content = ""
#endif

