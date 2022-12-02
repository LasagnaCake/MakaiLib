class Drawable {
public:
	Drawable(size_t layer = 0, bool manual = false) {
		if(!manual)
			Drawer::layers.addObject(&render, layer);
	}

	virtual ~Drawable() {
		if(!manualMode)
			Drawer::layers.removeFromAll(&render);
	}

	/// Called before rendering to screen.
	$tev Signal<Drawable*> onDrawBegin	= [](Drawable* target){};
	/// Called after rendering to screen.
	$tev Signal<Drawable*> onDrawEnd	= [](Drawable* target){};

	Drawable* setManual() {
		if(!manualMode)
			Drawer::layers.removeFromAll(&render);
		manualMode = true;
		return this;
	}

	Drawable* setAuto(size_t renderLayer) {
		if(manualMode)
			Drawer::layers.addObject(&render, renderLayer);
		manualMode = false;
		return this;
	}

	Drawable* setRenderLayer(size_t renderLayer) {
		Drawer::layers.removeFromAll(&render);
		Drawer::layers.addObject(&render, renderLayer);
		manualMode = false;
		return this;
	}

	Drawable* addToRenderLayer(size_t renderLayer) {
		Drawer::layers.addObject(&render, renderLayer);
		manualMode = false;
		return this;
	}

	Drawable* removeFromRenderLayer(size_t renderLayer) {
		Drawer::layers.removeFromGroup(&render, renderLayer);
		if (!Drawer::layers.getGroups(&render).size())
			manualMode = true;
		return this;
	}

	/// Renders the object to the screen.
	DrawFunc render = $func() {
		// If not active, return
		if (!active) return;
		// Call onDrawBegin function
		onDrawBegin(this);
		// Draw object
		draw();
		// Call onDrawEnd function
		onDrawEnd(this);
	};

	bool active	= true;

protected:
	virtual void draw() {}

private:

	/// Whether manually rendering or not.
	bool manualMode = false;
};
