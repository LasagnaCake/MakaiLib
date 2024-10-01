#ifndef CTL_EX_EVENT_PLAYABLE_H
#define CTL_EX_EVENT_PLAYABLE_H

#include "../../ctl/exnamespace.hpp"

CTL_EX_NAMESPACE_BEGIN

struct Playable {
	virtual Playable&	start()		{return *this;		}
	virtual Playable&	play()		{return *this;		}
	virtual Playable&	pause()		{return *this;		}
	virtual Playable&	stop()		{return *this;		}
	virtual bool		finished()	{return isFinished;	}

	bool paused		= false;

protected:
	bool isFinished	= false;
};

CTL_EX_NAMESPACE_END

#endif // CTL_EX_EVENT_PLAYABLE_H
