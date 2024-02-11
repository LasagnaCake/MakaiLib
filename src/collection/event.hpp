#ifndef EVENT_SIGNAL_TRIGGER_H
#define EVENT_SIGNAL_TRIGGER_H

#include <functional>
#include <vector>
#include "helper.hpp"
#include "conceptual.hpp"

#define T_SIGNAL_U(...)		(__VA_ARGS__)->void
#define T_TRIGGER_U(...)	(__VA_ARGS__)->bool
#define T_CALL_U(...)		(__VA_ARGS__)->Trigger
#define T_CHAIN_U(...)		(__VA_ARGS__)->Signal

#define	T_SIGNAL(...)	[&] T_SIGNAL_U(__VA_ARGS__)
#define	T_TRIGER(...)	[&] T_TRIGGER_U(__VA_ARGS__)
#define	T_CALL(...)		[&] T_CALL_U(__VA_ARGS__)
#define	T_CHAIN(...)	[&] T_CHAIN_U(__VA_ARGS__)

#define SIGNAL			T_SIGNAL()
#define TRIGGER			T_TRIGGER()
#define CALL			T_CALL()
#define CHAIN			T_CHAIN()

namespace Event{
	namespace {
		List<const Procedure<float>*> timerList;
	}

	/// Yields all available non-manual timers.
	void yieldAllTimers(float delta = 1) {
		// Loop through timers and step them
		if (timerList.size())
			for(const Procedure<float>* func : timerList)
				(*func)(delta);
	}

	/// A signal to be fired, whenever.
	typedef Function<void()>		Signal;

	/// A trigger to wait for.
	typedef Function<bool()>		Trigger;

	/// A call to be fired, which returns a Trigger to wait for.
	typedef Function<Trigger()>		Call;

	/// A signal to be fired whenever, which returns a Signal.
	typedef Function<Signal()>		Chain;

	const Signal	DEF_SIGNAL	=	[]()->void		{};
	const Trigger	DEF_TRIGGER	=	[]()->bool		{return true;};
	const Call		DEF_CALL	=	[]()->Trigger	{return DEF_TRIGGER;};
	const Chain		DEF_CHAIN	=	[]()->Signal	{return DEF_SIGNAL;};

	/**
	*****************
	*               *
	*  Timer Class  *
	*               *
	*****************
	*/
	class Timer {
	public:
		/// The signal to be fired.
		Signal onSignal = Event::DEF_SIGNAL;

		/// Whether the current Timer is paused / completed.
		bool paused	= false;

		/// Whether to repeatedly fire the signal, or just once.
		bool repeat = false;

		/// The delay until firing. the internal counter gets incremented until reaching it.
		float delay = 0;

		/// The amount of times to repeat for. If < 0, loops indefinitely.
		long loopCount = -1;

		/// Empty constructor.
		Timer(bool manual = false) {
			if (!manual)
				timerList.push_back(&_yield);
			this->manual = manual;
		}

		/// Signal + delay + repeat constructor.
		Timer(Signal onSignal, float delay = 1, bool repeat = false, bool manual = false) {
			this->onSignal	= onSignal;
			this->delay		= delay;
			this->repeat	= repeat;
			if (!manual)
				timerList.push_back(&_yield);
			this->manual = manual;
		}

		/// Delay + repeat constructor.
		Timer(float delay, bool repeat = false, bool manual = false) {
			this->delay		= delay;
			this->repeat	= repeat;
			if (!manual)
				timerList.push_back(&_yield);
			this->manual = manual;
		}

		Timer(Timer& other)
		: Timer(
			other.onSignal,
			other.delay,
			other.repeat,
			other.manual
		) {
			paused = other.paused;
			loopCount = other.loopCount;
			isFinished = other.isFinished;
			counter = other.counter;
			other.setManual();
		}

		Timer(Timer&& other): Timer(other) {}

		/// Destructor.
		~Timer() {
			// Loop through timer calls and delete if matches
			if (!timerList.empty()) std::erase_if(timerList, [&](auto& e){return e == &_yield;});
		}

		void setManual() {
			if (manual) return;
			// Loop through timer calls and delete if matches
			if (!timerList.empty()) std::erase_if(timerList, [&](auto& e){return e == &_yield;});
			manual = true;
		}

		void setAutomatic() {
			if (!manual) return;
			timerList.push_back(&_yield);
		}

