#ifndef MAKAILIB_AUDIO_PLAYABLE_H
#define MAKAILIB_AUDIO_PLAYABLE_H

#include "../compat/ctl.hpp"

#include "../core/extern.hpp"
#include "../data/encdec.hpp"

namespace Makai::Audio {
	class Source {
	public:
		using Callback = const Signal<>;

		Source();

		Source(String const& path);

		virtual ~Source();

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
		Callback yield = [&] {this->onUpdate();};

		BinaryData<> data;

		bool created = false;
	};

	using AudioCallback = typename Source::Callback;
}

#endif // MAKAILIB_AUDIO_PLAYABLE_H
