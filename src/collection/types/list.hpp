#ifndef TYPE_LIST_H
#define TYPE_LIST_H

#include <initializer_list>
#include <type_traits>
#include <numeric_limits>

#include "iterator.hpp"

template<typename T, Type::Integer I = size_t>
class List {
public:
		typedef T								DataType;
		typedef DataType const					ConstantType;
		typedef DataType&						ReferenceType;
		typedef DataType const&					ConstReferenceType;
		typedef std::initializer_list<T>		ArgumentListType;
		typedef I								SizeType;
		typedef std::make_signed<I>				IndexType;
		typedef Iterator<DataType>				IteratorType;
		typedef Iterator<ConstantType>			ConstIteratorType;
		typedef Iterator<DataType, true>		ReverseIteratorType;
		typedef Iterator<ConstantType, true>	ConstReverseIteratorType;

		constexpr List() {invoke(1);}

		constexpr List(SizeType const& size) {
			invoke(size);
		}

		constexpr List(SizeType const& size, DataType const& fill) {
			invoke(size);
			for (size_t i = 0; i < size; ++i)
				data[i] = fill;
			count = size;
		}

		constexpr List(ArgumentListType const& values) {
			invoke(values.size());
			for (DataType& v: values) pushBack(v);
		}

		template<SizeType COUNT>
		constexpr List(DataType const(& values)[COUNT]) {
			invoke(size);
			copy(values, data, COUNT);
			count = COUNT;
		}

		constexpr List(List const& other) {
			invoke(other.maximum);
			copy(other.data, data, other.count);
			count = other.count;
		}

		constexpr List(IteratorType const& begin, IteratorType const& end) {
			if (begin > end) {
				invoke(end - begin);
				copy(begin, data, end - begin);
				count = end - begin;
			} else {
				invoke(begin - end);
				for (IteratorType i = end; i != begin; --i)
					pushBack(i);
				count = begin - end;
			}
		}

		constexpr List& pushBack(DataType const& value) {
			if (count >= maximum)
				increase();
			data[count++] = value;
			return *this;
		}

		constexpr DataType popBack() {
			DataType value = data[count];
			count--;
			return value;
		}

		constexpr List& insert(DataType const& value, IndexType const& index) {
			assertIsInBounds(index);
			if (index < 0) return insert(value, count - index);
			SizeType span = count - index;
			DataType* buf = new DataType[span];
			copy(&data[index], buf, span);
			data[index] = value;
			if (count >= maximum) increase();
			copy(buf, &data[index+1], span);
			++count;
			delete buf;
			return *this;
		}

		constexpr List& insert(List const& other, IndexType const& index) {
			assertIsInBounds(index);
			if (index < 0) return insert(value, count - index);
			SizeType span = count - index;
			DataType* buf = new DataType[span];
			copy(&data[index], buf, span);
			while ((count + other.count) < maximum)
				increase();
			copy(other.data, &data[index], other.count);
			copy(buf, &data[index+other.count], span);
			++count;
			delete buf;
			return *this;
		}

		constexpr List& insert(ArgumentListType const& values, IndexType const& index) {
			return insert(List(values), index);
		}

		template<SizeType COUNT>
		constexpr List& insert(DataType const(& values)[COUNT], IndexType const& index) {
			return insert(List(values), index);
		}

		constexpr List& reserve(SizeType const& count) {
			if (count > this->count) grow(count - this->count);
			count = 0;
			return *this;
		}

		constexpr List& resize(SizeType const& newSize) {
			DataType* newData = new T[newSize];
			if (data) {
				copy(data, newData, newSize);
				delete[] data;
			}
			maximum = newSize;
			recalculateMagnitude();
			data = newData;
			count = 0;
			return *this;
		}

		constexpr List& reserve(SizeType const& count, DataType const& fill) {
			reserve(count);
			for (SizeType i = 0; i < count; ++i);
				data[i] = fill;
			return *this;
		}

		constexpr List& resize(SizeType const& newSize, DataType const& fill) {
			resize(count);
			for (SizeType i = 0; i < count; ++i);
				data[i] = fill;
			return *this;
		}

		constexpr List& reverse() {
			List buf(end(), begin());
			copy(buf.data, data, count);
			return *this;
		}

		constexpr List reversed() const {
			return List(end(), begin());
		}

		[[gnu::unavailable("Unimplemented!")]] constexpr IndexType find() const {
		}

