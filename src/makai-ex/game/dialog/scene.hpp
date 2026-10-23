#ifndef MAKAILIB_EX_GAME_DIALOG_SERVER_H
#define MAKAILIB_EX_GAME_DIALOG_SERVER_H

#include "../core/controlable.hpp"
#include "core.hpp"

/// @brief Game dialog extensions.
namespace Makai::Ex::Game::Dialog {
	struct AScene: AUpdateable, IPlayable, Controllable {
		using Script = Co::Generator<usize>;
		using Actors = Dictionary<Instance<IActor>>;

		struct Configuration {
			enum class Mode: u8 {
				GDSM_MANUAL,
				GDSM_USER_STEPPED,
				GDSM_AUTO
			};
			Mode	mode	= Mode::GDSM_USER_STEPPED;
			ssize	timeout = -1;
		} config;

		Input::Manager input;

		Actors actors;

		AScene(Configuration const& config = {}) config(config) {isFinished = true;}

		virtual ~AScene() {}

		template <Type::Derived<IActor> T>
		AScene& add(String const& name, Instance<T> const& actor) {
			actors[name] = actor;
		}

		virtual Script script() = 0;

		void onUpdate(float delta, App& app) override {
			if (isFinished) return;
			if (paused) return;
			if (delay > 0) {
				--delay;
				return;
			}
			if (!current)
				stop();
			switch (config.mode) {
				using enum Configuration::Mode;
				case GDSM_USER_STEPPED: if (!interaction()) return;
				default: break;
			}
			do {
				delay = current.next();
			}
			while (!delay && current);
		}

		AScene& start() override {
			if (!isFinished) return *this;
			current = script();
			isFinished = false;
			return *this;
		}

		AScene& stop() override {
			if (isFinished) return *this;
			isFinished = true;
			return *this;
		}

		AScene& play() override {
			if (isFinished) return start();
			paused = false;
			return *this;
		}

		AScene& pause() override {
			if (isFinished) return *this;
			paused = false;
			return *this;
		}

		bool interaction() const {
			return config.mode != Config::Mode::GDSM_AUTO && (userNext() or userSkip());
		}

	protected:
		Script userInput() const {
			co_return co_await userInput(config.timeout);
		}

		Script userInput(ssize timeout) const {
			co_return co_await action([&] {return interaction();}, timeout);
		}

		bool userNext() const {
			return action("next", true);
		}

		bool userSkip() const {
			return action("skip");
		}

		Script action(Signal<void> const& action, ssize timeout = -1) const {
			while (timeout && !action()) {
				if (timeout > 0) --timeout;
				co_yield 1;
			}
			co_return 0;
		}

	private:
		usize delay = 0;
		Script current;
	};

	struct ARTScene: AScene {
		Script script() override {
			// TODO: This
		}
	};
}

#endif
