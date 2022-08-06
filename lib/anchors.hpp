#ifndef ANCHORED_HEADERS_H
#define ANCHORED_HEADERS_H

#define $print(TEXT) std::cout << TEXT << std::endl

#ifdef _DEBUG_OUTPUT_
#include <iostream>
#define $debug(TEXT) $print(TEXT)
#else
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

/*
*********************
*                   *
*  Graphical stuff  *
*                   *
*********************
*/

//#include <SFML/Graphics.hpp>
//#include <raylib.h>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

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

#ifdef _$_FLOAT_AS_DOUBLE
#define float double
#endif // _$_FLOAT_AS_DOUBLE

#include "collection/entity/entity.hpp"
#include "collection/vectorn.hpp"
#include "collection/cyclical.hpp"
//#include "collection/matrices.hpp"
#include "collection/tween.hpp"
#include "collection/event.hpp"

#ifdef _$_FLOAT_AS_DOUBLE
#undef float
#endif // _$_FLOAT_AS_DOUBLE

#ifdef RAYLIB_H
#undef Vector2
#undef Vector3
#undef Vector4
#endif

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
#define NEXT	;
#define LESS	<
#define MORE	>
#define IS		==
#define EQUALS	=
#endif // _$_LUA_STYLE_PROGRAMMING

#define $func(...)	[&](...) -> void

#endif // ANCHORED_HEADERS_H
