#ifndef SHARED_VALUE_HANDLER_H
#define SHARED_VALUE_HANDLER_H

#include "referential.hpp"
#include <vector>

namespace Manager {
	using
		SmartPointer::Pointable,
		SmartPointer::Operation
	;
	using std::vector;

	template<Pointable T>
	using RefList = vector<T*>;

	template <Pointable T>
	class ValueManager {
	public:
		ValueManager() {}

		ValueManager(ValueManager<T>&& other)		{bind(other.refs);}
		ValueManager(ValueManager<T> const& other)	{bind(other.refs);}
		ValueManager(T* const& obj)					{bind(obj);}
		ValueManager(RefList<T> const& obj)			{bind(obj);}

		~ValueManager() {unbind();}

		ValueManager<T>& bind(T* const& obj)				{refs.push_back(obj); return (*this);								}
		ValueManager<T>& bind(T& obj)						{refs.push_back(&obj); return (*this);								}
		ValueManager<T>& bind(RefList<T> const& list)		{for (auto obj: list) refs.push_back(obj); return (*this);			}
		ValueManager<T>& bind(ValueManager<T> const& other)	{for (auto obj: other.refs) refs.push_back(obj); return (*this);	}

		ValueManager<T>& unbind(T* const& obj)	{}
		ValueManager<T>& unbind()				{refs.clear();}

		ValueManager<T>& modify(Operation<T> op)	{for (auto obj: refs) op(*obj)		}
		ValueManager<T>& modify(void (*op)(T&))		{for (auto obj: refs) (*op)(*obj)	}

		inline ValueManager<T>& operator()(Operation<T> op)	{return modify(op);	}
		inline ValueManager<T>& operator()(void (*op)(T&))	{return modify(op);	}

		inline ValueManager<T> operator[](size_t index) {RefList res; for(obj: refs) res.push_back(obj[i]); return res;	}

		bool exists()					{return !refs.empty();	}
		inline bool operator()()		{return exists();		}
		operator bool() const			{return exists();		}
		inline bool operator!()			{return	!exists();		}

		ValueManager<T>& operator=(T* obj)										{bind(obj); return (*this);			}
		const ValueManager<T>& operator=(T* obj) const							{bind(obj); return (*this);			}
		ValueManager<T>& operator=(const ValueManager<T>& other)				{bind(other.refs); return (*this);	}
		const ValueManager<T>& operator=(const ValueManager<T>& other) const	{bind(other.refs); return (*this);	}
		ValueManager<T>& operator=(const RefList<T>& other)						{bind(other.refs); return (*this);	}
		const ValueManager<T>& operator=(const RefList<T>& other) const			{bind(other.refs); return (*this);	}

		ValueManager<T>& operator=(T const& v)					{for(auto obj: refs) (*obj) = v; return (*this);	}
		const ValueManager<T>& operator=(T const& v) const		{for(auto obj: refs) (*obj) = v; return (*this);	}
		ValueManager<T>& operator+=(T const& v)					{for(auto obj: refs) (*obj) += v; return (*this);	}
		const ValueManager<T>& operator+=(T const& v) const		{for(auto obj: refs) (*obj) += v; return (*this);	}
		ValueManager<T>& operator-=(T const& v)					{for(auto obj: refs) (*obj) -= v; return (*this);	}
		const ValueManager<T>& operator-=(T const& v) const		{for(auto obj: refs) (*obj) -= v; return (*this);	}
		ValueManager<T>& operator*=(T const& v)					{for(auto obj: refs) (*obj) *= v; return (*this);	}
		const ValueManager<T>& operator*=(T const& v) const		{for(auto obj: refs) (*obj) *= v; return (*this);	}
		ValueManager<T>& operator/=(T const& v)					{for(auto obj: refs) (*obj) /= v; return (*this);	}
		const ValueManager<T>& operator/=(T const& v) const		{for(auto obj: refs) (*obj) /= v; return (*this);	}
		ValueManager<T>& operator^=(T const& v)					{for(auto obj: refs) (*obj) ^= v; return (*this);	}
		const ValueManager<T>& operator^=(T const& v) const		{for(auto obj: refs) (*obj) ^= v; return (*this);	}
		ValueManager<T>& operator&=(T const& v)					{for(auto obj: refs) (*obj) &= v; return (*this);	}
		const ValueManager<T>& operator&=(T const& v) const		{for(auto obj: refs) (*obj) &= v; return (*this);	}
		ValueManager<T>& operator|=(T const& v)					{for(auto obj: refs) (*obj) |= v; return (*this);	}
		const ValueManager<T>& operator|=(T const& v) const		{for(auto obj: refs) (*obj) |= v; return (*this);	}
		ValueManager<T>& operator%=(T const& v)					{for(auto obj: refs) (*obj) %= v; return (*this);	}
		const ValueManager<T>& operator%=(T const& v) const		{for(auto obj: refs) (*obj) %= v; return (*this);	}
		ValueManager<T>& operator<<=(T const& v)				{for(auto obj: refs) (*obj) <<= v; return (*this);	}
		const ValueManager<T>& operator<<=(T const& v) const	{for(auto obj: refs) (*obj) <<= v; return (*this);	}
		ValueManager<T>& operator>>=(T const& v)				{for(auto obj: refs) (*obj) >>= v; return (*this);	}
		const ValueManager<T>& operator>>=(T const& v) const	{for(auto obj: refs) (*obj) >>= v; return (*this);	}

	private:

		RefList<T> refs;
	};
}

#endif // SHARED_VALUE_HANDLER_H
