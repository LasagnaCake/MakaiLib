#ifndef MAKAILIB_AUDIO_PLAYABLE_H
#define MAKAILIB_AUDIO_PLAYABLE_H

#include "../ctl/ctl.hpp"
#include "../core/extern.hpp"
#include "../data/encdec.hpp"

namespace Makai::Audio {
	class Playable {
	public:
		using Callback = const Event::Signal;

		Playable();

		Playable(String const& path);

		virtual ~Playable();

		void create(String const& path);

		void destroy();

		virtual void onPlaybackEnd()	{}
		virtual void onPlaybackStart()	{}

		bool exists();

		static void update();

	protected:
		virtual void onCreate(Extern::Resource const& data)	= 0;
		virtual void onDestroy()							= 0;
		virtual void onUpdate()								{}

	private:
		Callback yield = SIGNAL {this->onUpdate();};

		Data::BinaryData data;

		bool created = false;
	};

	using AudioCallback = typename Playable::Callback;
}

#endif // MAKAILIB_AUDIO_PLAYABLE_H
