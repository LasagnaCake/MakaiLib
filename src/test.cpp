#ifdef _$$_$$_
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

#include <collection/algebra.hpp>

#ifdef _$_FLOAT_AS_DOUBLE
#define float double
#endif // _$_FLOAT_AS_DOUBLE

#include <collection/entity/entity.hpp>
#include <collection/vectorn.hpp>
#include <collection/cyclical.hpp>
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

void banner(std::string message) {
	std::cout << std::endl;
	std::cout << "<< [[ ";
	std::cout << message;
	std::cout << " ]] >>";
	std::cout << std::endl;
	std::cout << std::endl;
}

void entityTest() {
	banner("Entity Test");
	using namespace EntityClass;
	using std::string;

	$_ROOT += new Entity3D("Parent");
	$_ROOT["Parent"]->addChild(new Entity3D("Child"));
	$_ROOT["Parent/Child"]->addChild(new Entity3D("GChild"));
	// TODO: The fucking game engine you prick
	((Entity3D*)$_ROOT["Parent/Child"])->rotation = Vector::Vector3(0, 0, 0);
	//((Entity3D*)$_ROOT.getChild("Parent"))->position = Vector::Vector3(1, 0, 0);
	//((Entity3D*)$_ROOT.getChild("Parent/Child"))->rotation = Vector::Vector3(0, Math::pi, 0);
	getEntity<Entity3D>("Parent/Child")->position = Vector::Vector3(1, 0, 0);
	std::cout << $_ROOT["Parent/Child"]->getClass() << std::endl;
	std::cout << $_ROOT["Parent/Child"]->getName() << std::endl;
	std::cout << $_ROOT["Parent/Child"]->getChild("GChild")->getName() << std::endl;
	std::cout << $_ROOT["@root/Parent/Child/GChild"]->getRoot()->getChild("Parent")->getName() << std::endl;
	Vector::Vector3 gp = getEntity<Entity3D>("Parent/Child/GChild")->globalPosition();
	std::cout << Math::ceil(gp.x, 3);
	std::cout << " ";
	std::cout << Math::ceil(gp.y, 3);
	std::cout << " ";
	std::cout << Math::ceil(gp.z, 3) << std::endl;

	getEntity<Entity3D>("Parent/Child")->deleteChildren();
}

void timerTest() {
	banner("Timer Test");
	Event::Timer t(10, true);
	Event::Signal s = $signal{
		std::cout << "[  OK!  ]" << std::endl;
	};
	t.onSignal = s;
	for (size_t i = 0; i < 100; i++) {
		std::cout << i << std::endl;
		t.yield();
	}
}

void randomTest() {
	banner("Random Test");
	std::cout << (Math::Random::trueRandom() ? "true" : "false") << std::endl;
	for (size_t i = 0; i < 10; i++) {
		std::cout << Math::Random::integer(0, 10) << std::endl;
		std::cout << Math::Random::real(0, 10) << std::endl;
	}
}

void stupidTest() {
	banner("Stupid Test");
	for (size_t i = 0; i < 10; i++) {
		float f = Math::Random::real(0, 100);
		std::cout << f << std::endl;
		std::cout << (int)f <<std::endl;
	}
}

int main() {
	entityTest();
	timerTest();
	randomTest();
	stupidTest();
	return 0;
}
#endif
