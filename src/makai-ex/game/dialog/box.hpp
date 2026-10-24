#ifndef MAKAILIB_EX_GAME_DIALOG_BOX_H
#define MAKAILIB_EX_GAME_DIALOG_BOX_H

#include "core.hpp"

/// @brief Game dialog extensions.
namespace Makai::Ex::Game::Dialog {
	struct Box: IBox, AUpdateable {
		Graph::Renderable background;
		Graph::UTF8Label textDisplay;

		struct Configuration {
			struct Layers {
				usize text;
				usize background;
			} layers;
			struct Animation {
				enum class Mode: uint8 {
					GDB_CDM_NONE,
					GDB_CDM_STEP_CHARACTER,
					GDB_CDM_STEP_WORD_FIXED,
					GDB_CDM_STEP_WORD_VARIABLE
				} mode = Mode::GDB_CDM_NONE;
				ssize speed = -1;
			} display;
		};

		Configuration::Animation animation = {};

		Box(Configuration const& config): animation(config.animation) {
			background.setRenderLayer(layers.background);
			textDisplay.setRenderLayer(layers.text);
		}

		void onUpdate(float delta, App& app) {}

		void show() override {}
		void hide() override {}

		usize enter() override {}
		usize exit() override {}
		usize present() override {}
		usize withdraw() override {}

		usize title(UTF8String const& what) override	{}
		usize text(UTF8String const& what) override		{}
		usize display(TextContent const& what) override	{}

		usize say(UTF8String const& what) override	{}
		usize add(UTF8String const& what) override	{}
	};
}

#endif
