#ifndef SHARED_VALUE_HANDLER_H
#define SHARED_VALUE_HANDLER_H

#include "helper.hpp"
#include "referential.hpp"
#include <vector>

namespace Shared {
	using
		SmartPointer::Pointable
	;

	template<typename T>
	concept Shareable = Pointable<T> && !Type::Array<T>;

	template<Shareable T>
	using RefList = List<T*>;

	template <Shareable T>
	class GroupValue {
	public:
		GroupValue() {}

		GroupValue(GroupValue<T>&& other)		{bind(other.refs);}
		GroupValue(GroupValue<T> const& other)	{bind(other.refs);}
		GroupValue(T* const& obj)				{bind(obj);}
		GroupValue(RefList<T> const& obj)		{bind(obj);}

		~GroupValue() {unbind();}

		GroupValue<T>& bind(T* const& obj)				{refs.push_back(obj); return (*this);								}
		GroupValue<T>& bind(T& obj)						{refs.push_back(&obj); return (*this);								}
		GroupValue<T>& bind(RefList<T> const& list)		{for (auto obj: list) refs.push_back(obj); return (*this);			}
		GroupValue<T>& bind(GroupValue<T> const& other)	{for (auto obj: other.refs) refs.push_back(obj); return (*this);	}

		GroupValue<T>& unbind(T* const& obj)	{
			size_t i = 0;
			for(auto o: refs) {
				if (o == obj) break;
				i++;
			}
			if (i < refs.size())
				refs.remove(i + refs.begin());
			return (*this);
		}
		GroupValue<T>& unbind(T& obj)			{return unbind(&obj);			}
		GroupValue<T>& unbind()					{refs.clear(); return (*this);	}

		GroupValue<T>& modify(Operation<T> op)		{for (auto obj: refs) (*obj) = op(*obj); return (*this);	}
		GroupValue<T>& modify(T (*op)(T const&))	{for (auto obj: refs) (*obj) = (*op)(*obj); return (*this);	}

		inline GroupValue<T>& operator()(Operation<T> op)	{return modify(op);	}
		inline GroupValue<T>& operator()(T (*op)(T const&))	{return modify(op);	}

		inline T& operator[](size_t index)	{return (*refs[index]);}

		bool exists()				{return !refs.empty();	}
		inline bool operator()()	{return exists();		}
		operator bool() const		{return exists();		}
		inline bool operator!()		{return	!exists();		}

		GroupValue<T>& operator=(T* obj)									{bind(obj); return (*this);						}
		const GroupValue<T>& operator=(T* obj) const						{bind(obj); return (*this);						}
		GroupValue<T>& operator=(const GroupValue<T>& other)				{unbind(); bind(other.refs); return (*this);	}
		const GroupValue<T>& operator=(const GroupValue<T>& other) const	{unbind(); bind(other.refs); return (*this);	}
		GroupValue<T>& operator=(const RefList<T>& other)					{unbind(); bind(other.refs); return (*this);	}
		const GroupValue<T>& operator=(const RefList<T>& other) const		{unbind(); bind(other.refs); return (*this);	}

