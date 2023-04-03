template<
	BarType POWER_T		= LinearBar,
	BarType LIFE_T		= LinearBar,
	BarType BOMB_T		= LinearBar,
	BarType LIFE_BIT_T	= LinearBar,
	BarType BOMB_BIT_T	= LinearBar
>
struct GameUI {
	LabeledBar<POWER_T>	power;
	LIFE_T				life;
	BOMB_T				bomb;
	LIFE_BIT_T			lifeBit;
	BOMB_BIT_T			bombBit;
	Label				score, hiScore, point, graze;

	Texture2D		fontTX;
	FontData		font{&fontTX};

	GameUI() {
		power.bar.setRenderLayer($layer(UI));
		life.setRenderLayer($layer(UI));
		bomb.setRenderLayer($layer(UI));
		lifeBit.setRenderLayer($layer(UI));
		bombBit.setRenderLayer($layer(UI));
		score.setRenderLayer($layer(UI));
		hiScore.setRenderLayer($layer(UI));
		point.setRenderLayer($layer(UI));
		graze.setRenderLayer($layer(UI));
		power.label.font	=
		score.font			=
		hiScore.font		=
		point.font			=
		graze.font			= &font;
		power.label.text	= TextData{
			"0.00",
			TextRect{4,1},
			AlignRect{HAlign::CENTER}
		};
		score.text		=
		hiScore.text	=
		point.text		=
		graze.text		= TextData{
			"0",
			TextRect{10, 1},
			AlignRect{HAlign::RIGHT}
		};
	}

	void setUIMaximums(
		PlayerData const& max = {
			MAXSIZE_T,
			400,
			MAXSIZE_T,
			MAXSIZE_T,
			9,
			9,
			MAXSIZE_T,
			1,
			1
		}
	) {
		power.bar.max	= max.power;
		life.max		= max.life;
		bomb.max		= max.bomb;
		lifeBit.max		= max.lifeBit;
		bombBit.max		= max.bombBit;
	}

	void setUIValues(PlayerData const& data) {
		power.bar.value				= data.power;
		power.label.text.content	= Helper::floatString(data.power, 2);
		point.text.content			= std::to_string(data.point);
		graze.text.content			= std::to_string(data.graze);
		score.text.content			= std::to_string(data.score);
		life.value					= data.life;
		bomb.value					= data.bomb;
		lifeBit.value				= data.lifeBit;
		bombBit.value				= data.bombBit;
	}

	void setHighScore(size_t score) {
		hiScore.text.content		= std::to_string(score);
	}
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
