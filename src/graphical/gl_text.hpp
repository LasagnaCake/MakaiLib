class Font {
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
};

// TODO: Make this work
/* NOTE: It'll probably be done using SDL_FontCache + OpenGL. Scratch that. Freetype GL.
*	Y'know what? Fuck This. I'm making it by myself.
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

