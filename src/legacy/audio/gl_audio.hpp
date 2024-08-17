#ifndef AUDIO_PLAYER_SYSTEM_H
#define AUDIO_PLAYER_SYSTEM_H

#include "../anchors.hpp"

namespace Audio {

	namespace {
		using
		Event::Signal,
		std::string,
		std::vector;

		typedef const Signal AudioFunc;

		vector<AudioFunc*> audioList;

		bool isAudioSystemClosed = true;
	}

	namespace Base {
		class Playable {
		public:
			Playable() {
				audioList.push_back(&update);
			}

			Playable(String const& path): Playable() {
				create(path);
			}

			virtual ~Playable() {
				DEBUGLN("Deleting playable object...");
				ERASE_IF(audioList, elem == &update);
				DEBUGLN("Object deleted!");
			}

			void create(String const& path) {
				if (created) return;
				onCreate(path);
				created = true;
			};

			void destroy() {
				if (!created) return;
				onDestroy();
				created = false;
			};

			virtual void onPlaybackEnd()	{}
			virtual void onPlaybackStart()	{}

			bool exists() {return created;}

			AudioFunc update = SIGNAL {
				this->onUpdate();
			};

		protected:
			virtual void onCreate(String const& path)	= 0;
			virtual void onDestroy()					= 0;
			virtual void onUpdate()						{}

		private:
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
		Mix_HaltChannel(-1);
	}

	void openSystem() {
		Mix_OpenAudio(48000, AUDIO_F32SYS, 2, 1024);
		Mix_AllocateChannels(16);
		isAudioSystemClosed = false;
	}

	void closeSystem() {
		stopAll();
		isAudioSystemClosed = true;
		Mix_CloseAudio();
	}

	void updateAudioSystem() {
		//DEBUGLN("Updating music queue...");
		Music::update();
		//DEBUGLN("Updating audio systems...");
		for(AudioFunc*& func : audioList) (*func)();
	}

	bool isSystemOpen() {
		return !isAudioSystemClosed;
	}
}

#endif // AUDIO_PLAYER_SYSTEM_H
