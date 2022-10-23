#ifndef TASKING_TASKER_TASK_H
#define TASKING_TASKER_TASK_H

#include <vector>
#include <functional>
#include <type_traits>
#include "event.hpp"

/// Task parameter definition macro (AVOID USING).
#define $T_CATCHR			[&]
#define $T_CCATCH(cap...)	[&, cap]
#define $T_PARAMS			($tsk Tasker* _tasker) -> size_t
#define $T_TPARAM(T)		($tts Tasker<T>* _tasker, T* _self) -> size_t

/// Task parameter definition macro.
#define	$task			$T_CATCHR $T_PARAMS
#define	$ttask(TYPE)	$T_CATCHR $T_TPARAM(TYPE)

/// Zero-cycle delay task conclusion macro.
#define	$next	return 0

/// Single-cycle delay task conclusion macro.
#define	$end	return 1

// TODO: "C++ 20 coroutine"-ify it
namespace Tasking {
	namespace {
		using std::vector, std::function;
	}

	#define Task function<size_t(Tasker*)>

	/**
	******************
	*                *
	*  Tasker Class  *
	*                *
	******************
	*/
	class Tasker {
	public:
		/// Whether the current Tasker is paused. Different from completed.
		bool paused	= false;

		/// Whether to loop through the TaskList, or to only execute once.
		bool loop = false;

		/// Current trigger to wait for.
		$evt Trigger trigger = Event::DEF_TRIGGER;

		/// Signal to fire when completed.
		$evt Signal onCompleted = Event::DEF_SIGNAL;

		/// Current task being executed.
		size_t curTask = 0;

		/**
		* Amount of times the Tasker will loop.
		* If looping, this number gets decreased until reaching 0, when it stops looping.
		* Negative numbers means the Tasker will loop infinitely.
		*/
		int loopCount = -1;

		/// Empty constructor.
		Tasker() {}

		/// Vector constructor.
		Tasker(vector<Task> tList) {
			tasks	= tList;
			maxTask	= tasks.size();
		}

		/// Set's the Tasker's tasks.
		void setTasks(vector<Task> tList) {
			tasks.clear();
			tasks	= tList;
			maxTask	= tasks.size();
		}

		/**
		* Processes all Tasks in order. Task's return value is used as delay.
		* If returned delay is 0, process next Task,
		* until reaching the end of the list, or until returned delay is not 0.
		*/
		void yield() {
			// If loop count is zero...
			if (loopCount == 0) {
				// Set finished flag to true
				isFinished = true;
				// End execution
				return;
			}
			// If looping, and not an infinite loop....
			if (loop & (loopCount > 0)) {
				// Check if done looping
				loop = (loopCount > 0);
				// If done looping, end execution
				if (!loop) return;
			}
			// If no trigger present, set it to default trigger
			if (!trigger) trigger = Event::DEF_TRIGGER;
			// If not paused or done executing, and trigger is clear...
			if (!(isFinished || paused) && trigger()) {
				// Set trigger to default
				trigger = Event::DEF_TRIGGER;
				// Decrease delay if not already zero
				if (delay > 0) delay--;
				// If delay is 0 ("done waiting")
				if (delay < 1) {
					// Execute Task
					delay = tasks[curTask++](this);
					// If delay given is zero, yield next cycle
					if (delay < 1) yield();
				}
				// If not on last task...
				if (!(curTask < maxTask)) {
					// If loop...
					if (loop) {
						// set current task to first task
						curTask = 0;
						// decrease loop counter
						loopCount--;
					}
					// Else, set finished flag to true and fire signal
					else {
						onCompleted();
						isFinished = true;
					}
				}
			}
		}

		/// Returns whether Tasker is finished.
		bool finished() {
			return isFinished;
		}

		/// Sets the delay until next Task execution.
		void setDelay(size_t n) {
			delay = n;
		}

		/// Adds a Task to the end of the list of Tasks.
		void addTask(Task t) {
			tasks.push_back(t);
			maxTask = tasks.size();
		}

		/**
		* Resets the Tasker and sets it to the beginning of the TaskList.
		* Also sets loop count to infinite.
		*/
		void reset() {
			isFinished	= false;
			curTask		= 0;
			loopCount	= -1;
		}

		/// Ends Tasker execution, sets loop count to 0, and sets Tasker to completed.
		void stop() {
			isFinished	= true;
			curTask		= tasks.size() - 1;
			loopCount	= 0;
		}

		void clearTasks() {
			tasks.clear();
		}

	private:
		/// The list of Tasks to be executed.
		vector<Task> tasks;

		/// The delay until next Task. gets decremented when yield() is called, until 0.
		size_t delay = 0;

		/// The number of Tasks in the list.
		size_t maxTask = 0;

