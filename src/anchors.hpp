#ifndef ANCHORED_HEADERS_H
#define ANCHORED_HEADERS_H

#define $enforce(WHAT, CHECK) if(!(CHECK)) throw std::runtime_error(std::string("ERROR: failed to initialize ") + WHAT)

#ifdef _DEBUG_OUTPUT_
#include <iostream>
#define $errlog(TEXT) std::cerr << TEXT << std::endl
#define $debug(TEXT) std::cout << (TEXT); std::cout << "\n"
#define $debugp(TEXT) std::cout << (TEXT)
#define $flush() std::cout << std::endl
#else
#define $errlog(TEXT)
#define $debug(TEXT)
#define $debugp(TEXT)
#define $flush()
#endif // _MAKAI_DEBUG_

/*
*******************
*                 *
*  General stuff  *
*                 *
*******************
*/

#include <string>
#include <vector>
#include <functional>
#include <cstring>
#include <stdexcept>
#include <cstdlib>
#include <math.h>
#include <thread>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <chrono>
//#include <coroutine>
#include <filesystem>
#include <execution>

#include "redefines.hpp"

/*
***************************
*                         *
*  File management stuff  *
*                         *
***************************
*/

#include <zlib.h>
#include <bzlib.h>
#define MINI_CASE_SENSITIVE
#include <mINI-0.9.14/ini.h>

/*
*********************
*                   *
*  Graphical stuff  *
*                   *
*********************
*/

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define GLEW_STATIC
#include <GLEW/include/GL/glew.h>
#include <GLEW/include/GL/wglew.h>
#include <GL/gl.h>
//#include <GLFW/include/GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/*
***************
*             *
*  SDL stuff  *
*             *
***************
*/
#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
#include <winuser.h>
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>

/*
*****************
*               *
*  Audio stuff  *
*               *
*****************
*/
/*
#include <portaudio.h>
#include <AL/al.h>
//*/
// Shut up
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#define STB_VORBIS_IMPLEMENTATION
#include <stb_vorbis.c>
#define CUTE_SOUND_FORCE_SDL
#define CUTE_SOUND_IMPLEMENTATION
#include <cute_sound.h>
#pragma GCC diagnostic pop
/*
***************************
*                         *
*  Engine-specific stuff  *
*                         *
***************************
*/

#include "collection/algebra.hpp"
#include "collection/definitions.hpp"
#include "collection/helper.hpp"
#include "collection/conceptual.hpp"

//#define sin glm::sin
//#define cos glm::cos
//#define tan glm::tan

#include "collection/entity/entity.hpp"
#include "collection/vectorn.hpp"
#include "collection/cyclical.hpp"
#include "collection/tween.hpp"
#include "collection/event.hpp"
#include "collection/tasking.hpp"
#include "collection/grouping.hpp"
#include "collection/dictionary.hpp"

#include "fileloader.hpp"

/*
*	[+]=[   [Headers END]   ]=[+]
*/
/*
#ifdef _$_LUA_STYLE_PROGRAMMING
#define END		}
#define AND		&&
#define OR		||
#define NOT		!
#define THEN	)
#define EXECUTE	{
#define TAKE	(
#define DONE	;
#define LESS	<
#define MORE	>
#define IS		==
#define EQUALS	=
#endif // _$_LUA_STYLE_PROGRAMMING
*/
#endif // ANCHORED_HEADERS_H
