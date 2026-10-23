#ifndef MAKAILIB_EX_GAME_DIALOG_CORE_H
#define MAKAILIB_EX_GAME_DIALOG_CORE_H

#include <makai/makai.hpp>

/// @brief Game dialog extensions.
namespace Makai::Ex::Game::Dialog {
	struct TextContent {
		UTF8String title;
		UTF8String text;
	};

	struct IAnimatable {
		virtual ~IAnimatable() {}

		virtual usize play(usize const animation) = 0;
	};

	struct IPresentable {
		virtual ~IPresentable() {}

		virtual usize enter()		= 0;
		virtual usize exit()		= 0;
		virtual usize present()		= 0;
		virtual usize withdraw()	= 0;
	};

	struct ITextDisplay {
		virtual ~ITextDisplay() {}

		virtual usize title(UTF8String const& content)		= 0;
		virtual usize text(UTF8String const& content)		= 0;
		virtual usize display(TextContent const& content)	= 0;
	};

	struct ITalkable {
		virtual ~ITalkable() {}

		virtual usize say(TextContent const& content) = 0;
		virtual usize add(TextContent const& content) = 0;
	};

	struct IActive {
		virtual ~IActive() {}

		virtual usize act(usize const action) = 0;
	};

	struct IEmotive {
		virtual ~IEmotive() {}

		virtual usize emote(usize const emotion) = 0;
	};

	struct IActor:	IVisible, IAnimatable, IPresentable, ITalkable, IActive, IEmotive	{};
	struct IBox:	IVisible, IAnimatable, IPresentable, ITextDisplay					{};
}

#endif
