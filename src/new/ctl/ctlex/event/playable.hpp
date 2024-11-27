#ifndef CTL_EX_EVENT_PLAYABLE_H
#define CTL_EX_EVENT_PLAYABLE_H

#include "../../ctl/exnamespace.hpp"

CTL_EX_NAMESPACE_BEGIN

/// @brief Controllable event interface.
struct Playable {
	virtual Playable&	start()		= 0;
	virtual Playable&	play()		= 0;
	virtual Playable&	pause()		= 0;
	virtual Playable&	stop()		= 0;
	virtual bool		finished()	{return isFinished;	}

	bool paused		= false;

	virtual ~Playable() {}

protected:
	bool isFinished	= false;
};

CTL_EX_NAMESPACE_END

#endif // CTL_EX_EVENT_PLAYABLE_H
