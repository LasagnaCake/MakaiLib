#ifndef MAKAILIB_LIBRARY_HEADER_H
#define MAKAILIB_LIBRARY_HEADER_H

#include "anchors.hpp"
#include "program.hpp"
#include "graphical.hpp"
#include "gamedata.hpp"

namespace Makai {
	namespace Resolution {
		#define RESOLUTION(W, H) {#W" × "#H , Vector2(W, H)}
		const map<string, Vector2> set4x3 = {
			RESOLUTION(480, 320),
			RESOLUTION(640, 480),
			RESOLUTION(960, 720),
			RESOLUTION(800, 600),
			RESOLUTION(1200, 900),
			RESOLUTION(1600, 1200)
		};
		#undef RESOLUTION
	}
}

#define $res $mki Resolution::

#endif // MAKAILIB_LIBRARY_HEADER_H
