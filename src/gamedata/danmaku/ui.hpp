typedef Helper::FloatFormat<float> PowerFormat;

typedef Function<String(size_t)> PowerStringFunc;

template<
	BarType POWER_T		= LinearBar,
	BarType LIFE_T		= LinearBar,
	BarType BOMB_T		= LinearBar,
	BarType LIFE_BIT_T	= LinearBar,
	BarType BOMB_BIT_T	= LinearBar
>
struct GameUI {
	typedef POWER_T		PowerBarType;
	typedef LIFE_T		LifeBarType;
	typedef BOMB_T		BombBarType;
	typedef LIFE_BIT_T	LifeBitBarType;
	typedef BOMB_BIT_T	BombBitBarType;

	LabeledBar<POWER_T>	power;
	LIFE_T				life;
	BOMB_T				bomb;
	LIFE_BIT_T			lifeBit;
	BOMB_BIT_T			bombBit;
	Label				score, hiScore, point, graze;
	Label				grazeTitle, pointTitle, scoreTitle, hiScoreTitle, difficulty;

	Texture2D		fontTX;
	FontData		font{&fontTX};

	GameUI() {
		power.bar.trans.position.z = 0.1;
		power.bar.setRenderLayer(UI_LAYER);
		power.label.setRenderLayer(UI_LAYER);
		life.setRenderLayer(UI_LAYER);
		bomb.setRenderLayer(UI_LAYER);
		lifeBit.setRenderLayer(UI_LAYER);
		bombBit.setRenderLayer(UI_LAYER);
		score.setRenderLayer(UI_LAYER);
		hiScore.setRenderLayer(UI_LAYER);
		point.setRenderLayer(UI_LAYER);
		graze.setRenderLayer(UI_LAYER);
		grazeTitle.setRenderLayer(UI_LAYER);
		pointTitle.setRenderLayer(UI_LAYER);
		scoreTitle.setRenderLayer(UI_LAYER);
		hiScoreTitle.setRenderLayer(UI_LAYER);
		difficulty.font		=
		hiScoreTitle.font	=
		scoreTitle.font		=
		grazeTitle.font		=
		pointTitle.font		=
		power.label.font	=
		score.font			=
		hiScore.font		=
		point.font			=
		graze.font			= &font;
		power.label.text	= TextData{
			"0.00",
			TextRect{4,1},
			Vector2(0.5, 0.5)
		};
		score.text		=
		hiScore.text	=
		point.text		=
		graze.text		= TextData{
			"0",
			TextRect{10, 1},
			Vec2(1.0, 0.5)
		};
		difficulty.text		=
		scoreTitle.text		=
		hiScoreTitle.text	=
		grazeTitle.text		=
		pointTitle.text		= TextData{
			"0",
			TextRect{10, 1},
			Vector2(0, 0.5)
		};
		difficulty.text.content		= "NULL";
		pointTitle.text.content		= "Point";
		grazeTitle.text.content		= "Graze";
		scoreTitle.text.content		= "Score";
		hiScoreTitle.text.content	= "HiScore";
	}

	void show()	{	setVisible(true);	}

	void hide()	{	setVisible(false);	}

	void setVisible(bool visibility = true) {
		power.bar.active	= visibility;
		power.label.active	= visibility;
		life.active			= visibility;
		bomb.active			= visibility;
		scoreTitle.active	= visibility;
		hiScoreTitle.active	= visibility;
		grazeTitle.active	= visibility;
		pointTitle.active	= visibility;
		bomb.active			= visibility;
		lifeBit.active		= visibility;
		bombBit.active		= visibility;
		score.active		= visibility;
		hiScore.active		= visibility;
		point.active		= visibility;
		graze.active		= visibility;
	}

	void setUIMaximums(
		size_t maxPower		= 400,
		size_t maxLife		= 9,
		size_t maxBomb		= 9,
		size_t maxLifeBit	= 5,
		size_t maxBombBit	= 5
	) {
		power.bar.max	= maxPower;
		life.max		= maxLife;
		bomb.max		= maxBomb;
		lifeBit.max		= maxLifeBit;
		bombBit.max		= maxBombBit;
	}

	void setUIValues(PlayerData const& data) {
		power.bar.value				= data.power;
		power.label.text.content	= onPowerStringRequest(data.power);
		point.text.content			= toString(data.point);
		graze.text.content			= toString(data.graze);
		score.text.content			= toString(data.score);
		life.value					= data.life;
		bomb.value					= data.bomb;
		lifeBit.value				= data.lifeBit;
		bombBit.value				= data.bombBit;
	}

	void setCharacterSpacing(Vec2 kerning) {
		difficulty.text.spacing		=
		grazeTitle.text.spacing		=
		pointTitle.text.spacing		=
		scoreTitle.text.spacing		=
		hiScoreTitle.text.spacing	=
		score.text.spacing			=
		point.text.spacing			=
		hiScore.text.spacing		=
		power.label.text.spacing	=
		graze.text.spacing			= kerning;
	}

	void setHighScore(size_t score) {hiScore.text.content = toString(score);}

	void setDifficulty(String difficulty) {hiScore.text.content = difficulty;}

	PowerStringFunc onPowerStringRequest = [](size_t p){return toString(p);};
};

/// "Single Bar Type" Game UI alias.
template<BarType T = LinearBar>
using SimpleGameUI = GameUI<T, T, T, T, T>;


/// "Power, Resources, Bits" Game UI alias.
template<
	BarType POWER		= LinearBar,
	BarType RESOURCES	= LinearBar,
	BarType BITS		= LinearBar
>
using PRBGameUI = GameUI<POWER, RESOURCES, RESOURCES, BITS, BITS>;

/// "Power, Life, Bomb" Game UI alias.
template<
	BarType POWER	= LinearBar,
	BarType LIFE	= LinearBar,
	BarType BOMB	= LinearBar
>
using PLBGameUI = GameUI<POWER, LIFE, BOMB, LIFE, BOMB>;
