#ifndef MAKAILIB_AUDIO_PLAYABLE_H
#define MAKAILIB_AUDIO_PLAYABLE_H

#include "../compat/ctl.hpp"

#include "../core/extern.hpp"
#include "../data/encdec.hpp"

namespace Makai::Audio {
	class ISource {
	public:
		using Callback = const Signal<>;

		ISource();

		ISource(String const& path);

		virtual ~ISource();

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

	using AudioCallback = typename ISource::Callback;
}

#endif // MAKAILIB_AUDIO_PLAYABLE_H
