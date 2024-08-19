#ifndef AUDIO_SAMPLE_FRAMES
#define AUDIO_SAMPLE_FRAMES 2048
#endif // AUDIO_SAMPLE_FRAMES

#define GLEW_STATIC
#include <GLEW/include/GL/glew.h>
#include <GLEW/include/GL/wglew.h>
#include <GL/gl.h>

#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
#include <winuser.h>
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>

#include <SDL2/SDL_mixer.h>

#define window ((SDL_Window*)window)

#include "app.hpp"
