#ifndef MAKAILIB_EX_GAME_DIALOG_SERVER_H
#define MAKAILIB_EX_GAME_DIALOG_SERVER_H

#include "core.hpp"

/// @brief Game dialog extensions.
namespace Makai::Ex::Game::Dialog {
	struct AScene: AUpdateable, IPlayable {
		using Script = Co::Generator<usize>;
		using Actors = List<Instance<IActor>>;

		Actors actors;

		AScene() {isFinished = true;}

		virtual ~AScene() {}

		template <Type::Derived<IActor> T>
		AScene& add(Instance<T> const& actor) {
			actors.pushBack(actor);
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
			do delay = current.next();
			while (!delay);
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

	private:
		usize delay = 0;
		Script current;
	};
}

#endif
