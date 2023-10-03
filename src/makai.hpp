#ifndef MAKAILIB_LIBRARY_HEADER_H
#define MAKAILIB_LIBRARY_HEADER_H

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wnarrowing"
#pragma GCC diagnostic ignored "-Wcatch-value="
//#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wvolatile"
#pragma GCC diagnostic ignored "-Wvla"
#pragma GCC diagnostic ignored "-Wvariadic-macros"
#pragma GCC diagnostic ignored "-Wattributes"

#include "memoverride.hpp"

#include "anchors.hpp"
#include "audio.hpp"
#include "program.hpp"
#include "graphical.hpp"
#include "gamedata.hpp"

#ifndef $_FORCE_INTEGRATED_GRAPHICS
extern "C" {
DLL_EXPORT	uint32	NvOptimusEnablement = 1;
DLL_EXPORT	int		AmdPowerXpressRequestHighPerformance = 1;
}
#endif // $_FORCE_INTEGRATED_GRAPHICS

namespace Makai {
}

#pragma GCC diagnostic pop

#endif // MAKAILIB_LIBRARY_HEADER_H
