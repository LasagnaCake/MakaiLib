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
		namespace {
			Mix_Music*		current	= nullptr;
			const Signal*	queued	= nullptr;
		}

		void update() {
			if (!Mix_PlayingMusic()) {
				current = nullptr;
				if (queued) {
					(*queued)();
					queued = nullptr;
				}
			}
		}

		void stopMusic(size_t fadeOutTime = 0) {
			if (fadeOutTime)
				Mix_FadeOutMusic(fadeOutTime);
			else
				Mix_HaltMusic();
		}

		void pauseMusic() {
			Mix_PauseMusic();
		}

		void resumeMusic() {
			Mix_ResumeMusic();
		}

		void setVolume(signed char volume) {
			Mix_VolumeMusic(volume);
		}

		signed char getVolume() {
			return Mix_VolumeMusic(-1);
		}

		struct Music: public Base::Playable {
			using Playable::Playable;

			virtual ~Music() {}

			void onCreate(String path) override {
				source = Mix_LoadMUS(path.c_str());
				if (!source)
					throw std::runtime_error("Could not load file '" + path + "'!\n\n REASON: " + String(Mix_GetError()));
			}

			void onDestroy() override {
				if (queued = &onQueue)
					queued = nullptr;
				if (Mix_PlayingMusic() && current == source)
					Mix_HaltMusic();
				Mix_FreeMusic(source);
			}

			void onUpdate() override {
				if (current != source) return;
			}

			void play(int loops = 0, size_t fadeInTime = 0) {
				if (!created) return;
				this->loops		= loops;
				current	= source;
				if (fadeInTime)
					Mix_FadeInMusic(source, loops, fadeInTime);
				else
					Mix_PlayMusic(source, loops);
			}

			void switchTo(size_t fadeOutTime, size_t fadeInTime) {
				if (!created) return;
				this->fadeInTime	= fadeInTime;
				queueMusic();
				stopMusic(fadeOutTime);
			}

			void switchTo(size_t fadeOutTime, size_t fadeInTime, int loops) {
				if (!created) return;
				this->fadeInTime	= fadeInTime;
				this->loops			= loops;
				queueMusic();
				stopMusic(fadeOutTime);
			}

			void queueMusic() {
				queued = &onQueue;
			}

			inline Mix_Music* getSource() {
				return source;
			}
		private:
			size_t	fadeInTime	= 0;
			int		loops		= 0;

			const Signal onQueue = $signal {
				play(loops, fadeInTime);
			};

			Mix_Music* source = nullptr;
		};
	}

	void updateAudioSystem() {
		Music::update();
		for(const Signal* func : soundList) (*func)();
	}
}

#define $aud Audio::
#define $mus $aud Music::

#endif // AUDIO_PLAYER_SYSTEM_H