		/// Whether the Tasker is finished.
		bool isFinished	= false;
	};

	#undef Task

	/// A Task. Essentally a function, that takes in its parent Tasker, and returns a delay.
	typedef function<size_t(Tasker*)> Task;

	/// A list of Tasks.
	typedef vector<Task> TaskList;

	/// A list of Taskers.
	typedef vector<Tasker*> TaskerList;

	/**
	***********************
	*                     *
	*  MultiTasker Class  *
	*                     *
	***********************
	*/
	class MultiTasker {
	public:
		/// Whether the current MultiTasker should process.
		bool processing = true;

		/// Empty constructor.
		MultiTasker() {}

		/// Vector constructor.
		MultiTasker(TaskerList tList) {
			taskers.clear();
			taskers = tList;
		}

		/// Array pointer constructor (UNTESTED).
		MultiTasker(size_t lSize, Tasker** tList) {
			taskers.clear();
			for (size_t i = 0; i < lSize; i++) taskers.push_back(tList[i]);
		}

		/// Sets the MultiTasker's Taskers.
		void setTaskers(TaskerList tList) {
			taskers.clear();
			taskers = tList;
		}

		/// Sets the MultiTasker's Taskers (UNTESTED).
		void setTaskers(size_t lSize, Tasker** tList) {
			taskers.clear();
			for (size_t i = 0; i < lSize; i++) taskers.push_back(tList[i]);
		}

		/// Adds a Tasker to the MultiTasker. If overwrite is true, Tasker replaces completed Taskers.
		void addTasker(Tasker* t, bool overwrite = false) {
			// If overwriting is allowed, loop through Taskers
			if (overwrite) for (size_t i = 0; i < taskers.size(); i++) {
				// If a finished Tasker is found, replace it with given Tasker, and end function
				if (taskers[i]->finished()) {
					taskers[i] = t;
					return;
				}
			}
			// If no completed Tasker is found or overwriting is forbidden, append to Taskers
			taskers.push_back(t);
		}

		void clearTaskers() {
			taskers.clear();
		}

		/// Processes one task cycle of all Taskers.
		void yield() {
			// If there are taskers, loop through them and process cycle
			if (taskers.size() && processing)
				for (Tasker* t: taskers) t->yield();
		}

	private:

		/// The list of Taskers handled by the MultiTasker.
		TaskerList taskers;
	};

	/// A list of MultiTaskers.
	typedef vector<MultiTasker> MultiTaskerList;
}

namespace TypedTasking {
	namespace {
		using std::vector, std::function;
	}

	#define Task(args) function<size_t(Tasker*, args)>

	/**
	******************
	*                *
	*  Tasker Class  *
	*                *
	******************
	*/
	template<typename T>
	class Tasker {
	public:
		/// Whether the current Tasker is paused. Different from completed.
		bool paused	= false;

		/// Whether to loop through the TaskList, or to only execute once.
		bool loop = false;

		/// Current trigger to wait for.
		$evt Trigger trigger = Event::DEF_TRIGGER;

		/// Signal to fire when completed.
		$tev Signal<Tasker<T>*> onCompleted = [&](Tasker<T>* tskr){};

		/// The target object of the tasker.
		T* target = nullptr;

		/// Current task being executed.
		size_t curTask = 0;

		/**
		* Amount of times the Tasker will loop.
		* If looping, this number gets decreased until reaching 0, when it stops looping.
		* Negative numbers means the Tasker will loop infinitely.
		*/
		int loopCount = -1;

		/// Empty constructor.
		Tasker() {}

		/// Vector constructor.
		Tasker(vector<Task(T*)> tList) {
			tasks	= tList;
			maxTask	= tasks.size();
		}

		/// Set's the Tasker's tasks.
		void setTasks(vector<Task(T*)> tList) {
			tasks.clear();
			tasks	= tList;
			maxTask	= tasks.size();
		}

		/**
		* Processes all Tasks in order. Task's return value is used as delay.
		* If returned delay is 0, process next Task,
		* until reaching the end of the list, or until returned delay is not 0.
		*/
		void yield(T* target = nullptr) {
			if (!target) target = this->target;
			// If loop count is zero...
			if (loopCount == 0) {
				// Set finished flag to true
				isFinished = true;
				// End execution
				return;
			}
			// If looping, and not an infinite loop....
			if (loop & (loopCount > 0)) {
				// Check if done looping
				loop = (loopCount > 0);
				// If done looping, end execution
				if (!loop) return;
			}
			// If no trigger present, set it to default trigger
			if (!trigger) trigger = Event::DEF_TRIGGER;
			// If not paused or done executing, and trigger is clear...
			if (!(isFinished || paused) && trigger()) {
				// Set trigger to default
				trigger = Event::DEF_TRIGGER;
				// Decrease delay if not already zero
				if (delay > 0) delay--;
				// If delay is 0 ("done waiting")
				if (delay < 1) {
					// Execute Task
					delay = tasks[curTask++](this, target);
					// If delay given is zero, yield next cycle
					if (delay < 1) yield(target);
				}
				// If not on last task...
				if (!(curTask < maxTask)) {
					// If loop...
					if (loop) {
						// set current task to first task
						curTask = 0;
						// decrease loop counter
						loopCount--;
					}
					// Else, set finished flag to true and fire signal
					else {
						onCompleted(this);
						isFinished = true;
					}
				}
			}
		}

