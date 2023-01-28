#ifndef AUDIO_PLAYER_SYSTEM_H
#define AUDIO_PLAYER_SYSTEM_H

#include "../anchors.hpp"

#define $yse(COMPONENT) YSE::COMPONENT()

namespace Audio {

	namespace {
		using
		Event::Signal,
		std::string,
		std::vector;

		vector<const Signal*> soundList;
	}

	void updateSounds() {
		for(const Signal* func : soundList) (*func)();
	}

	namespace Base {
		struct Playable {
			Playable() {
				soundList.push_back(&update);
			}
			virtual ~Playable() {
				for $ssrange(i, 0, soundList.size())
					if (soundList[i] == &update) {
						soundList.erase(soundList.begin() + i);
						break;
				}
			}
			virtual void onPlaybackEnd()	{}
			virtual void onPlaybackStart()	{}
			virtual void onFrame()			{}
			const Signal update = $signal{onFrame();};
		};
	}

	struct LoopData {
		float	start = 0;
		float	stop = -1;
		bool	enabled = false;
		long	count = -1;
	};

	struct Music: public Base::Playable {
		float		start = 0;
		float		stop = -1;
		LoopData	loop;
		bool		stopOnFadeOut = true;
		void fadeIn() {}
		void fadeOut() {}
	};
}

#endif // AUDIO_PLAYER_SYSTEM_H
