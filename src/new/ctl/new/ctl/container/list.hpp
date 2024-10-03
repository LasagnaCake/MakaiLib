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
		for (DataType& v: values) pushBack(v);
	}

	template<SizeType COUNT>
	constexpr List(DataType const(& values)[COUNT]) {
		invoke(size);
		copy(values, contents, COUNT);
		count = COUNT;
	}

	constexpr List(SelfType const& other) {
		invoke(other.maximum);
		copy(other.contents, contents, other.count);
		count = other.count;
	}

	constexpr List(SelfType&& other) {
		maximum			= CTL::move(other.maximum);
		contents		= CTL::move(other.contents);
		count			= CTL::move(other.count);
		magnitude		= CTL::move(other.magnitude);
		other.contents	= nullptr;
	}

	constexpr List(IteratorType const& begin, IteratorType const& end) {
		invoke(end - begin);
		copy(begin, contents, end - begin);
		count = end - begin;
	}

	constexpr List(ReverseIteratorType const& begin, ReverseIteratorType const& end) {
		invoke(end - begin);
		for (IteratorType i = begin; i != end; ++i)
			pushBack(i);
		count = end - begin;
	}

	template<class T>
	constexpr List(T const& other)
	requires requires {
		T::begin;
		T::end;
	}: List(other.begin(), other.end()) {}

	constexpr ~List() {if (contents) dump();}

	constexpr SelfType& pushBack(DataType const& value) {
		if (count >= maximum)
			increase();
		contents[count++] = value;
		return *this;
	}

	constexpr DataType popBack() {
		DataType value = contents[count];
		count--;
		return value;
	}

	constexpr SelfType& insert(DataType const& value, IndexType index) {
		assertIsInBounds(index);
		wrapBounds(index, count);
		SizeType span = count - index;
		if (count >= maximum) increase();
		copy(&contents[index], &contents[index+1], span);
		contents[index] = value;
		++count;
		return *this;
	}

	constexpr SelfType& insert(SelfType const& other, IndexType index) {
		assertIsInBounds(index);
		wrapBounds(index, count);
		while ((count + other.count) < maximum)
			increase();
		copy(&contents[index], &contents[index+other.count], other.count);
		copy(other.contents, &contents[index], other.count);
		count += other.count;
		return *this;
	}

	constexpr SelfType& insert(ArgumentListType const& values, IndexType const& index) {
		return insert(SelfType(values), index);
	}

	template<SizeType S>
	constexpr SelfType& insert(Decay::AsType<DataType[S]> const& values, IndexType const& index) {
		return insert(SelfType(values, S), index);
	}

	constexpr SelfType& insert(DataType const& data, SizeType const& count, IndexType const& index) {
		return insert(SelfType(data, count), index);
	}

	constexpr SelfType& reserve(SizeType const& count) {
		while (count > maximum)
			increase();
		if (this->count > count)
			this->count = count;
		return *this;
	}

	constexpr SelfType& resize(SizeType const& newSize) {
		DataType* newData = memcreate(newSize);
		if (contents) {
			copy(contents, newData, count);
			free(contents, maximum);
		}
		maximum = newSize;
		contents = newData;
		if (count > newSize)
			count = newSize;
		recalculateMagnitude();
		return *this;
	}

	constexpr SelfType& expand(SizeType const& count) {
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

	constexpr SelfType& expand(SizeType const& count, DataType const& fill) {
		expand(this->count + count);
		while (count-- > 0) pushBack(fill);
		return *this;
	}

	constexpr SelfType& tighten() {
		resize(count);
		return *this;
	}

	constexpr SelfType& reverse() {
		::CTL::reverse(begin(), end());
		return *this;
	}

	constexpr SelfType reversed() const {
		return SelfType(*this).reverse();
	}

	constexpr SelfType& sort() {
		::CTL::sort(begin(), end());
	}

	constexpr SelfType sorted() const {
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
				return i-start;
		return -1;
	}

	constexpr IndexType bsearch(DataType const& value) const
	requires (
		Type::Comparable::Threeway<DataType, DataType>
	&&	Type::Comparable::Equals<DataType, DataType>
	) {
		IndexType pivot = count / 2, index = pivot;
		while (pivot != 0) {
			switch (contents[index] <=> value) {
				case Order::EQUAL:		return index;
				case Order::GREATER:	index -= (pivot /= 2);	break;
				case Order::LESS:		index += (pivot /= 2);	break;
				case Order::UNORDERED:	return -1;
			}
		}
		return -1;
	}

	constexpr SelfType& remove(IndexType const& index) {
		assertIsInBounds(index);
		copy(&contents[index], &contents[index-1], count-index);
		MX::memzero(&contents.back());
		return *this;
	}

	constexpr SelfType& erase(IndexType const& index) {
		remove(index);
		count--;
		return *this;
	}

	constexpr SelfType& removeIf(PredicateType const& predicate) {
		auto const start = begin();
		for(auto i = begin(); i != end(); ++i)
			if (predicate(*i)) {
				remove(i-start);
				--i;
			}
		return *this;
	}

	constexpr SelfType& removeIfNot(PredicateType const& predicate) {
		auto const start = begin();
		for(auto i = begin(); i != end(); ++i)
			if (!predicate(*i)) {
				remove(i-start);
				--i;
			}
		return *this;
	}

	constexpr SelfType& eraseIf(PredicateType const& predicate) {
		SizeType removed = 0;
		auto const start = begin();
		for(auto i = begin(); i != end(); ++i) {
			if (predicate(*i)) {
				remove(i-start);
				--i;
				removed++;
			}
		}
		count -= removed;
		return *this;
	}

	constexpr SelfType& eraseIfNot(PredicateType const& predicate) {
		SizeType removed = 0;
		auto const start = begin();
		for(auto i = begin(); i != end(); ++i) {
			if (!predicate(*i)) {
				remove(i-start);
				--i;
				removed++;
			}
		}
		count -= removed;
		return *this;
	}

	constexpr SelfType sliced(IndexType const& start, SizeType const& count) const {
		IndexType const stop = start + count;
		assertIsInBounds(start);
		return SelfType(begin() + start, begin() + (stop < this->count ? stop : this->count-1));
	}

	constexpr SelfType& appendBack(SelfType const& other) {
		expand(other.count);
		copy(other.contents, contents + other.count, other.count);
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
		copy(values, contents + S, S);
		return *this;
	}

	constexpr SelfType& clear() {count = 0;}

	constexpr SelfType& dispose() {
		if (contents) dump();
		count = 0;
		contents = nullptr;
		recalculateMagnitude();
	}

	constexpr PointerType		data()			{return contents;	}
	constexpr ConstPointerType	data() const	{return contents;	}

	constexpr IteratorType		begin()			{return contents;		}
	constexpr IteratorType		end()			{return contents+count;	}
	constexpr ConstIteratorType	begin() const	{return contents;		}
	constexpr ConstIteratorType	end() const		{return contents+count;	}

	constexpr ReverseIteratorType		rbegin()		{return contents+count;	}
	constexpr ReverseIteratorType		rend()			{return contents;		}
	constexpr ConstReverseIteratorType	rbegin() const	{return contents+count;	}
	constexpr ConstReverseIteratorType	rend() const	{return contents;		}

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

	constexpr ReferenceType	operator[](IndexType const& index) {
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

	constexpr OrderType compare(SelfType const& other)
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

	constexpr List<SelfType, IndexType> split(DataType const& sep) const {
		List<SelfType, IndexType> res;
		SelfType buf;
		for (DataType& v : *this) {
			if (v == sep) {
				res.pushBack(buf);
				buf.clear();
				continue;
			}
			buf += v;
		}
	}

	constexpr List<SelfType, IndexType> split(List const& seps) const {
		List<SelfType, IndexType> res;
		SelfType buf;
		for (DataType& v : *this) {
			if (seps.find(v)) {
				res.pushBack(buf);
				buf.clear();
				continue;
			}
			buf.pushBack(v);
		}
	}

	constexpr List<SelfType, IndexType> split(ArgumentListType const& seps) const {
		List<SelfType, IndexType> res;
		SelfType buf;
		for (DataType& v : *this) {
			bool isSeparator = false;
			for (DataType& s: seps)
				if (v == s) {
					res.pushBack(buf);
					buf.clear();
					isSeparator = true;
					break;
				}
			if (isSeparator)
				continue;
			buf.pushBack(v);
		}
	}

	constexpr List<SelfType, IndexType> splitAtFirst(DataType const& sep) const {
		List<SelfType, IndexType> res;
		IndexType idx = find(sep);
		if (idx < 0)
			return res.pushBack(*this);
		return res.pushBack({sliced(0, idx), sliced(idx, count)});
	}

	constexpr List<SelfType, IndexType> splitAtFirst(List<DataType> const& seps) const {
		List<SelfType, IndexType> res;
		IndexType idx = -1;
		for(DataType const& sep: seps) {
			IndexType i = find(sep);
			if (i > -1 && i < idx)
				idx = i;
		}
		if (idx < 0)
			return res.pushBack(*this);
		return res.pushBack({sliced(0, idx), sliced(idx, count)});
	}

	constexpr List<SelfType, IndexType> splitAtFirst(ArgumentListType const& seps) const {
		List<SelfType, IndexType> res;
		IndexType idx = -1;
		for(DataType const& sep: seps) {
			IndexType i = find(sep);
			if (i > -1 && i < idx)
				idx = i;
		}
		if (idx < 0)
			return res.pushBack(*this);
		return res.pushBack({sliced(0, idx), sliced(idx, count)});
	}

	constexpr List<SelfType, IndexType> splitAtLast(DataType const& sep) const {
		List<SelfType, IndexType> res;
		IndexType idx = rfind(sep);
		if (idx < 0)
			return res.pushBack(*this);
		return res.pushBack({sliced(0, idx), sliced(idx, count)});
	}

	constexpr List<SelfType, IndexType> splitAtLast(List<DataType> const& seps) const {
		List<SelfType, IndexType> res;
		IndexType idx = -1;
		for(DataType const& sep: seps) {
			IndexType i = rfind(sep);
			if (i > -1 && i > idx)
				idx = i;
		}
		if (idx < 0)
			return res.pushBack(*this);
		return res.pushBack({sliced(0, idx), sliced(idx, count)});
	}

	constexpr List<SelfType, IndexType> splitAtLast(ArgumentListType const& seps) const {
		List<SelfType, IndexType> res;
		IndexType idx = -1;
		for(DataType const& sep: seps) {
			IndexType i = rfind(sep);
			if (i > -1 && i > idx)
				idx = i;
		}
		if (idx < 0)
			return res.pushBack(*this);
		return res.pushBack({sliced(0, idx), sliced(idx, count)});
	}

	constexpr SelfType& transform(Function<DataType(DataType const&)> fun) {
		for(DataType& v: *this)
			v = fun(v);
		return *this;
	}

	constexpr SelfType transformed(Function<DataType(DataType const&)> fun) const {
		return SelfType(*this).transform();
	}

	constexpr bool validate(PredicateType const& cond) const {
		for (DataType const& c: *this)
			if (!cond(c))
				return false;
		return true;
	}

	constexpr SelfType filtered(PredicateType const& filter) const {
		return SelfType(*this).eraseIfNot(filter);
	}

	constexpr SelfType filtered(CompareType const& compare) {
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
		return filtered([](ConstReferenceType a, ConstReferenceType b){return a == b;});
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

	constexpr SelfType& replace(DataType const& val, DataType const& rep) {
		for (DataType& v: *this)
			if (v == val) v = rep;
		return *this;
	}

	constexpr SelfType& replace(SelfType const& values, DataType const& rep) {
		for (DataType const& val: values)
			replace(val, rep);
		return *this;
	}

	constexpr SelfType& replace(ArgumentListType const& values, DataType const& rep) {
		for (DataType const& val: values)
			replace(val, rep);
		return *this;
	}

	struct Replacement {
		SelfType	targets;
		DataType	replacement;
	};

	constexpr SelfType& replace(Replacement const& rep) {
		replace(rep.targets, rep.replacement);
		return *this;
	}

	constexpr SelfType& replace(List<Replacement, SizeType> const& reps) {
		for (Replacement const& rep: reps)
			replace(rep);
		return *this;
	}

	constexpr SelfType& replace(Arguments<Replacement> const& reps) {
		for (Replacement const& rep: reps)
			replace(rep);
		return *this;
	}

	template<typename T>
	constexpr SelfType replaced(T  const& rep) const {return SelfType(*this).replace(rep);}

	constexpr bool tight() const {return count == maximum;}

private:
	constexpr void dump() {
		memdestroy(contents, maximum);
		contents = nullptr;
		maximum = 0;
	}

	constexpr static void memdestroy(PointerType const& p, SizeType const& sz) {
		for (auto i = p; i != (p+sz); ++i)
			delete i;
		MX::free<DataType>(p, sz);
	}

	constexpr static DataType* memcreate(SizeType const& sz) {
		return MX::malloc<DataType>(sz);
	}

	constexpr static void copy(DataType* src, DataType* dst, SizeType count) {
		MX::memcpy<DataType>(dst, src, count);
	};

	constexpr SelfType& invoke(SizeType const& size) {
		if (contents) dump();
		contents = memcreate(size);
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
			if ((maximum >> order) & 1) {
				magnitude <<= 1;
				return *this;
			}
		}
		magnitude = 0;
	}

	constexpr SelfType& increase() {
		if (magnitude == 0) atItsLimitError();
		resize(magnitude);
		return *this;
	}

	constexpr SelfType& grow(SizeType const& count) {
		if (SizeType(this->count + count) < count)
			atItsLimitError();
		SizeType const newSize = this->count + count;
		resize(newSize);
		return *this;
	}

	void assertIsInBounds(IndexType const& index) {
		if (index > count-1) outOfBoundsError(index);
	}

	[[noreturn]] constexpr void invalidSizeError(SizeType const& size) {
		throw InvalidValueException("Invalid list size!");
	}

	[[noreturn]] constexpr void atItsLimitError() {
		throw MaximumSizeFailure("Maximum list capacity reached!");
	}

	[[noreturn]] constexpr void outOfBoundsError(IndexType const& index) {
		throw OutOfBoundsException("Array is out of bounds!");
	}

	[[noreturn]] constexpr void emptyError() {
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
