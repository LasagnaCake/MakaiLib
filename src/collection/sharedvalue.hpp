#ifndef SHARED_VALUE_HANDLER_H
#define SHARED_VALUE_HANDLER_H

#include "referential.hpp"
#include <vector>

namespace Shared {
	using
		SmartPointer::Pointable,
		SmartPointer::Operation
	;
	using std::vector;

	template<Pointable T>
	using RefList = vector<T*>;

	template <Pointable T>
	class SharedValue {
	public:
		SharedValue() {}

		SharedValue(SharedValue<T>&& other)		{bind(other.refs);}
		SharedValue(SharedValue<T> const& other)	{bind(other.refs);}
		SharedValue(T* const& obj)					{bind(obj);}
		SharedValue(RefList<T> const& obj)			{bind(obj);}

		~SharedValue() {unbind();}

		SharedValue<T>& bind(T* const& obj)					{refs.push_back(obj); return (*this);								}
		SharedValue<T>& bind(T& obj)						{refs.push_back(&obj); return (*this);								}
		SharedValue<T>& bind(RefList<T> const& list)		{for (auto obj: list) refs.push_back(obj); return (*this);			}
		SharedValue<T>& bind(SharedValue<T> const& other)	{for (auto obj: other.refs) refs.push_back(obj); return (*this);	}

		SharedValue<T>& unbind(T* const& obj)	{
			size_t i = 0;
			for(auto o: refs) {
				if (o == obj) break;
				i++;
			}
			if (i < refs.size())
				refs.remove(i + refs.begin());
			return (*this);
		}
		SharedValue<T>& unbind(T& obj)			{return unbind(&obj);}
		SharedValue<T>& unbind()				{refs.clear(); return (*this);}

		SharedValue<T>& modify(Operation<T> op)	{for (auto obj: refs) op(*obj)		}
		SharedValue<T>& modify(void (*op)(T&))	{for (auto obj: refs) (*op)(*obj)	}

		inline SharedValue<T>& operator()(Operation<T> op)	{return modify(op);	}
		inline SharedValue<T>& operator()(void (*op)(T&))	{return modify(op);	}

		inline SharedValue<T> operator[](size_t index) requires Type::Array<T>	{RefList res; for(obj: refs) res.push_back(obj[i]); return res;	}
		inline T& operator[](size_t index) requires !Type::Array<T>				{return (*refs[i]);}

		bool exists()				{return !refs.empty();	}
		inline bool operator()()	{return exists();		}
		operator bool() const		{return exists();		}
		inline bool operator!()		{return	!exists();		}

		SharedValue<T>& operator=(T* obj)									{bind(obj); return (*this);						}
		const SharedValue<T>& operator=(T* obj) const						{bind(obj); return (*this);						}
		SharedValue<T>& operator=(const SharedValue<T>& other)				{unbind(); bind(other.refs); return (*this);	}
		const SharedValue<T>& operator=(const SharedValue<T>& other) const	{unbind(); bind(other.refs); return (*this);	}
		SharedValue<T>& operator=(const RefList<T>& other)					{unbind(); bind(other.refs); return (*this);	}
		const SharedValue<T>& operator=(const RefList<T>& other) const		{unbind(); bind(other.refs); return (*this);	}

		SharedValue<T>& operator=(T const& v)				{for(auto obj: refs) (*obj) = v; return (*this);	}
		const SharedValue<T>& operator=(T const& v) const	{for(auto obj: refs) (*obj) = v; return (*this);	}
		SharedValue<T>& operator++()						{for(auto obj: refs) (*obj)++; return (*this);	}
		const SharedValue<T>& operator++() const			{for(auto obj: refs) (*obj)++; return (*this);	}
		SharedValue<T>& operator--()						{for(auto obj: refs) (*obj)--; return (*this);	}
		const SharedValue<T>& operator--() const			{for(auto obj: refs) (*obj)--; return (*this);	}
		SharedValue<T>& operator+=(T const& v)				{for(auto obj: refs) (*obj) += v; return (*this);	}
		const SharedValue<T>& operator+=(T const& v) const	{for(auto obj: refs) (*obj) += v; return (*this);	}
		SharedValue<T>& operator-=(T const& v)				{for(auto obj: refs) (*obj) -= v; return (*this);	}
		const SharedValue<T>& operator-=(T const& v) const	{for(auto obj: refs) (*obj) -= v; return (*this);	}
		SharedValue<T>& operator*=(T const& v)				{for(auto obj: refs) (*obj) *= v; return (*this);	}
		const SharedValue<T>& operator*=(T const& v) const	{for(auto obj: refs) (*obj) *= v; return (*this);	}
		SharedValue<T>& operator/=(T const& v)				{for(auto obj: refs) (*obj) /= v; return (*this);	}
		const SharedValue<T>& operator/=(T const& v) const	{for(auto obj: refs) (*obj) /= v; return (*this);	}
		SharedValue<T>& operator^=(T const& v)				{for(auto obj: refs) (*obj) ^= v; return (*this);	}
		const SharedValue<T>& operator^=(T const& v) const	{for(auto obj: refs) (*obj) ^= v; return (*this);	}
		SharedValue<T>& operator&=(T const& v)				{for(auto obj: refs) (*obj) &= v; return (*this);	}
		const SharedValue<T>& operator&=(T const& v) const	{for(auto obj: refs) (*obj) &= v; return (*this);	}
		SharedValue<T>& operator|=(T const& v)				{for(auto obj: refs) (*obj) |= v; return (*this);	}
		const SharedValue<T>& operator|=(T const& v) const	{for(auto obj: refs) (*obj) |= v; return (*this);	}
		SharedValue<T>& operator%=(T const& v)				{for(auto obj: refs) (*obj) %= v; return (*this);	}
		const SharedValue<T>& operator%=(T const& v) const	{for(auto obj: refs) (*obj) %= v; return (*this);	}
		SharedValue<T>& operator<<=(T const& v)				{for(auto obj: refs) (*obj) <<= v; return (*this);	}
		const SharedValue<T>& operator<<=(T const& v) const	{for(auto obj: refs) (*obj) <<= v; return (*this);	}
		SharedValue<T>& operator>>=(T const& v)				{for(auto obj: refs) (*obj) >>= v; return (*this);	}
		const SharedValue<T>& operator>>=(T const& v) const	{for(auto obj: refs) (*obj) >>= v; return (*this);	}

	private:

		RefList<T> refs;
	};
}

#endif // SHARED_VALUE_HANDLER_H