		GroupValue<T>& operator=(T const& v)															{for(auto obj: refs) (*obj) = v; return (*this);	}
		const GroupValue<T>& operator=(T const& v) const												{for(auto obj: refs) (*obj) = v; return (*this);	}
		GroupValue<T>& operator++() requires Type::PreIncrementable<T>									{for(auto obj: refs) ++(*obj); return (*this);		}
		const GroupValue<T>& operator++() const requires Type::PreIncrementable<T>						{for(auto obj: refs) ++(*obj); return (*this);		}
		GroupValue<T>& operator++(int) requires Type::PostIncrementable<T>								{for(auto obj: refs) (*obj)++; return (*this);		}
		const GroupValue<T>& operator++(int) const requires Type::PostIncrementable<T>					{for(auto obj: refs) (*obj)++; return (*this);		}
		GroupValue<T>& operator--() requires Type::PreDecrementable<T>									{for(auto obj: refs) --(*obj); return (*this);		}
		const GroupValue<T>& operator--() const requires Type::PreDecrementable<T>						{for(auto obj: refs) --(*obj); return (*this);		}
		GroupValue<T>& operator--(int) requires Type::PostDecrementable<T>								{for(auto obj: refs) (*obj)--; return (*this);		}
		const GroupValue<T>& operator--(int) const requires Type::PostDecrementable<T>					{for(auto obj: refs) (*obj)--; return (*this);		}
		GroupValue<T>& operator+=(T const& v) requires Type::AddAssignable<T, T>						{for(auto obj: refs) (*obj) += v; return (*this);	}
		const GroupValue<T>& operator+=(T const& v) const requires Type::AddAssignable<T, T>			{for(auto obj: refs) (*obj) += v; return (*this);	}
		GroupValue<T>& operator-=(T const& v) requires Type::SubAssignable<T, T>						{for(auto obj: refs) (*obj) -= v; return (*this);	}
		const GroupValue<T>& operator-=(T const& v) const requires Type::SubAssignable<T, T>			{for(auto obj: refs) (*obj) -= v; return (*this);	}
		GroupValue<T>& operator*=(T const& v) requires Type::MulAssignable<T, T>						{for(auto obj: refs) (*obj) *= v; return (*this);	}
		const GroupValue<T>& operator*=(T const& v) const requires Type::MulAssignable<T, T>			{for(auto obj: refs) (*obj) *= v; return (*this);	}
		GroupValue<T>& operator/=(T const& v) requires Type::DivAssignable<T, T>						{for(auto obj: refs) (*obj) /= v; return (*this);	}
		const GroupValue<T>& operator/=(T const& v) const requires Type::DivAssignable<T, T>			{for(auto obj: refs) (*obj) /= v; return (*this);	}
		GroupValue<T>& operator^=(T const& v) requires Type::Bitwise::XorAssignable<T, T>				{for(auto obj: refs) (*obj) ^= v; return (*this);	}
		const GroupValue<T>& operator^=(T const& v) const requires Type::Bitwise::XorAssignable<T, T>	{for(auto obj: refs) (*obj) ^= v; return (*this);	}
		GroupValue<T>& operator&=(T const& v) requires Type::Bitwise::AndAssignable<T, T>				{for(auto obj: refs) (*obj) &= v; return (*this);	}
		const GroupValue<T>& operator&=(T const& v) const requires Type::Bitwise::AndAssignable<T, T>	{for(auto obj: refs) (*obj) &= v; return (*this);	}
		GroupValue<T>& operator|=(T const& v) requires Type::Bitwise::OrAssignable<T, T>				{for(auto obj: refs) (*obj) |= v; return (*this);	}
		const GroupValue<T>& operator|=(T const& v) const requires Type::Bitwise::OrAssignable<T, T>	{for(auto obj: refs) (*obj) |= v; return (*this);	}
		GroupValue<T>& operator%=(T const& v) requires Type::ModAssignable<T, T>						{for(auto obj: refs) (*obj) %= v; return (*this);	}
		const GroupValue<T>& operator%=(T const& v) const requires Type::ModAssignable<T, T>			{for(auto obj: refs) (*obj) %= v; return (*this);	}
		GroupValue<T>& operator<<=(T const& v) requires Type::Stream::InsAssignable<T, T>				{for(auto obj: refs) (*obj) <<= v; return (*this);	}
		const GroupValue<T>& operator<<=(T const& v) const requires Type::Stream::InsAssignable<T, T>	{for(auto obj: refs) (*obj) <<= v; return (*this);	}
		GroupValue<T>& operator>>=(T const& v) requires Type::Stream::ExtAssignable<T, T>				{for(auto obj: refs) (*obj) >>= v; return (*this);	}
		const GroupValue<T>& operator>>=(T const& v) const requires Type::Stream::ExtAssignable<T, T>	{for(auto obj: refs) (*obj) >>= v; return (*this);	}

	private:

		RefList<T> refs;
	};
}

#endif // SHARED_VALUE_HANDLER_H
