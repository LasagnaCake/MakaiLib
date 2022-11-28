struct Font {
	Texture2D*	texture = nullptr;
	Vector2		frame = Vector2(16);
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

	Font		font;
	TextData	text;

	Transform2D trans;
protected:

	void draw() override {
	};
};

#define $txt $rdt Text::
