#ifndef MAKAI_GAME_SYSTEM_DATA
#define MAKAI_GAME_SYSTEM_DATA

#include "anchors.hpp"
#include "graphical.hpp"
#include "program.hpp"

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
		$drw Texture2D,
		$vmt Transform2D,
		$vmt Transform3D,
		std::string;

		using namespace Vector;

		namespace Reference {using namespace RenderData::Reference;};
	}

	typedef std::map<string, SDL_Scancode> KeyBinds;

	void addToGame(Entity* e, std::string gameType) {
		Entity* game = $ecl $_ROOT[gameType];
		if (game == nullptr) {
			game = new $ecl Entity(gameType);
			$ecl $_ROOT.addChild(game);
		}
		game->addChild(e);
	}

	namespace Danmaku {
		#include "gamedata/danmaku/laser.hpp"
		#include "gamedata/danmaku/spellcard.hpp"
		#include "gamedata/danmaku/bullet.hpp"
		#include "gamedata/danmaku/player.hpp"
	}
}

#define $gdt GameData::
#define $dmk GameData::Danmaku::

#undef DERIVED_CONSTRUCTOR
#undef DERIVED_CLASS

#endif // MAKAI_GAME_SYSTEM_DATA
