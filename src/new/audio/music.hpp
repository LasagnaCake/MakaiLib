#ifndef MAKAILIB_AUDIO_MUSIC_H
#define MAKAILIB_AUDIO_MUSIC_H

#include "core.hpp"
#include "source.hpp"

namespace Makai {
	using namespace CTL;
}

namespace Makai::Audio {
	class Music: public ISource {
	public:
		struct MetaData {
			String	title;
			String	artist;
			String	album;
			String	copyright;
		};

		using ISource::ISource;

		virtual ~Music() final;

		void play(int const loops = 0, usize const fadeInTime = 0);

		String getTitle();

		MetaData getMetaData();

		void switchInto(usize const fadeOutTime, usize const fadeInTime);

		void switchInto(usize const fadeOutTime, usize const fadeInTime, int const loops);

		void queueMusic();

		inline Extern::Resource getSource();

		static void		update();
		static void		stop(usize const fade = 0);
		static void		pause();
		static void		resume();
		static void		setVolume(schar const volume);
		static schar	getVolume();

	protected:
		void onCreate(Extern::Resource const& data) final override;

		void onDestroy() final override;

		void onUpdate() final override;

	private:
		usize	fadeInTime	= 0;
		int		loops		= 0;

		AudioCallback onQueue = [&] {play(loops, fadeInTime);};

		Extern::Resource source = NULL;
	};
}

#endif // MAKAILIB_AUDIO_MUSIC_H
