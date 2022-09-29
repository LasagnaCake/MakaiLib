#ifndef MAKAI_GAME_SYSTEM_LAYOUT
#define MAKAI_GAME_SYSTEM_LAYOUT

#ifndef SUBLAYER_COUNT
#define SUBLAYER_COUNT 8
#endif // SUBLAYER_COUNT

#define R_LAYER(NAME) \
	NAME##_$_$, \
    NAME##_LAYER = (NAME##_$_$ + (SUBLAYER_COUNT - 1))

#define $sublayer(INDEX) ((INDEX) - (SUBLAYER_COUNT - 1))

#define BULLET_MANAGER_GROUP 1024

namespace RenderLayer {
	enum RenderLayer: size_t {
		R_LAYER(WORLD),
		R_LAYER(PLAYER),
		R_LAYER(PLAYER_BULLET),
		R_LAYER(ENEMY),
		R_LAYER(ENEMY_BULLET),
		R_LAYER(ITEM),
		R_LAYER(PLAYER_HITBOX),
		R_LAYER(UI),
		R_LAYER(DIALOG)
	};
}

#undef R_LAYER
#define $layer(NAME) RenderLayer::NAME##_LAYER

#endif // MAKAI_GAME_SYSTEM_LAYOUT