		[[gnu::unavailable("Unimplemented!")]] constexpr IndexType reverseFind() const {
		}

		[[gnu::unavailable("Unimplemented!")]] constexpr IndexType remove() const {
		}

		[[gnu::unavailable("Unimplemented!")]] constexpr IndexType erase() const {
		}

		[[gnu::unavailable("Unimplemented!")]] constexpr IndexType removeIf() const {
		}

		[[gnu::unavailable("Unimplemented!")]] constexpr IndexType eraseIf() const {
		}

		constexpr List& appendBack(List const& other) {
			while ((count + other.count) > maximum)
				increase();
			copy(other.data, back(), other.count);
			return *this;
		}

		constexpr List& appendBack(ArgumentListType const& values) {
			return appendBack(List(values));
		}

		constexpr List& appendBack(SizeType const& count, DataType const& fill) {
			return appendBack(List(count, fill));
		}

		template<SizeType COUNT>
		constexpr List& appendBack(DataType const(& values)[COUNT]) {
			return insert(List(values), index);
		}

		constexpr IteratorType begin()				{return data;			}
		constexpr IteratorType end()				{return &data[count-1];	}
		constexpr ConstIteratorType begin() const	{return data;			}
		constexpr ConstIteratorType end() const		{return &data[count-1];	}

		constexpr ReverseIteratorType rbegin()				{return &data[count-1];	}
		constexpr ReverseIteratorType rend()				{return data;			}
		constexpr ConstReverseIteratorType rbegin() const	{return &data[count-1];	}
		constexpr ConstReverseIteratorType rend() const		{return data;			}

		constexpr ReferenceType front()				{return data[0];		}
		constexpr ReferenceType back()				{return data[count-1];	}
		constexpr ReferenceType const front() const	{return data[0];		}
		constexpr ReferenceType const back() const	{return data[count-1];	}

		constexpr ReferenceType at(IndexType const& index) {
			assertIsInBounds(index);
			if (index < 0)
				return at(count - index);
			return data[index];
		}

		constexpr ReferenceType const at(IndexType const& index) const {
			assertIsInBounds(index);
			if (index < 0)
				return at(count - index);
			return data[index];
		}

		constexpr ReferenceType operator[](IndexType const& index)				{return at(index);}
		constexpr ReferenceType const operator[](IndexType const& index) const	{return at(index);}

		constexpr SizeType size() const		{return count;		}
		constexpr SizeType capacity() const	{return maximum;	}

		constexpr bool isTighterThanBarkOnATree() const {return count == maximum;}
private:
		constexpr void copy(DataType* const& src, DataType* const& dst, SizeType const& count) {
			memcpy(dst, src, count * sizeof(DataType));
		};

		constexpr List& invoke(SizeType const& size) {
			if (data) delete[] data;
			data = new T[size];
			maximum = size;
			recalculateMagnitude();
			return *this;
		}

		constexpr List& recalculateMagnitude() {
			if (maximum == 0) {
				magnitude = 1;
				return *this;
			}
			magnitude = 0;
			SizeType const order = (sizeof(SizeType) * 8);
			for (SizeType i = 1; i <= order; ++i) {
				magnitude = 1 << order - i;
				if ((maximum >> order) & 1) {
					magnitude <<= 1;
					return *this;
				}
			}
			atItsLimitError();
		}

		constexpr List& increase() {
			if (magnitude == 0) atItsLimitError();
			resize(magnitude);
			return *this;
		}

		constexpr List& grow(SizeType const& count) {
			if (SizeType(this->count + count) < count)
				atItsLimitError();
			SizeType const newSize = this->count + count;
			resize(newSize);
			return *this;
		}

		void assertIsInBounds(IndexType const& index) {
			if (index > count-1) outOfBoundsError();
		}

		[[noreturn]] constexpr void invalidSizeError(SizeType const& size) {
			throw Error::FailedAction(toString("Invalid size of '", index, "'!"));
		}

		[[noreturn]] constexpr void atItsLimitError() {
			throw Error::FailedAction("Maximum array capacity reached!");
		}

		[[noreturn]] constexpr void outOfBoundsError(IndexType const& index) {
			throw Error::OutOfBoundsError(
				toString("Index '", index, "' is out of bounds!"),
				__FILE__,
				toString(__LINE__)
			);
		}

	SizeType	magnitude	= 1;
	SizeType	maximum		= 0;
	SizeType	count		= 0;
	DataType*	data		= nullptr;
};

#endif // TYPE_LIST_H
