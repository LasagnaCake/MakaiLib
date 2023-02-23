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

	namespace Base {
		class Playable {
		public:
			Playable() {
				soundList.push_back(&update);
			}

			Playable(const String& path): Playable() {
				create(path);
			}

			virtual ~Playable() {
				$debug("Deleting playable object...");
				for $ssrange(i, 0, soundList.size())
					if (soundList[i] == &update) {
						soundList.erase(soundList.begin() + i);
						break;
				}
				$debug("Object deleted!");
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
			virtual void onPlaybackEnd()	{}
			virtual void onPlaybackStart()	{}
			virtual void onUpdate()			{}

			const Signal update = $signal {
				this->onUpdate();
			};
		protected:
			bool created = false;
		};
	}

	namespace Music {
		#include "gl_music.hpp"
	}

	namespace Sound {
		#include "gl_sound.hpp"
	}

	void stopAll() {
		Mix_HaltMusic();
	}

	void updateAudioSystem() {
		Music::update();
		for(const Signal* func : soundList) (*func)();
	}
}

#define $aud Audio::
#define $mus $aud Music::

#endif // AUDIO_PLAYER_SYSTEM_H
