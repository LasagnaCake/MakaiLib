/*
*******************
*                 *
*  General stuff  *
*                 *
*******************
*/

#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <math.h>

/*
***************************
*                         *
*  Engine-specific stuff  *
*                         *
***************************
*/

#ifdef _$_FLOAT_AS_DOUBLE
#define float double
#endif // _$_FLOAT_AS_DOUBLE

#include <collection/entity/entity.hpp>
#include <collection/vectorn.hpp>
#include <collection/cyclical.hpp>
#include <collection/algebra.hpp>
#include <collection/matrices.hpp>
#include <collection/event.hpp>

#ifdef _$_FLOAT_AS_DOUBLE
#undef float
#endif // _$_FLOAT_AS_DOUBLE

/*
*********************
*                   *
*  Graphical stuff  *
*                   *
*********************
*/

#include <include/Ogre.h>
#include <include/GLFW/glfw3.h>

/*
#define IF		if (
#define END		}
#define ELSE	} else
#define AND		&&
#define OR		||
#define NOT		!
#define THEN	)
#define EXECUTE	{
//*/