		/// Returns whether Tasker is finished.
		bool finished() {
			return isFinished;
		}

		/// Sets the delay until next Task execution.
		void setDelay(size_t n) {
			delay = n;
		}

		/// Adds a Task to the end of the list of Tasks.
		void addTask(Task(T*) t) {
			tasks.push_back(t);
			maxTask = tasks.size();
		}

		/**
		* Resets the Tasker and sets it to the beginning of the TaskList.
		* Also sets loop count to infinite.
		*/
		void reset() {
			isFinished	= false;
			curTask		= 0;
			loopCount	= -1;
		}

		/// Ends Tasker execution, sets loop count to 0, and sets Tasker to completed.
		void stop() {
			isFinished	= true;
			curTask		= tasks.size() - 1;
			loopCount	= 0;
		}

		void clearTasks() {
			tasks.clear();
		}

	private:
		/// The list of Tasks to be executed.
		vector<Task(T*)> tasks;

		/// The delay until next Task. gets decremented when yield() is called, until 0.
		size_t delay = 0;

		/// The number of Tasks in the list.
		size_t maxTask = 0;

		/// Whether the Tasker is finished.
		bool isFinished	= false;
	};

	#undef Task

	/// A Task. Essentally a function, that takes in its parent Tasker, and returns a delay.
	template <typename T>
	using Task = function<size_t(Tasker<T>*, T*)>;

	/// A list of Tasks.
	template <typename T>
	using TaskList = vector<Task<T>>;

	/// A list of Taskers.
	template <typename T>
	using TaskerList = vector<Tasker<T>*>;

	/**
	***********************
	*                     *
	*  MultiTasker Class  *
	*                     *
	***********************
	*/
	template <typename T>
	class MultiTasker {
	public:
		/// Whether the current MultiTasker should process.
		bool processing = true;

		/// The target object of the MultiTasker.
		T* target = nullptr;

		/// Empty constructor.
		MultiTasker() {}

		/// Vector constructor.
		MultiTasker(TaskerList<T> tList) {
			taskers.clear();
			taskers = tList;
		}

		/// Array pointer constructor (UNTESTED).
		MultiTasker(size_t lSize, Tasker<T>** tList) {
			taskers.clear();
			for (size_t i = 0; i < lSize; i++) taskers.push_back(tList[i]);
		}

		/// Sets the MultiTasker's Taskers.
		void setTaskers(TaskerList<T> tList) {
			taskers.clear();
			taskers = tList;
		}

		/// Sets the MultiTasker's Taskers (UNTESTED).
		void setTaskers(size_t lSize, Tasker<T>** tList) {
			taskers.clear();
			for (size_t i = 0; i < lSize; i++) taskers.push_back(tList[i]);
		}

		/// Adds a Tasker to the MultiTasker. If overwrite is true, Tasker replaces completed Taskers.
		void addTasker(Tasker<T>* t, bool overwrite = false) {
			// If overwriting is allowed, loop through Taskers
			if (overwrite) for (size_t i = 0; i < taskers.size(); i++) {
				// If a finished Tasker is found, replace it with given Tasker, and end function
				if (taskers[i]->finished()) {
					taskers[i] = t;
					return;
				}
			}
			// If no completed Tasker is found or overwriting is forbidden, append to Taskers
			taskers.push_back(t);
		}

		void clearTaskers() {
			taskers.clear();
		}

		/// Processes one task cycle of all Taskers.
		void yield(T* target = nullptr) {
			if (!target) target = this->target;
			// If there are taskers, loop through them and process cycle
			if (taskers.size() && processing)
				for (Tasker<T>* t: taskers) t->yield(target);
		}

	private:

		/// The list of Taskers handled by the MultiTasker.
		TaskerList<T> taskers;
	};

	/// A list of MultiTaskers.
	template <typename T>
	using MultiTaskerList = vector<MultiTasker<T>>;
}

#define $tsk Tasking::
#define $tts TypedTasking::

#endif // TASKER_TASK_H
