#ifndef MAKAILIB_LIBRARY_HEADER_H
#define MAKAILIB_LIBRARY_HEADER_H

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wnarrowing"
#pragma GCC diagnostic ignored "-Wcatch-value="
#pragma GCC diagnostic ignored "-Wvla"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wattributes"
#pragma GCC diagnostic ignored "-Wpedantic"

#include "memoverride.hpp"

#include "anchors.hpp"
#include "audio.hpp"
#include "program.hpp"
#include "graphical.hpp"
#include "gamedata.hpp"

#ifndef MAKAILIB_FORCE_INTEGRATED_GRAPHICS
extern "C" {
DLL_EXPORT	uint32	NvOptimusEnablement = 1;
DLL_EXPORT	int		AmdPowerXpressRequestHighPerformance = 1;
}
#endif // MAKAILIB_FORCE_INTEGRATED_GRAPHICS

namespace Makai {
}

#pragma GCC diagnostic pop

#endif // MAKAILIB_LIBRARY_HEADER_H
