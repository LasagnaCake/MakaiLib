#ifndef MAKAILIB_LIBRARY_HEADER_H
#define MAKAILIB_LIBRARY_HEADER_H

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wnarrowing"
#pragma GCC diagnostic ignored "-Wcatch-value="
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wvolatile"
#pragma GCC diagnostic ignored "-Wvla"
#pragma GCC diagnostic ignored "-Wvariadic-macros"

#include "anchors.hpp"
#include "audio.hpp"
#include "program.hpp"
#include "graphical.hpp"
#include "gamedata.hpp"

#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllexport)	uint32	NvOptimusEnablement = 1;
__declspec(dllexport)	int		AmdPowerXpressRequestHighPerformance = 1;
#ifdef __cplusplus
}
#endif

namespace Makai {
}

#pragma GCC diagnostic pop

#endif // MAKAILIB_LIBRARY_HEADER_H
