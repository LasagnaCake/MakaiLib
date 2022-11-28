struct Font {
};

struct TextData {
	std::string		content;
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

	Font		font;
	TextData	text;

	Transform2D trans;
protected:

	void draw() override {
	};
};

#define $txt $rdt Text::
