#ifndef MAX_INSTANCE_COUNT
#define MAX_INSTANCE_COUNT 4096
#endif // MAX_INSTANCE_COUNT

#ifndef PLAYER_LASER_COUNT
#define PLAYER_LASER_COUNT 16
#endif // PLAYER_LASER_COUNT

#ifndef PLAYER_BULLET_COUNT
#define PLAYER_BULLET_COUNT 128
#endif // PLAYER_BULLET_COUNT

#ifndef ENEMY_LASER_COUNT
#define ENEMY_LASER_COUNT 64
#endif // ENEMY_LASER_COUNT

#ifndef ENEMY_BULLET_COUNT
#define ENEMY_BULLET_COUNT MAX_INSTANCE_COUNT - (PLAYER_BULLET_COUNT + PLAYER_LASER_COUNT + ENEMY_LASER_COUNT)
#endif // ENEMY_BULLET_COUNT

#define $getman(TYPE) $dmk TYPE##Manager
#define $setman(TYPE) $dmk TYPE##Manager = &

#define UPDATE_PARAM(PNAME) \
	if (params.PNAME.omega) { \
		params.PNAME.factor = Math::clamp(params.PNAME.factor + params.PNAME.omega, 0.0f, 1.0f); \
		float vf = params.PNAME.easing(params.PNAME.factor, 0.0, 1.0, 1.0f); \
		params.PNAME.current = $mth lerp( \
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
#define INVINCIBLE_COLOR $vec4(Color::SEMISOLID).compensated()
#endif // INVINCIBLE_COLOR

namespace ItemType {
	enum ItemType: size_t {
		POWER_ITEM,
		POINT_ITEM,
		EXTRA_ITEM,
		BOMB_ITEM,
		LIFE_ITEM
	};
};

#define $item(NAME) ItemType::NAME##_ITEM