		/**
		* Yields a cycle.
		*/
		void yield(float delta = 1) {
			// If not paused or not finished...
			if (!isFinished && !paused) {
				// If counter has reached target...
				if(counter >= delay) {
					// If repeating and not done looping, set counter to 0
					if (repeat && loopCount != 0) counter = 0;
					// Else, stop timer
					else isFinished = true;
					// Fire signal
					onSignal();
					// If loop count above zero, decrease it
					if (loopCount > 0) loopCount--;
				}
				// Increment counter
				counter += delta;
			}
		}

		/// Resets the Timer's counter to 0.
		Timer& reset() {
			counter = 0;
			return (*this);
		}

		/// Starts the timer from the beginning.
		Timer& start(float time) {
			counter = 0;
			delay = time;
			isFinished = false;
			return (*this);
		}

		/// Starts the timer from the beginning.
		Timer& start() {
			counter = 0;
			isFinished = false;
			return (*this);
		}

		/// Stops the timer.
		Timer& stop() {
			isFinished = true;
			return (*this);
		}

		/// Unpauses the timer.
		Timer& play() {
			paused = true;
			return (*this);
		}

		/// Pauses the timer.
		Timer& pause() {
			paused = false;
			return (*this);
		}

		/// Gets the counter's current value.
		float getCounter() {
			return counter;
		}

		/// Gets whether the timer is done executing.
		bool finished() {
			return isFinished;
		}

	private:
		/// Internal signal used for automatic processing.
		const Procedure<float> _yield = [&](float delta = 1) {
			this->yield(delta);
		};

		bool manual = false;

		/// Whether the timer is finished.
		bool isFinished = false;

		/// The current yield cycle.
		float counter = 0;
	};

	struct Notification {
		typedef Signal			SignalType;
		typedef Functor<void()>	SignalWrapper;

		Notification() {}

		Notification(String const& name): id(name)											{}
		Notification(String const& name, SignalType const& action): Notification(name)		{if (!func) return; db[id] = func = action;}
		Notification(String const& name, SignalWrapper const& action): Notification(name)	{if (!func) return; db[id] = func = action;}
		Notification(Notification const& other): Notification(other.id)						{}
		Notification(Notification&& other): Notification(other.id, other.func)				{}

		Notification& operator=(String const& name)			{id = name; return *this;												}
		Notification& operator=(Notification const& other)	{id = other.id; return *this;											}
		Notification& operator=(Notification&& other)		{id = other.id; if (!func) db[id] = func = other.func; return *this;	}

		~Notification() {if (db[id] == func) db[id] = SignalWrapper();}

		Notification const& emit() const		{db[id](); return *this;	}
		Notification& emit()					{db[id](); return *this;	}
		Notification const& operator()() const	{return emit();				}
		Notification& operator()()				{return emit();				}
	private:
		SignalWrapper	func;
		String			id;

		inline static Dictionary<SignalWrapper> db;
	};

	class Notifier {
	public:
		typedef Signal	SignalType;

		typedef List<SignalType>				SignalList;
		typedef Arguments<SignalType>			SignalArguments;
		typedef Functor<void()>					SignalWrapper;
		typedef Dictionary<List<SignalWrapper>>	SignalDatabase;

		Notifier() {}

		Notifier(String const& signal, Signal const& action)				{subscribe(signal, action);		}
		Notifier(String const& signal, SignalList const& actions)			{subscribe(signal, actions);	}
		Notifier(String const& signal, SignalArguments const& actions)		{subscribe(signal, actions);	}

		Notifier(Dictionary<SignalList> const& signals)						{subscribe(signals);			}
		Notifier(Arguments<Pair<String, SignalList>> const& signals)		{subscribe(signals);			}
		Notifier(Arguments<Pair<String, SignalArguments>> const& signals)	{subscribe(signals);			}

		~Notifier() {
			unsubscribeFromAll();
		}

		Notifier& subscribe(String const& signal, Signal const& action) {
			added[signal].push_back(action);
			db[signal].push_back(action);
			return *this;
		}

		Notifier& subscribe(String const& signal, SignalList const& actions) {
			for (Signal const& a: actions) {
				added[signal].push_back(a);
				db[signal].push_back(a);
			}
			return *this;
		}

		Notifier& subscribe(String const& signal, SignalArguments const& actions) {
			for (Signal const& a: actions) {
				added[signal].push_back(a);
				db[signal].push_back(a);
			}
			return *this;
		}

