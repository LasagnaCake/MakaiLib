#ifndef AUDIO_PLAYER_SYSTEM_H
#define AUDIO_PLAYER_SYSTEM_H

#include "../anchors.hpp"

#define $yse(COMPONENT) YSE::COMPONENT()

namespace Audio {

	namespace {
		using
		TypedEvent::Signal,
		std::string,
		std::vector;

		vector<const Signal<float>*> soundList;
	}

	void updateSounds(float delta) {
		for(const Signal<float>* func : soundList) (*func)(delta);
	}

	namespace Base {
		struct Playable {
			Playable() {
				soundList.push_back(&update);
			}
			Playable(String path) {

			}
			virtual ~Playable() {
				for $ssrange(i, 0, soundList.size())
					if (soundList[i] == &update) {
						soundList.erase(soundList.begin() + i);
						break;
				}
				destroy();
			}
			void create(String path)	{
				if (created) return;
				onCreate(path);
				created = true;
			};
			void destroy()				{
				if (!created) return;
				onDestroy();
				created = false;
			};
			virtual void onCreate(String path)	= 0;
			virtual void onDestroy()			= 0;
			virtual void onPlaybackEnd()		{}
			virtual void onPlaybackStart()		{}
			virtual void onUpdate(float delta)	{}
			const Signal<float> update = [&] (float delta) {
				this->onUpdate(delta);
			};
		private:
			bool created = false;
		};
	}

	struct LoopData {
		float	start	= 0;
		float	stop	= -1;
		bool	enabled	= false;
		long	count	= -1;
	};

	struct Music: public Base::Playable {
		float		start = 0;
		float		stop = -1;
		LoopData	loop;
		bool		stopOnFadeOut = true;
		void create(String path) override {
			if()
			source = Mix_LoadMUS();
		}
		void destroy() override {
		}
		void onUpdate(float delta) override {
		}
		void play(float fadeInTime = 0) {
		}
		void switchTo(float fadeOutTime = 0, float fadeInTime = 0) {
		}
		void crossfadeIn(float time = 0) {
		}
	private:
		Mix_Music* source;
	};
}

#endif // AUDIO_PLAYER_SYSTEM_H
