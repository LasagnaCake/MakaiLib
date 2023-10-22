#ifndef MAX_INSTANCE_COUNT
//#define MAX_INSTANCE_COUNT 4096
#define MAX_INSTANCE_COUNT 3072
#endif // MAX_INSTANCE_COUNT

#ifndef PLAYER_LASER_COUNT
#ifndef MAKAILIB_DANMAKU_REDUCED_GAME_INSTANCE_COUNT
#define PLAYER_LASER_COUNT 16
#else
#define PLAYER_LASER_COUNT 8
#endif
#endif // PLAYER_LASER_COUNT

#ifndef PLAYER_BULLET_COUNT
#ifndef MAKAILIB_DANMAKU_REDUCED_GAME_INSTANCE_COUNT
#define PLAYER_BULLET_COUNT 256
#else
#define PLAYER_BULLET_COUNT 128
#endif
#endif // PLAYER_BULLET_COUNT

#ifndef ENEMY_LASER_COUNT
#ifndef MAKAILIB_DANMAKU_REDUCED_GAME_INSTANCE_COUNT
#define ENEMY_LASER_COUNT 64
#else
#define ENEMY_LASER_COUNT 32
#endif
#endif // ENEMY_LASER_COUNT

#ifndef ENEMY_BULLET_COUNT
#ifndef MAKAILIB_DANMAKU_REDUCED_GAME_INSTANCE_COUNT
//#define ENEMY_BULLET_COUNT ((MAX_INSTANCE_COUNT) - ((PLAYER_BULLET_COUNT) + ((PLAYER_LASER_COUNT) + (ENEMY_LASER_COUNT)) * 4))
#define ENEMY_BULLET_COUNT 2048
#else
//#define ENEMY_BULLET_COUNT 1536
#define ENEMY_BULLET_COUNT 1024
//#define ENEMY_BULLET_COUNT 512
#endif
#endif // ENEMY_BULLET_COUNT

#ifndef COLLECTIBLE_ITEM_COUNT
#define COLLECTIBLE_ITEM_COUNT 256
#endif // COLLECTIBLE_ITEM_COUNT

#ifndef DANMAKU_PARAMS_COUNT
#define DANMAKU_PARAMS_COUNT 8
#endif // DANMAKU_PARAMS_COUNT

#if ((MAX_INSTANCE_COUNT) < ((PLAYER_BULLET_COUNT) + (ENEMY_BULLET_COUNT) + ((PLAYER_LASER_COUNT) + (ENEMY_LASER_COUNT)) * 4))
#error "Too many instances!"
#endif

#define UPDATE_PARAM(PNAME) \
	if (params.PNAME.omega) { \
		params.PNAME.factor = Math::clamp(params.PNAME.factor + params.PNAME.omega * (delta * 100.0f), 0.0f, 1.0f); \
		float vf = params.PNAME.easing(params.PNAME.factor, 0.0, 1.0, 1.0f); \
		params.PNAME.current = Math::lerp( \
			params.PNAME.start, \
			params.PNAME.end, \
			vf \
		); \
	}

#define DANMAKU_FRAME_BEGIN \
	if (free) return; \
	DanmakuObject::onFrame(delta); \
	if (pause.enabled) return

#ifndef INVINCIBLE_COLOR
#define INVINCIBLE_COLOR Color::LUCENT
#endif // INVINCIBLE_COLOR

namespace ItemType {
	enum ItemType: size_t {
		POWER_ITEM = 0,
		FULL_POWER_ITEM,
		POINT_ITEM,
		EXTRA_ITEM,
		LIFE_ITEM,
		LIFE_BIT_ITEM,
		BOMB_ITEM,
		BOMB_BIT_ITEM
	};
};
