class Dummy: public Base::Drawable {
public:
	Dummy(Arguments<size_t> const& layers) {
		for (size_t const& l : layers)
			addToRenderLayer(l);
	}
};
