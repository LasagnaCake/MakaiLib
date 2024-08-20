#ifndef MAKAILIB_AUDIO_SOUND_H
#define MAKAILIB_AUDIO_SOUND_H

#include "core.hpp"
#include "playable.hpp"

namespace Makai::Sound {
	class Sound: public Playable {
	public:
		using Playable::Playable;

		virtual ~Sound() final;

		static void stopAll(usize const& fade = 0);

		static void pauseAll();

		static void resumeAll();

		static void setMasterVolume(schar const& volume);

		static schar getMasterVolume();

		void play(
			int const&		loops		= 0,
			usize const&	fadeInTime	= 0,
			bool const&		force		= false
		);

		void playOnceThisFrame(
			int const&		loops		= 0,
			usize const&	fadeInTime	= 0,
			bool const&		force		= false
		);

		void playOnceAndWait(
			int const&		loops		= 0,
			usize const&	fadeInTime	= 0,
			bool const&		force		= false,
			usize const&	cycles		= 0
		);

		void stop(usize const& fadeOutTime = 0);

		void pause();

		void resume();

		void setVolume(uchar const& volume);

		int getVolume();

	protected:
		void onCreate(Extern::Resource const& data) final override;

		void onDestroy() final override;

		void onUpdate() final override;

	private:
		bool canPlayThisFrame = true;

		usize delay = 0;

		int getChannel();

		bool active();

		int					channel = -1;
		Extern::Resource	source	= NULL;
	};
}

#endif // MAKAILIB_AUDIO_SOUND_H
