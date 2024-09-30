#ifndef CTL_EX_ASYNC_WAITER_H
#define CTL_EX_ASYNC_WAITER_H

#include "../../ctl/exnamespace.hpp"
#include "../../ctl/async/atomic.hpp"

CTL_EX_NAMESPACE_BEGIN

class Ticker: public SelfIdentified<Ticker> {
	using Counter = Atomic<usize>;
	using CounterInstance = Instance<Counter>;

public:
	class Waiter: public SelfIdentified<Waiter>, Base::Async::Yieldable {
		constexpr Waiter(Waiter const& other): counter(other.counter)	{}
		constexpr Waiter(Waiter&& other): counter(move(other.counter))	{}

		constexpr ~Waiter() {counter.unbind();}

		void wait(usize const& ticks) {
			(*counter) = ticks;
			while (counter && counter->value() > 0) asyncYield();
		}

	private:

		constexpr Waiter(CounterInstance const& _counter): counter(_counter)	{}

		CounterInstance counter;

		friend class Ticker;
	};
	Ticker() {}

	~Ticker() {
		for(CounterInstance* counter: counters)
			delete counter;
	}

	void yield() {
		usize i = 0;
		for (CounterInstance* counter: counters) {
			if (counter.count() < 2) {
				delete counter;
				counters.erase(counters.find(counter));
			}
			else if (counter->value() > 0)	(*counter)--;
			++i;
		}
	}

	Waiter getWaiter() {
		counters.pushBack(createCounter());
		return Waiter(counters.back());
	}

private:
	static CounterInstance* createCounter() {
		return new CounterInstance(new Counter());
	}

	List<CounterInstance*> counters;
};

CTL_EX_NAMESPACE_END

#endif // CTL_EX_ASYNC_WAITER_H
