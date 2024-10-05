#ifndef CTL_EX_EVENT_NOTIFICATION_H
#define CTL_EX_EVENT_NOTIFICATION_H

#include "../../ctl/exnamespace.hpp"
#include "../../ctl/container/functor.hpp"
#include "../../ctl/container/string.hpp"
#include "../../ctl/container/map.hpp"
#include "../../ctl/container/dictionary.hpp"
#include "../../ctl/container/arguments.hpp"

CTL_EX_NAMESPACE_BEGIN

class Notifier {
public:
	typedef Signal<>	SignalType;

	typedef List<SignalType>				SignalList;
	typedef Arguments<SignalType>			SignalArguments;
	typedef SignalType						SignalWrapper;
	typedef Dictionary<List<SignalWrapper>>	SignalDatabase;

	Notifier() {}

	Notifier(String const& signal, SignalType const& action)			{subscribe(signal, action);		}
	Notifier(String const& signal, SignalList const& actions)			{subscribe(signal, actions);	}
	Notifier(String const& signal, SignalArguments const& actions)		{subscribe(signal, actions);	}

	Notifier(Dictionary<SignalList> const& signals)						{subscribe(signals);			}
	Notifier(Arguments<Pair<String, SignalList>> const& signals)		{subscribe(signals);			}
	Notifier(Arguments<Pair<String, SignalArguments>> const& signals)	{subscribe(signals);			}

	~Notifier() {
		unsubscribeFromAll();
	}

	Notifier& subscribe(String const& signal, SignalType const& action) {
		added[signal].pushBack(action);
		db[signal].pushBack(action);
		return *this;
	}

	Notifier& subscribe(String const& signal, SignalList const& actions) {
		for (SignalType const& a: actions) {
			added[signal].pushBack(a);
			db[signal].pushBack(a);
		}
		return *this;
	}

	Notifier& subscribe(String const& signal, SignalArguments const& actions) {
		for (SignalType const& a: actions) {
			added[signal].pushBack(a);
			db[signal].pushBack(a);
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
			db[signal].eraseIf([&] (SignalWrapper& elem) {return al.find(elem);});
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
			if (!added.contains(name))
				continue;
			auto& al = added[name];
			lst.eraseIf([&] (SignalWrapper& elem) {return al.find(elem);});
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

template<typename... Args>
struct Notification {
	typedef Signal<Args...>	SignalType;

	Notification() {}

	Notification(String const& name): id(name)											{}
	Notification(String const& name, SignalType const& action): Notification(name)		{db[id] = (func = action);}
	Notification(Notification const& other): Notification(other.id)						{}
	Notification(Notification&& other): Notification(other.id, other.func)				{}

	Notification& operator=(String const& name)			{id = name; return *this;								}
	Notification& operator=(Notification const& other)	{id = other.id; return *this;							}
	Notification& operator=(Notification&& other)		{id = other.id; moveFunction(other.func); return *this;	}

	~Notification() {if (db[id] == func) db[id] = SignalType();}

	Notification const& emit(Args... args) const		{db[id](args...); return *this;	}
	Notification& emit(Args... args)					{db[id](args...); return *this;	}
	Notification const& operator()(Args... args) const	{return emit(args...);			}
	Notification& operator()(Args... args)				{return emit(args...);			}

private:
	void moveFunction(SignalType& f) {
		if (f) {
			db[id] = (func = f);
			f = SignalType();
		}
	}

	Notification(String const& name, SignalType& action): Notification(name) {moveFunction(action);}

	SignalType	func;
	String		id;

	inline static Dictionary<SignalType> db;
};

CTL_EX_NAMESPACE_END

#endif // CTL_EX_EVENT_NOTIFICATION_H
