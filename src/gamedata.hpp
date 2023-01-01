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
	inline	static string getCoreClass() {return BASE::getCoreClass();}\
	virtual	~NAME() {onDelete(); removeFromTree();};
	// NOTE: This is the WORST way this could have been done. But this obstinate fuck of a language is an obstinate fuck, so be it.

#ifdef $_DANMAKU_GAME
#include "gamedata/danmaku/layout.hpp"
#endif

namespace GameData {
	#define $gdt GameData::
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
		$mat WorldMaterial,
		std::string;

		using namespace Vector;

		namespace Reference {using namespace RenderData::Reference;}

		namespace Material {using namespace RenderData::Material;}
	}

	typedef std::unordered_map<string, SDL_Scancode> KeyBinds;

	void addToGame(Entity* e, std::string gameType) {
		Entity* game = $ecl $_ROOT[gameType];
		if (game == nullptr) {
			game = new $ecl Entity(gameType);
			$ecl $_ROOT.addChild(game);
		}
		game->addChild(e);
	}

	#include "gamedata/dialogue.hpp"
	#include "gamedata/saving.hpp"
	#include "gamedata/animation.hpp"

	#ifdef $_DANMAKU_GAME

	namespace Danmaku {
		#define $dmk GameData::Danmaku::
		AreaCircle2D* mainPlayer = nullptr;
		#include "gamedata/danmaku/predefs.hpp"
		#include "gamedata/danmaku/structs.hpp"
		#include "gamedata/danmaku/bullet.hpp"
		#include "gamedata/danmaku/laser.hpp"
		#include "gamedata/danmaku/player.hpp"
		#include "gamedata/danmaku/item.hpp"
		#include "gamedata/danmaku/spellcard.hpp"
		#include "gamedata/danmaku/enemy.hpp"
		#include "gamedata/danmaku/boss.hpp"
		#include "gamedata/danmaku/stage.hpp"
		#include "gamedata/danmaku/program.hpp"
	}

	#endif
}

#undef DERIVED_CONSTRUCTOR
#undef DERIVED_CLASS

#endif // MAKAI_GAME_SYSTEM_DATA
