#ifndef CTL_EX_EVENT_PLAYABLE_H
#define CTL_EX_EVENT_PLAYABLE_H

#include "../../ctl/exnamespace.hpp"

CTL_EX_NAMESPACE_BEGIN

/// @brief Controllable event interface.
struct Playable {
	/// @brief Starts the event. Must be implemented.
	/// @return Reference to self.
	virtual Playable&	start()		= 0;
	/// @brief Resumes the event. Must be implemented.
	/// @return Reference to self.
	virtual Playable&	play()		= 0;
	/// @brief Pauses the event. Must be implemented.
	/// @return Reference to self.
	virtual Playable&	pause()		= 0;
	/// @brief Stops the event. Must be implemented.
	/// @return Reference to self.
	virtual Playable&	stop()		= 0;
	/// @brief Returns whether the event is finished being processed.
	/// @return Whether the event is finished being processed.
	virtual bool		finished()	{return isFinished;	}

	/// @brief Whether the event is paused.
	bool paused		= false;

	/// @brief Virtual destructor.
	virtual ~Playable() {}

protected:
	/// @brief Whether the event is finished being processed.
	bool isFinished	= false;
};

CTL_EX_NAMESPACE_END

#endif // CTL_EX_EVENT_PLAYABLE_H
