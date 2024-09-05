struct Controlable {
	virtual bool action(String const& what, bool const& justPressed = false)	= 0;
	virtual size_t actionState(String const& what)								= 0;
};
