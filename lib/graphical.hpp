#ifndef MAKAILIB_GRAPHICS
#define MAKAILIB_GRAPHICS

#include "anchors.hpp"

#ifdef MAKAI_ALLEGRO5
#include "graphical/al_graphical.hpp"
#else
#include "graphical/gl_graphical.hpp"
#include "graphical/gl_shader.hpp"
#endif

#endif // GRAPHICAL_RENDERER_H
