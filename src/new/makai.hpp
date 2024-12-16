#ifndef MAKAILIB_H
#define MAKAILIB_H

#ifdef MAKAILIB_DEBUG
#define CTL_CONSOLE_OUT
#define NDEBUG
#endif

#define ENABLE_NVIDIA_GPU extern "C" __declspec(dllexport) ulong NvOptimusEnablement = 1
#define ENABLE_AMD_GPU extern "C" __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1

#define ENABLE_DEDICATED_GPU ENABLE_NVIDIA_GPU; ENABLE_AMD_GPU

#include "core/core.hpp"
#include "data/data.hpp"
#include "file/file.hpp"
#include "graph/graph.hpp"
#include "audio/audio.hpp"
#include "tool/tool.hpp"

#endif // MAKAILIB_H
