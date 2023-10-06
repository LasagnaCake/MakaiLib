#ifndef MAKAI_GAME_SYSTEM_LAYOUT
#define MAKAI_GAME_SYSTEM_LAYOUT

#ifndef SUBLAYER_COUNT
#define SUBLAYER_COUNT 8
#endif // SUBLAYER_COUNT

#ifndef UI_LAYER_OFFSET
#define UI_LAYER_OFFSET 32
#endif // UI_LAYER_OFFSET

#define TRUE_SUBLAYER_COUNT (SUBLAYER_COUNT * 2 + 1)

#define R_LAYER(NAME) \
	NAME##_BOTTOM_LAYER, \
    NAME##_LAYER		= (NAME##_BOTTOM_LAYER + (SUBLAYER_COUNT)),\
    NAME##_TOP_LAYER	= (NAME##_LAYER + (SUBLAYER_COUNT))\

#define CR_LAYER(NAME, INDEX) \
	NAME##_BOTTOM_LAYER	= (INDEX),\
	NAME##_LAYER		= (NAME##_BOTTOM_LAYER + (SUBLAYER_COUNT)),\
	NAME##_TOP_LAYER	= (NAME##_LAYER + (SUBLAYER_COUNT))

enum Layer: size_t {
	R_LAYER(WORLD),
	R_LAYER(WORLD_OVERLAY),
	#ifdef $_DANMAKU_GAME
	R_LAYER(BOSS_SPELL),
	R_LAYER(BOSS_SPELL_OVERLAY),
	R_LAYER(SPELL),
	R_LAYER(SPELL_OVERLAY),
	R_LAYER(PLAYER_BULLET),
	R_LAYER(PLAYER_LASER),
	R_LAYER(PLAYER_BOMB),
	R_LAYER(PLAYER_OPTION),
	R_LAYER(PLAYER),
	R_LAYER(ENEMY),
	R_LAYER(ENEMY_BULLET),
	R_LAYER(ENEMY_LASER),
	R_LAYER(ITEM),
	R_LAYER(PLAYER_HITBOX),
	R_LAYER(STAGE_CARD),
	#endif
	CR_LAYER(OVERLAY, UI_LAYER_OFFSET * TRUE_SUBLAYER_COUNT),
	CR_LAYER(UI, OVERLAY_TOP_LAYER),
	CR_LAYER(DIALOG, UI_TOP_LAYER),
	CR_LAYER(MENU, DIALOG_TOP_LAYER),
};

#undef R_LAYER
#undef CR_LAYER

#endif // MAKAI_GAME_SYSTEM_LAYOUT
