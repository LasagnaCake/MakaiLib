#ifndef CTL_CONTAINER_LIST_H
#define CTL_CONTAINER_LIST_H

#include "../templates.hpp"
#include "../ctypes.hpp"
#include "../cpperror.hpp"
#include "../typetraits/traits.hpp"
#include "arguments.hpp"
#include "iterator.hpp"
#include "function.hpp"
#include "../algorithm/sort.hpp"
#include "../algorithm/reverse.hpp"
#include "../algorithm/memory.hpp"

CTL_NAMESPACE_BEGIN

template<class TData, Type::Integer TIndex = usize>
struct List:
	Iteratable<TData, TIndex>,
	SelfIdentified<List<TData, TIndex>>,
	ListInitializable<TData>,
	Ordered {
public:
	using Iteratable		= ::CTL::Iteratable<TData, TIndex>;
	using SelfIdentified	= ::CTL::SelfIdentified<List<TData, TIndex>>;
	using ListInitializable	= ::CTL::ListInitializable<TData>;

	using
		typename Iteratable::DataType,
		typename Iteratable::ConstantType,
		typename Iteratable::PointerType,
		typename Iteratable::ConstPointerType,
		typename Iteratable::ReferenceType,
		typename Iteratable::ConstReferenceType
	;

	using
		typename Iteratable::IndexType,
		typename Iteratable::SizeType
	;

	using
		typename Iteratable::IteratorType,
		typename Iteratable::ConstIteratorType,
		typename Iteratable::ReverseIteratorType,
		typename Iteratable::ConstReverseIteratorType
	;

	using
		typename SelfIdentified::SelfType
	;

	using
		typename ListInitializable::ArgumentListType
	;

	using PredicateType	= Function<bool(ConstReferenceType)>;
	using CompareType	= Function<bool(ConstReferenceType, ConstReferenceType)>;

	constexpr List() {invoke(1);}

	constexpr List(SizeType const& size) {
		invoke(size);
	}

	constexpr List(SizeType const& size, DataType const& fill) {
		invoke(size);
		for (usize i = 0; i < size; ++i)
			contents[i] = fill;
		count = size;
	}

	constexpr List(ArgumentListType const& values) {
		invoke(values.size());
		for (DataType const& v: values) pushBack(v);
	}

	template<SizeType S>
	constexpr explicit List(Decay::AsType<ConstantType[S]> const& values) {
		invoke(S);
		copy(values, contents, S);
		count = S;
	}

	constexpr List(SelfType const& other) {
		invoke(other.maximum);
		copy(other.contents, contents, other.count);
		count = other.count;
	}

	constexpr List(SelfType&& other) {
		maximum			= ::CTL::move(other.maximum);
		contents		= ::CTL::move(other.contents);
		count			= ::CTL::move(other.count);
		magnitude		= ::CTL::move(other.magnitude);
		other.contents	= nullptr;
	}
	
	constexpr List(ConstIteratorType const& begin, ConstIteratorType const& end) {
		invoke(end - begin + 1);
		copy(begin, contents, end - begin);
		count = end - begin;
	}

	constexpr List(ConstReverseIteratorType const& begin, ConstReverseIteratorType const& end) {
		invoke(end - begin + 1);
		for (IteratorType i = begin; i != end; ++i)
			pushBack(i);
		count = end - begin;
	}

	template<class T>
	constexpr List(T const& other)
	requires requires (T t) {
		{t.begin()} -> Type::Equal<IteratorType>;
		{t.end()} -> Type::Equal<IteratorType>;
	}: List(other.begin(), other.end()) {}

	template<class T>
	constexpr List(List<T, SizeType> const& other)
	requires requires (T t) {
		{t.begin()} -> Type::Equal<IteratorType>;
		{t.end()} -> Type::Equal<IteratorType>;
		requires Type::Constructible<DataType, IteratorType, IteratorType>;
	} {
		invoke(other.size());
		for (auto& v: other)
			pushBack(DataType(v.begin(), v.end()));
	}

	constexpr ~List() {dump();}

	constexpr SelfType& pushBack(DataType const& value) {
		if (count >= maximum)
			increase();
		MX::construct(contents+(count++), value);
		return *this;
	}

	constexpr DataType popBack() {
		DataType value = back();
		count--;
		if (count) MX::destruct<DataType>(contents+count);
		return value;
	}

	constexpr SelfType& insert(DataType const& value, IndexType index) {
		assertIsInBounds(index);
		wrapBounds(index, count);
		if (count >= maximum) increase();
		copy(&contents[index], &contents[index+1], count - index);
		MX::construct(contents+index, value);
		++count;
		return *this;
	}

	constexpr SelfType& insert(SelfType const& other, IndexType index) {
		assertIsInBounds(index);
		wrapBounds(index, count);
		expand(other.count);
		copy(&contents[index], &contents[index+other.count], count - index);
		copy(other.contents, &contents[index], other.count);
		count += other.count;
		return *this;
	}

	constexpr SelfType& insert(ArgumentListType const& values, IndexType const& index) {
		return insert(SelfType(values, values + count), index);
	}

	template<SizeType S>
	constexpr SelfType& insert(Decay::AsType<ConstantType[S]> const& values, IndexType const& index) {
		return insert(SelfType(values), index);
	}

	constexpr SelfType& insert(DataType const& value, SizeType const& count, IndexType const& index) {
		return insert(SelfType(value, count), index);
	}

	constexpr SelfType& reserve(SizeType const& count) {
		while (count >= maximum)
			increase();
		return *this;
	}

	constexpr SelfType& resize(SizeType const& newSize) {
		if (contents)	memresize(contents, newSize, maximum, count);
		else			contents = memcreate(newSize);
		maximum = newSize;
		if (count > newSize)
			count = newSize;
		recalculateMagnitude();
		return *this;
	}

	constexpr SelfType& expand(SizeType const& count) {
		if (!count) return *this;
		reserve(this->count + count);
		return *this;
	}

	constexpr SelfType& reserve(SizeType const& count, DataType const& fill) {
		reserve(count);
		for (SizeType i = 0; i < count; ++i)
			contents[i] = fill;
		this->count = count;
		return *this;
	}

	constexpr SelfType& resize(SizeType const& newSize, DataType const& fill) {
		resize(newSize);
		for (SizeType i = 0; i < count; ++i)
			contents[i] = fill;
		this->count = count;
		return *this;
	}

	constexpr SelfType& expand(SizeType count, DataType const& fill) {
		expand(this->count + count);
		while (count-- > 0) pushBack(fill);
		return *this;
	}

	constexpr SelfType& tighten() {
		resize(count);
		return *this;
	}

	constexpr SelfType& reverse() {
		::CTL::reverse(contents, count);
		return *this;
	}

	constexpr SelfType reversed() const {
		return SelfType(*this).reverse();
	}

	constexpr SelfType& sort()
	requires Sortable<DataType> {
		static_assert(SortableIterator<IteratorType>);
		::CTL::sort(begin(), end());
		return *this;
	}

	constexpr SelfType sorted() const
	requires Sortable<DataType> {
		static_assert(SortableIterator<IteratorType>);
		return SelfType(*this).sort();
	}

	constexpr IndexType find(DataType const& value) const
	requires Type::Comparable::Equals<DataType, DataType> {
		auto const start = begin(), stop = end();
		for (auto i = start; i != stop; ++i)
			if ((*i) == value)
				return i-start;
		return -1;
	}

	constexpr IndexType rfind(DataType const& value) const
	requires Type::Comparable::Equals<DataType, DataType> {
		auto const start = rbegin(), stop = rend();
		for (auto i = start; i != stop; ++i)
			if ((*i) == value)
				return count-(i-start)-1;
		return -1;
	}

	constexpr IndexType bsearch(DataType const& value) const
	requires (Type::Comparable::Threeway<DataType, DataType>) {
		if (empty()) return -1;
		if (OrderType(front() <=> value) == Order::EQUAL) return 0;
		if (OrderType(back() <=> value) == Order::EQUAL) return size() - 1;
		IndexType lo = 0, hi = size() - 1, i = -1;
		SizeType loop = 0;
		while (hi >= lo & loop < size()) {
			i = lo + (hi - lo) / 2;
			switch(OrderType(value <=> *(cbegin() + i))) {
				case Order::LESS:		hi = i-1; break;
				case Order::EQUAL:		return i;
				case Order::GREATER:	lo = i+1; break;
				default:
				case Order::UNORDERED:	return -1;
			}
		}
		return -1;
	}

	constexpr SelfType& remove(IndexType const& index) {
		assertIsInBounds(index);
		copy(&contents[index], &contents[index-1], count-index);
		MX::destruct(contents+count-1);
		return *this;
	}

	constexpr SelfType& erase(IndexType const& index) {
		remove(index);
		count--;
		return *this;
	}

	template<class TPredicate>
	constexpr SizeType removeIf(TPredicate const& predicate) {
		SizeType removed = 0;
		auto const start = begin();
		for(auto i = begin(); i != end();)
			if (predicate(*i)) {
				remove(i-start+1);
				removed++;
			} else ++i;
		return removed;
	}

	template<class TPredicate>
	constexpr SizeType removeIfNot(TPredicate const& predicate) {
		SizeType removed = 0;
		auto const start = begin();
		for(auto i = begin(); i != end();)
			if (!predicate(*i)) {
				remove(i-start+1);
				removed++;
			} else ++i;
		return removed;
	}

	template<class TPredicate>
	constexpr SelfType& eraseIf(TPredicate const& predicate) {
		count -= removeIf(predicate);
		return *this;
	}

	template<class TPredicate>
	constexpr SelfType& eraseIfNot(TPredicate const& predicate) {
		count -= removeIfNot(predicate);
		return *this;
	}

	constexpr SelfType sliced(IndexType start) const {
		if (IndexType(count) < start) return SelfType();
		assertIsInBounds(start);
		wrapBounds(start, count);
		return SelfType(cbegin() + start, cend());
	}

	constexpr SelfType sliced(IndexType start, IndexType stop) const {
		if (IndexType(count) < start) return SelfType();
		assertIsInBounds(start);
		wrapBounds(start, count);
		if (IndexType(count) < stop) return sliced(start);
		wrapBounds(stop, count);
		if (stop < start) return SelfType();
		return SelfType(cbegin() + start, cbegin() + stop + 1);
	}

	constexpr List<SelfType, IndexType> divide(IndexType index) const {
		List<SelfType, IndexType> res;
		assertIsInBounds(res);
		wrapBounds(index, count);
		res.pushBack(sliced(0, index));
		res.pushBack(sliced(index+1));
		return res;
	}

	constexpr SelfType& appendBack(SelfType const& other) {
		expand(other.count);
		copy(other.contents, contents + count, other.count);
		count += other.count;
		return *this;
	}

	constexpr SelfType& appendBack(ArgumentListType const& values) {
		return appendBack(SelfType(values));
	}

	constexpr SelfType& appendBack(SizeType const& count, DataType const& fill) {
		return expand(count, fill);
	}

	constexpr SelfType& appendBack(IteratorType const& begin, IteratorType const& end) {
		return appendBack(SelfType(begin, end));
	}

	constexpr SelfType& appendBack(ReverseIteratorType const& begin, ReverseIteratorType const& end) {
		return appendBack(SelfType(begin, end));
	}

	template<SizeType S>
	constexpr SelfType& appendBack(Decay::AsType<DataType[S]> const& values) {
		expand(S);
		copy(values, contents + count, S);
		count += S;
		return *this;
	}

	constexpr SelfType& clear() {count = 0; return *this;}

	constexpr SelfType& dispose() {
		dump();
		recalculateMagnitude();
		return *this;
	}

	constexpr SelfType& operator=(SelfType const& other) {
		memdestruct(contents, count);
		resize(other.count);
		copy(other.contents, contents, other.count);
		count = other.count;
		return *this;
	}

	constexpr SelfType& operator=(SelfType&& other) {
		memdestroy(contents, count);
		maximum			= CTL::move(other.maximum);
		contents		= CTL::move(other.contents);
		count			= CTL::move(other.count);
		magnitude		= CTL::move(other.magnitude);
		other.contents	= nullptr;
		return *this;
	}

	constexpr PointerType		data()			{return contents;	}
	constexpr ConstPointerType	data() const	{return contents;	}

	constexpr IteratorType		begin()			{return contents;		}
	constexpr IteratorType		end()			{return contents+count;	}
	constexpr ConstIteratorType	begin() const	{return contents;		}
	constexpr ConstIteratorType	end() const		{return contents+count;	}

	constexpr ReverseIteratorType		rbegin()		{return ReverseIteratorType(contents+count);		}
	constexpr ReverseIteratorType		rend()			{return ReverseIteratorType(contents);				}
	constexpr ConstReverseIteratorType	rbegin() const	{return ConstReverseIteratorType(contents+count);	}
	constexpr ConstReverseIteratorType	rend() const	{return ConstReverseIteratorType(contents);			}

	constexpr PointerType		cbegin()		{return contents;		}
	constexpr PointerType		cend()			{return contents+count;	}
	constexpr ConstPointerType	cbegin() const	{return contents;		}
	constexpr ConstPointerType	cend() const	{return contents+count;	}

	constexpr ReferenceType			front()			{return contents[0];		}
	constexpr ReferenceType 		back()			{return contents[count-1];	}
	constexpr ConstReferenceType	front() const	{return contents[0];		}
	constexpr ConstReferenceType	back() const	{return contents[count-1];	}


	constexpr DataType at(IndexType index) const {
		assertIsInBounds(index);
		wrapBounds(index, count);
		return contents[index];
	}

	constexpr ReferenceType	operator[](IndexType index) {
		assertIsInBounds(index);
		wrapBounds(index, count);
		return contents[index];
	}

	constexpr DataType operator[](IndexType const& index) const {return at(index);}

	constexpr SizeType size() const		{return count;		}
	constexpr SizeType capacity() const	{return maximum;	}
	constexpr SizeType empty() const	{return count == 0;	}

	constexpr bool operator==(SelfType const& other) const
	requires Type::Comparable::Equals<DataType, DataType> {
		return equals(other);
	}

	constexpr OrderType operator<=>(SelfType const& other) const
	requires Type::Comparable::Threeway<DataType, DataType> {
		return compare(other);
	}

	template <class T2>
	constexpr explicit operator List<T2, IndexType>() const
	requires (Type::Different<DataType, T2> && Type::Convertible<DataType, T2>) {
		List<T2, IndexType> result(count);
		for (usize i = 0; i < count; ++i)
			result[i] = T2(contents[i]);
		return result;
	}

	constexpr SizeType equals(SelfType const& other) const
	requires Type::Comparable::Equals<DataType, DataType> {
		bool result = true;
		IndexType i = 0;
		while (result) {
			if (i == count || i == other.count)
				return count == other.count;
			result = contents[i] == other.contents[i];
			++i;
		}
		return result;
	}

	constexpr OrderType compare(SelfType const& other) const
	requires Type::Comparable::Threeway<DataType, DataType> {
		OrderType result = Order::EQUAL;
		IndexType i = 0;
		while (result == Order::EQUAL) {
			if (i == count || i == other.count)
				return count <=> other.count;
			result = contents[i] <=> other.contents[i];
			++i;
		}
		return result;
	}

	constexpr SizeType disparity(SelfType const& other) const
	requires Type::Comparable::NotEquals<DataType, DataType> {
		SizeType
			diff	= 0,
			max		= (count > other.count ? count : other.count),
			min		= (count < other.count ? count : other.count)
		;
		for (SizeType i = 0; i < max; ++i)
			if (contents[i] != other.contents[i]) ++diff;
		return diff + (max - min);
	}

	/*
	constexpr SelfType operator+(DataType(const& values)[COUNT]) const {
		SelfType result(*this);
		return result.appendBack(values);
	}

	constexpr SelfType operator+(SelfType const& other) const {
		SelfType result(*this);
		return result.appendBack(other);
	}

	constexpr SelfType operator+(DataType const& value) const {
		SelfType result(*this);
		return result.pushBack(value);
	}

	constexpr SelfType& operator+=(DataType(const& values)[COUNT]) {
		return appendBack(values);
	}

	constexpr SelfType& operator+=(SelfType const& other) {
		return appendBack(other);
	}

	constexpr SelfType& operator+=(DataType const& value) {
		return pushBack(value);
	}

	constexpr SelfType operator*(SizeType const& times) const {
		SelfType result(count * times);
		for (SizeType i = 0 i < times, ++i)
			result += (*this);
		return result;
	}

	constexpr SelfType& operator*=(SizeType const& times) {
		*this = (*this) * times;
	}
	*/

	template <class TProcedure>
	constexpr SelfType& transform(TProcedure const& fun) {
		for(DataType& v: *this)
			v = fun(v);
		return *this;
	}

	template<class TProcedure>
	constexpr SelfType transformed(TProcedure const& fun) const {
		return SelfType(*this).transform();
	}

	template<class TPredicate>
	constexpr bool validate(TPredicate const& cond) const {
		for (DataType const& c: *this)
			if (!cond(c))
				return false;
		return true;
	}

	template<Type::Functional<bool(DataType const&)> TPredicate>
	constexpr SelfType filter(TPredicate const& filter) const {
		return eraseIfNot(filter);
	}

	template<Type::Functional<bool(DataType const&, DataType const&)> TCompare>
	constexpr SelfType filter(TCompare const& compare) {
		return *this = filtered(compare);
	}

	template<Type::Functional<bool(DataType const&)> TPredicate>
	constexpr SelfType filtered(TPredicate const& filter) const {
		return SelfType(*this).eraseIfNot(filter);
	}

	template<Type::Functional<bool(DataType const&, DataType const&)> TCompare>
	constexpr SelfType filtered(TCompare const& compare) {
		SelfType result;
		for (SizeType i = 0; i < count; ++i) {
			bool miss = false;
			for(SizeType j = count - 1; j >= 0; --j) {
				if (i == j) break;
				if (miss = !compare(contents[i], contents[j]))
					break;
			}
			if (!miss) result.pushBack(contents[i]);
		}
		return result;
	}

	constexpr SelfType uniques() {
		return filtered([](ConstReferenceType a, ConstReferenceType b){return a != b;});
	}

	template <Type::Equal<DataType> T>
	constexpr DataType join(typename T::DataType const& sep) const
	requires requires {
		requires Type::Class<DataType>;
		typename DataType::DataType;
		typename DataType::SizeType;
		requires Type::Equal<
			DataType,
			List<
				typename DataType::DataType,
				typename DataType::SizeType
			>
		>;
	} {
		DataType result = front();
		for (SizeType i = 1; i < count; ++i) {
			result.pushBack(sep);
			result.appendBack(contents[i]);
		}
		return result;
	}

	constexpr bool tight() const {return count == maximum;}

private:
	using Iteratable::wrapBounds;

	constexpr void dump() {
		if (!contents) return;
		memdestroy(contents, count);
		contents	= nullptr;
		maximum		= 0;
		count		= 0;
	}

	constexpr static void memdestruct(PointerType const& p, SizeType const& sz) {
		if constexpr (!Type::Primitive<DataType>) {
			for (auto i = p; i != (p+sz); ++i)
				MX::destruct(i);
		}
	}

	constexpr static void memdestroy(PointerType const& p, SizeType const& sz) {
		memdestruct(p, sz);
		MX::free<DataType>(p);
	}

	constexpr static DataType* memcreate(SizeType const& sz) {
		return MX::malloc<DataType>(sz);
	}

	constexpr static void memresize(DataType*& data, SizeType const& sz, SizeType const& oldsz, SizeType const& count) {
		if constexpr(Type::Primitive<DataType>)
			data = MX::realloc<DataType>(data, sz);
		else {
			DataType* ndata = MX::malloc<DataType>(sz);
			copy(data, ndata, count < sz ? count : sz);
			memdestroy(data, count);
			data = ndata;
		}
	}

	constexpr static void copy(ConstantType* src, DataType* dst, SizeType count) {
		if constexpr (Type::Primitive<DataType>)
			MX::memmove<DataType>(dst, src, count);
		else MX::objcopy<DataType>(dst, src, count);
	}

	constexpr SelfType& invoke(SizeType const& size) {
		if (contents)	memresize(contents, size, maximum, count);
		else			contents = memcreate(size);
		maximum = size;
		recalculateMagnitude();
		return *this;
	}

	constexpr SelfType& recalculateMagnitude() {
		if (maximum == 0) {
			magnitude = 1;
			return *this;
		}
		magnitude = 0;
		SizeType const order = (sizeof(SizeType) * 8)-1;
		for (SizeType i = 1; i <= order; ++i) {
			magnitude = 1 << order - i;
			if ((maximum >> (order - i)) & 1) {
				magnitude <<= 1;
				return *this;
			}
		}
		magnitude = 0;
		return *this;
	}

	constexpr SelfType& increase() {
		if (magnitude == 0) atItsLimitError();
		resize(magnitude);
		return *this;
	}

	constexpr SelfType& grow(SizeType const& count) {
		if (SizeType(this->count + count) < this->count)
			atItsLimitError();
		SizeType const newSize = this->count + count;
		resize(newSize);
		return *this;
	}

	void assertIsInBounds(IndexType const& index) const {
		if (index > count-1) outOfBoundsError(index);
	}

	[[noreturn]] constexpr static void invalidSizeError(SizeType const& size) {
		throw InvalidValueException("Invalid list size!");
	}

	[[noreturn]] constexpr static void atItsLimitError() {
		throw MaximumSizeFailure();
	}

	[[noreturn]] constexpr static void outOfBoundsError(IndexType const& index) {
		throw OutOfBoundsException("Array is out of bounds!");
	}

	[[noreturn]] constexpr static void emptyError() {
		throw NonexistentValueException("Container is empty!");
	}

	SizeType	magnitude	= 1;
	SizeType	maximum		= 0;
	SizeType	count		= 0;
	DataType*	contents	= nullptr;
};

template <Type::Integer TIndex = usize>
using BinaryData = List<uint8, TIndex>;

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_LIST_H
