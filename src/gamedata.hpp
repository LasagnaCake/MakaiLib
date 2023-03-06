#ifndef MAKAI_GAME_SYSTEM_DATA
#define MAKAI_GAME_SYSTEM_DATA

#include "anchors.hpp"
#include "graphical.hpp"
#include "program.hpp"

#define DERIVED_CONSTRUCTOR(NAME, BASE, CODE)\
	NAME(string name = #NAME, bool uniqueEntity = true) : BASE(name, uniqueEntity) CODE\
	NAME(Entity* parent, string name = #NAME , bool uniqueEntity = true) : BASE(parent, name, uniqueEntity) CODE

#define DERIVED_CLASS(NAME, BASE)\
	inline	virtual string getClass() {return #NAME;}\
	inline	virtual string getBaseClass() {return #BASE;}\
	inline	static string getCoreClass() {return BASE::getCoreClass();}\
	virtual	~NAME() {onDelete(); removeFromTree(); removeFromAllGroups();}
	// NOTE: This is the WORST way this could have been done, but it works I guess

#include "gamedata/layout.hpp"

#ifndef GAME_PARALLEL_THREAD_COUNT
#define GAME_PARALLEL_THREAD_COUNT PARALLEL_THREAD_COUNT
#endif // GAME_PARALLEL_THREAD_COUNT

#ifndef GAME_PARALLEL_FOR
#define GAME_PARALLEL_FOR PRAGMA_PARALLEL_FOR(GAME_PARALLEL_THREAD_COUNT)
#endif // GAME_PARALLEL_FOR

namespace GameData {
	#define $gdt GameData::
	namespace {
		using
		$ecl AreaCircle2D,
		$ecl Entity2D,
		$ecl Entity,
		$rdt Renderable,
		$txt Label,
		$ref AnimatedPlane,
		$drw Texture2D,
		$vmt Transform2D,
		$vmt Transform3D,
		$mat WorldMaterial,
		$mat BufferMaterial,
		$mat ObjectMaterial,
		$mat setMaterial,
		$cdt AreaCollisionData,
		$cdt CollisionType,
		$evt Signal,
		$mki InputManager,
		std::string;

		using namespace Vector;
		using namespace RenderData::Bar;

		namespace Reference {using namespace RenderData::Reference;}

		namespace Material {using namespace RenderData::Material;}
	}

	typedef std::unordered_map<string, SDL_Scancode> KeyBinds;

	template <typename T>
	using TypedSignal = $tev Signal<T>;

	void addToGame(Entity* e, std::string gameType) {
		if (!EntityClass::$_ROOT)
			throw std::runtime_error("Root wasn't created!");
		Entity* game = $ecl $_ROOT->getChild(gameType);
		if (game == nullptr) {
			game = new $ecl Entity(gameType);
			$ecl $_ROOT->addChild(game);
		}
		game->addChild(e);
	}

	void setWorldMaterial(WorldMaterial& mat) {
		setMaterial($mainshader, mat);
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

#define GAME_CLASS(NAME, BASE)\
	inline	virtual String getClass() {return #NAME;}\
	inline	virtual String getBaseClass() {return #BASE;}\
	inline	static String getCoreClass() {return BASE::getCoreClass();}\
	virtual	~NAME() {onDelete(); removeFromTree(); removeFromAllGroups();}\
	using	BASE::BASE;

#endif // MAKAI_GAME_SYSTEM_DATA
