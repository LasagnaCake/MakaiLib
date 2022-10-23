#ifndef ANCHORED_HEADERS_H
#define ANCHORED_HEADERS_H

#define $enforce(WHAT, CHECK) if(!(CHECK)) throw std::runtime_error(std::string("ERROR: failed to initialize ") + WHAT)

#ifdef _DEBUG_OUTPUT_
#include <iostream>
#define $errlog(TEXT) std::cerr << TEXT << std::endl
#define $debug(TEXT) std::cout << (TEXT) << std::endl
#else
#define $errlog(TEXT)
#define $debug(TEXT)
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
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <chrono>

/*
*********************
*                   *
*  Graphical stuff  *
*                   *
*********************
*/

//#include <SFML/Graphics.hpp>
//#include <raylib.h>

#ifdef MAKAI_ALLEGRO5
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#else
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>
//#include <SDL2/SDL_ttf.h>
#endif

/*
***************************
*                         *
*  Engine-specific stuff  *
*                         *
***************************
*/

#ifdef RAYLIB_H
#define Vector2 VecV2
#define Vector3 VecV3
#define Vector4 VecV4
#else
#define VecV2 Vector2
#define VecV3 Vector3
#define VecV4 Vector4
#endif

#include "collection/algebra.hpp"
#include "collection/definitions.hpp"
#include "collection/helper.hpp"

#ifdef _$_FLOAT_AS_DOUBLE
#define float double
#endif // _$_FLOAT_AS_DOUBLE

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

#ifdef _$_FLOAT_AS_DOUBLE
#undef float
#endif // _$_FLOAT_AS_DOUBLE

#ifdef RAYLIB_H
#undef Vector2
#undef Vector3
#undef Vector4
#endif

#include "fileloader.hpp"

/*
*	[+]=[   [Headers END]   ]=[+]
*/

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

#endif // ANCHORED_HEADERS_H
