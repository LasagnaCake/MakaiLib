#ifndef MAKAI_GAME_SYSTEM_DATA
#define MAKAI_GAME_SYSTEM_DATA

#include "anchors.hpp"
#include "graphical.hpp"
#include "program.hpp"

#define DERIVED_CONSTRUCTOR(NAME, BASE, CODE)\
	/*protected:*/\
	NAME(string name = #NAME, bool uniqueEntity = true) : BASE(name, uniqueEntity) CODE\
	NAME(Entity* parent, string name = #NAME , bool uniqueEntity = true) : BASE(parent, name, uniqueEntity) CODE\
	/*private:*/

#define DERIVED_CLASS(NAME, BASE)\
	/*public:*/\
	inline	virtual string getClass() {return #NAME;}\
	inline	virtual string getBaseClass() {return #BASE;}\
	inline	static string getCoreClass() {return BASE::getCoreClass();}\
	virtual	~NAME() {onDelete(); removeFromTree(); removeFromAllGroups();};\
	/*private:*/
	// NOTE: This is the WORST way this could have been done, but it works I guess

#include "gamedata/layout.hpp"

#ifndef PARALLEL_THREAD_COUNT
#define PARALLEL_THREAD_COUNT 1
#endif // PARALLEL_THREAD_COUNT

#ifndef GAME_PARALLEL_FOR
#ifndef _DEBUG_OUTPUT_
#define GAME_PARALLEL_FOR _Pragma("omp parallel for num_threads(PARALLEL_THREAD_COUNT)")
#else
#define GAME_PARALLEL_FOR
#endif
#endif // GAME_PARALLEL_FOR

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
		$cdt AreaCollisionData,
		$cdt CollisionType,
		std::string;

		using namespace Vector;
		using namespace RenderData::Reference;
		using namespace SmartPointer;

		namespace Material {using namespace RenderData::Material;}
	}

	typedef std::unordered_map<string, SDL_Scancode> KeyBinds;

	void addToGame(Pointer<Entity> e, std::string gameType) {
		$debug("Adding '" + e->getName() + "' to '" + gameType + "'...");
		if (!EntityClass::$_ROOT)
			throw std::runtime_error("Root wasn't created!");
		Pointer<Entity> game = $ecl $_ROOT->getChild(gameType);
		if (!game) {
			game = new $ecl Entity(gameType);
			$ecl $_ROOT->addChild(game);
		}
		game->addChild(e);
	}

	template <typename T> using Callback = std::function<void(T&)>;

	#include "gamedata/dialogue.hpp"
	#include "gamedata/saving.hpp"
	#include "gamedata/animation.hpp"

	#ifdef $_DANMAKU_GAME

	namespace Danmaku {
		class BossEntity2D;
		class EnemyEntity2D;
		class PlayerEntity2D;
		class Stage;
		class DanmakuApp;
		#define $dmk GameData::Danmaku::
		Pointer<AreaCircle2D> mainPlayer;
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
