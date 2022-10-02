#ifndef MAKAI_GAME_SYSTEM_DATA
#define MAKAI_GAME_SYSTEM_DATA

#include "anchors.hpp"

#define DERIVED_CONSTRUCTOR(NAME, BASE, CODE)\
	NAME(string name = #NAME) : BASE(name) CODE\
	NAME(Entity* parent, string name = #NAME , bool uniqueEntity = true) : BASE(parent, name, uniqueEntity) CODE

#define DERIVED_CLASS(NAME, BASE)\
	inline	virtual string getClass() {return #NAME;}\
	inline	virtual string getBaseClass() {return #BASE;}\
	inline	static string getCoreClass() {return BASE::getCoreClass();}

#include "gamedata/danmaku/layout.hpp"

namespace GameData {
	namespace {
		using
		$ecl AreaCircle2D,
		$ecl Entity2D,
		$ecl Entity,
		$rdt Renderable,
		$ref AnimatedPlane,
		Drawer::Texture2D,
		VecMath::Transform2D,
		VecMath::Transform3D,
		std::string;

		using namespace Vector;

		namespace Reference {using namespace RenderData::Reference;};
	}

	typedef std::map<string, SDL_Scancode> KeyBinds;

	#include "gamedata/danmaku/laser.hpp"
	#include "gamedata/danmaku/spellcard.hpp"
	#include "gamedata/danmaku/bullet.hpp"
	#include "gamedata/player.hpp"
}

#define $gdt GameData::

#undef DERIVED_CONSTRUCTOR
#undef DERIVED_CLASS

#endif // MAKAI_GAME_SYSTEM_DATA