		Notifier& subscribe(Dictionary<SignalList> const& signals) {
			for (auto& [name, actions]: signals)
				subscribe(name, actions);
			return *this;
		}

		Notifier& subscribe(Arguments<Pair<String, SignalList>> const& signals) {
			for (auto& [name, actions]: signals)
				subscribe(name, actions);
			return *this;
		}

		Notifier& subscribe(Arguments<Pair<String, SignalArguments>> const& signals) {
			for (auto& [name, actions]: signals)
				subscribe(name, actions);
			return *this;
		}

		Notifier& unsubscribe(String const& signal) {
			if (db.contains(signal)) {
				auto& al = added[signal];
				std::erase_if(
					db[signal],
					[&] (SignalWrapper& elem) {
						return std::find(al.begin(), al.end(), elem) != al.end();
					}
				);
			}
			added[signal].clear();
			return *this;
		}

		Notifier& unsubscribe(StringList const& signals) {
			for (String const& s: signals)
				unsubscribe(s);
			return *this;
		}

		Notifier& unsubscribe(StringArguments const& signals) {
			for (String const& s: signals)
				unsubscribe(s);
			return *this;
		}

		template <typename... Args>
		Notifier& unsubscribe(Args const&... signals)
		requires (... && Type::Equal<Args, String>) {
			(..., unsubscribe(signals));
			return *this;
		}

		Notifier& unsubscribeFromAll() {
			for (auto& [name, lst]: db) {
				auto& al = added[name];
				std::erase_if(
					lst,
					[&] (SignalWrapper& elem) {
						return std::find(al.begin(), al.end(), elem) != al.end();
					}
				);
			}
			added.clear();
			return *this;
		}

		static void broadcast(String const& signal) {
			if (db.contains(signal))
				for (SignalWrapper& s: db[signal])
					s();
		}

		static void broadcast(StringList const& signals) {
			for (String const& s: signals)
				broadcast(s);
		}

		static void broadcast(StringArguments const& signals) {
			for (String const& s: signals)
				broadcast(s);
		}

		template <typename... Args>
		static void broadcast(Args const&... signals)
		requires (... && Type::Equal<Args, String>) {
			(..., broadcast(signals));
		}


		Notifier& operator()(String const& signal) {
			broadcast(signal);
			return *this;
		}

		Notifier& operator()(StringList const& signals) {
			broadcast(signals);
			return *this;
		}

		Notifier& operator()(StringArguments const& signals) {
			broadcast(signals);
			return *this;
		}

		template <typename... Args>
		Notifier& operator()(Args const&... signals)
		requires (... && Type::Equal<Args, String>) {
			broadcast(signals...);
			return *this;
		}

	private:
		SignalDatabase added;

		inline static SignalDatabase db;
	};
}

namespace TypedEvent {
	/// A signal to be fired, whenever.
	template <typename... Args>
	using Signal = Function<void(Args...)>;

	template<typename... Args>
	struct Notification {
		typedef Signal<Args...>			SignalType;
		typedef Functor<void(Args...)>	SignalWrapper;

		Notification() {}

		Notification(String const& name): id(name)											{}
		Notification(String const& name, SignalType const& action): Notification(name)		{if (!func) return; db[id] = func = action;}
		Notification(String const& name, SignalWrapper const& action): Notification(name)	{if (!func) return; db[id] = func = action;}
		Notification(Notification const& other): Notification(other.id)						{}
		Notification(Notification&& other): Notification(other.id, other.func)				{}

		Notification& operator=(String const& name)			{id = name; return *this;												}
		Notification& operator=(Notification const& other)	{id = other.id; return *this;											}
		Notification& operator=(Notification&& other)		{id = other.id; if (!func) db[id] = func = other.func; return *this;	}

		~Notification() {if (db[id] == func) db[id] = SignalWrapper();}

		Notification const& emit(Args... args) const		{db[id](args...); return *this;	}
		Notification& emit(Args... args)					{db[id](args...); return *this;	}
		Notification const& operator()(Args... args) const	{return emit(args...);			}
		Notification& operator()(Args... args)				{return emit(args...);			}
	private:
		SignalWrapper	func;
		String			id;

		inline static Dictionary<SignalWrapper> db;
	};
}

#endif // SIGNAL_TRIGGER_H
