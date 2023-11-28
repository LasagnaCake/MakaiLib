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
	inline	static string getCoreClass() {return BASE::getCoreClass();}
	// NOTE: This is the WORST way this could have been done, but it works I guess

#include "gamedata/layout.hpp"

#ifndef GAME_PARALLEL_THREAD_COUNT
#define GAME_PARALLEL_THREAD_COUNT PARALLEL_THREAD_COUNT
#endif // GAME_PARALLEL_THREAD_COUNT

#ifndef GAME_PARALLEL_FOR
#define GAME_PARALLEL_FOR PRAGMA_PARALLEL_FOR(GAME_PARALLEL_THREAD_COUNT)
#endif // GAME_PARALLEL_FOR

#define GAME_CLASS(NAME, BASE)\
	inline	virtual String getClass() {return #NAME;}\
	inline	virtual String getBaseClass() {return #BASE;}\
	inline	static String getCoreClass() {return BASE::getCoreClass();}\
	NAME(String name = #NAME, bool uniqueEntity = true) : BASE(name, uniqueEntity) {onCreate();}\
	NAME(EntityClass::Entity* parent, String name = #NAME , bool uniqueEntity = true) : BASE(parent, name, uniqueEntity) {onCreate();}

namespace GameData {
	namespace {
		using
		EntityClass::AreaCircle2D,
		EntityClass::Entity2D,
		EntityClass::Entity,
		RenderData::Renderable,
		RenderData::Reference::Plane,
		RenderData::Reference::AnimatedPlane,
		Drawer::Texture2D,
		VecMath::Transform2D,
		VecMath::Transform3D,
		RenderData::Material::WorldMaterial,
		RenderData::Material::BufferMaterial,
		RenderData::Material::ObjectMaterial,
		RenderData::Material::setMaterial,
		CollisionData::AreaCollisionData,
		CollisionData::CollisionType,
		Event::Signal,
		Makai::InputManager,
		Makai::Program,
		std::string;

		using namespace RenderData::Bar;
		using namespace RenderData::Text;

		namespace Reference {using namespace RenderData::Reference;}

		namespace Material {using namespace RenderData::Material;}
	}

	typedef std::unordered_map<String, SDL_Scancode> KeyBinds;

	template <typename T>
	using TypedSignal = TypedEvent::Signal<T>;

	void addToGame(Entity* const& e, String const& gameType) {
		if (!EntityClass::_ROOT)
			throw Error::NonexistentValue(
				"Root wasn't created!",
				__FILE__,
				toString(__LINE__),
				"GameData::addToGame()"
			);
		Entity* gameRoot = EntityClass::_ROOT->getChild("Game");
		if (gameRoot == nullptr) {
			gameRoot = new EntityClass::Entity("Game");
			EntityClass::_ROOT->addChild(gameRoot);
		}
		Entity* game = EntityClass::_ROOT->getChild(gameType);
		if (game == nullptr) {
			game = new EntityClass::Entity(gameType);
			gameRoot->addChild(game);
		}
		game->addChild(e);
	}

	Entity* getGame(String const& gameType) {
		return EntityClass::_ROOT->getChild("Game/"+gameType);
	}

	void setWorldMaterial(WorldMaterial& mat) {
		setMaterial(MAIN_SHADER, mat);
	}

	template <typename T> using Callback = Function<void(T&)>;

	#include "gamedata/program.hpp"
	#include "gamedata/dialogue.hpp"
	#include "gamedata/saving.hpp"
	#include "gamedata/animation.hpp"
	#include "gamedata/menu.hpp"

	#ifdef MAKAILIB_DANMAKU_GAME

	namespace Danmaku {
		namespace {
			using
				GameData::Material::PolarWarpEffect
			;
		}
		class BossEntity2D;
		class EnemyEntity2D;
		class PlayerEntity2D;
		class Stage;
		class DanmakuApp;
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
		#include "gamedata/danmaku/ui.hpp"
		#include "gamedata/danmaku/dialogue.hpp"
	}

	#endif
}

#undef DERIVED_CONSTRUCTOR
#undef DERIVED_CLASS

#endif // MAKAI_GAME_SYSTEM_DATA
