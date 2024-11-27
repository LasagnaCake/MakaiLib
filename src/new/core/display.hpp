#ifndef MAKAILIB_CORE_DISPLAY_H
#define MAKAILIB_CORE_DISPLAY_H

#include "../compat/ctl.hpp"

namespace Makai {
	/// A position on the screen.
	struct ScreenPosition {
		int x, y;
	};

	/// Returns the window size of the selected display.
	Vector2 getDisplaySize(usize const display = 0);
}

#endif // MAKAILIB_CORE_DISPLAY_H
