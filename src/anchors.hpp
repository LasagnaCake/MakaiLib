#ifndef ANCHORED_HEADERS_H
#define ANCHORED_HEADERS_H

#define $enforce(WHAT, CHECK) if(!(CHECK)) throw Error::FailedAction(std::string("Failed to initialize ") + WHAT)

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

#include <cstdint>

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
#include <nlohmann/json.hpp>

/*
********************
*                  *
*  Encoding stuff  *
*                  *
********************
*/

#include <cppcodec/base64_rfc4648.hpp>
#include <cppcodec/base32_rfc4648.hpp>

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
#include <SDL2/SDL_mixer.h>
/*
***************************
*                         *
*  Engine-specific stuff  *
*                         *
***************************
*/

#if INTPTR_MAX == INT64_MAX
#define size_t uint64_t
#elif INTPTR_MAX == INT32_MAX
#define size_t uint32_t
#else
#error "This engine was designed for at least 32 bit processors."
#endif

#include "collection/types.hpp"
#include "collection/conceptual.hpp"
#include "collection/referential.hpp"
#include "collection/algebra.hpp"
#include "collection/definitions.hpp"
#include "collection/helper.hpp"
#include "collection/errors.hpp"
#include "collection/entity/entity.hpp"
#include "collection/vectorn.hpp"
#include "collection/cyclical.hpp"
#include "collection/tween.hpp"
#include "collection/event.hpp"
#include "collection/tasking.hpp"
#include "collection/grouping.hpp"
#include "collection/dictionary.hpp"
#include "collection/sharedvalue.hpp"

#include "fileloader.hpp"
#include "predefs.hpp"

#ifdef _USE_OPENMP_
#ifndef _DEBUG_OUTPUT_
#include <omp.h>
#endif // _DEBUG_OUTPUT_
#endif // _USE_OPENMP_

#ifdef _OMP_H
#ifndef PARALLEL_THREAD_COUNT
#define PARALLEL_THREAD_COUNT 1
#endif // PARALLEL_THREAD_COUNT
#define PRAGMA_PARALLEL_FOR(THREADS) _Pragma("omp parallel for num_threads(" #THREADS ")")
#else
#define PARALLEL_THREAD_COUNT 0
#define PRAGMA_PARALLEL_FOR(THREADS)
#endif // PARALLEL_THREAD_COUNT

#define PARALLEL_FOR PRAGMA_PARALLEL_FOR(PARALLEL_THREAD_COUNT)

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
