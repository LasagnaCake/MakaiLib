#ifndef TYPE_LIST_H
#define TYPE_LIST_H

#include <initializer_list>
#include <type_traits>
#include <numeric_limits>

#include "iterator.hpp"
#include "../types.hpp"

template<typename T, Type::Integer I = size_t>
class List {
public:
	// Types
	typedef T								DataType;
	typedef DataType const					ConstantType;
	typedef DataType&						ReferenceType;
	typedef DataType const&					ConstReferenceType;
	typedef std::initializer_list<DataType>	ArgumentListType;
	typedef DataType*						PointerType;
	typedef const DataType*					ConstPointerType;
	typedef ValueOrder						OrderType;
	// Size types
	typedef I							SizeType;
	typedef std::make_signed<SizeType>	IndexType;
	// Iterators
	typedef Iterator<DataType, false, SizeType>		IteratorType;
	typedef Iterator<ConstantType, false, SizeType>	ConstIteratorType;
	typedef Iterator<DataType, true, SizeType>		ReverseIteratorType;
	typedef Iterator<ConstantType, true, SizeType>	ConstReverseIteratorType;
	// Constant values
	constexpr SizeType maxSize = std::numeric_limits<SizeType>::max;

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
		invoke(end - begin);
		copy(begin, data, end - begin);
		count = end - begin;
	}

	constexpr List(ReverseIteratorType const& begin, ReverseIteratorType const& end) {
		invoke(end - begin);
		for (IteratorType i = begin; i != end; ++i)
			pushBack(i);
		count = end - begin;
	}

	constexpr ~List() {delete[] data;}

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

	constexpr List& insert(DataType const& value, IndexType index) {
		assertIsInBounds(index);
		while (index < 0) index += count;
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

	constexpr List& insert(List const& other, IndexType index) {
		assertIsInBounds(index);
		while (index < 0) index += count;
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
		resize(newSize);
		for (SizeType i = 0; i < count; ++i);
			data[i] = fill;
		return *this;
	}

	constexpr List& tighten() {
		resize(count);
		return *this;
	}

	constexpr List& reverse() {
		List buf(end(), begin());
		copy(buf.data, data, count);
		return *this;
	}

	constexpr List reversed() const {
		return List(rbegin(), rend());
	}

	constexpr IndexType find(DataType const& value) const {
		auto const start = begin();
		for (auto i = start; i != end(); ++i)
			if ((*i) == value)
				return i-start;
		return -1;
	}

	constexpr IndexType reverseFind(DataType const& value) const {
		auto const start = rbegin();
		for (auto i = start; i != rend(); ++i)
			if ((*i) == value)
				return i-start;
		return -1;
	}

	constexpr List& remove(IndexType const& index) {
		assertIsInBounds(index);
		copy(&data[index], &data[index-1], count-index);
	}

	constexpr List& erase(IndexType const& index) {
		remove(index);
		count--;
	}

	constexpr List& removeIf(View::Functor<bool(DataType const&)> const& predicate) const {
		auto const start = begin();
		for(auto i = begin(); i != end(); ++i) {
			if (predicate(*i))
				remove(i-start);
		}
		return *this;
	}

	constexpr List& eraseIf(View::Functor<bool(DataType const&)> const& predicate) const {
		SizeType removed = 0;
		auto const start = begin();
		for(auto i = begin(); i != end(); ++i) {
			if (predicate(*i)) {
				remove(i-start);
				removed++;
			}
		}
		count -= removed;
		return *this;
	}

	constexpr slice(IndexType const& start, SizeType const& count) const {
		IndexType const stop = start + count;
		assertIsInBounds(start);
		return List(begin() + start, begin() + (stop < this->count ? stop : this->count-1));
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

	constexpr List& appendBack(IteratorType const& begin, IteratorType const& end) {
		return appendBack(List(begin, end));
	}

	constexpr List& appendBack(ReverseIteratorType const& begin, ReverseIteratorType const& end) {
		return appendBack(List(begin, end));
	}

	template<SizeType COUNT>
	constexpr List& appendBack(DataType const(& values)[COUNT]) {
		return insert(List(values), index);
	}

	constexpr IteratorType		begin()			{return data;		}
	constexpr IteratorType		end()			{return data+count;	}
	constexpr ConstIteratorType	begin() const	{return data;		}
	constexpr ConstIteratorType	end() const		{return data+count;	}

	constexpr ReverseIteratorType		rbegin()		{return data+count;	}
	constexpr ReverseIteratorType		rend()			{return data;		}
	constexpr ConstReverseIteratorType	rbegin() const	{return data+count;	}
	constexpr ConstReverseIteratorType	rend() const	{return data;		}

	constexpr PointerType		cbegin()		{return data;		}
	constexpr PointerType		cend()			{return data+count;	}
	constexpr ConstPointerType	cbegin() const	{return data;		}
	constexpr ConstPointerType	cend() const	{return data+count;	}

	constexpr ReferenceType			front()			{return data[0];		}
	constexpr ReferenceType 		back()			{return data[count-1];	}
	constexpr ReferenceType const	front() const	{return data[0];		}
	constexpr ReferenceType const	back() const	{return data[count-1];	}

	constexpr ReferenceType at(IndexType index) {
		assertIsInBounds(index);
		while (index < 0) index += count;
		return data[index];
	}

	constexpr ReferenceType const at(IndexType index) const {
		assertIsInBounds(index);
		while (index < 0) index += count;
		return data[index];
	}

	constexpr ReferenceType			operator[](IndexType const& index)			{return at(index);}
	constexpr ReferenceType const	operator[](IndexType const& index) const	{return at(index);}

	constexpr SizeType size() const		{return count;		}
	constexpr SizeType capacity() const	{return maximum;	}
	constexpr SizeType empty() const	{return count == 0;	}

	constexpr bool operator==(List const& other) const
	requires Type::Comparable::Equals<DataType, DataType> {
		bool result = true;
		IndexType i = 0;
		while (result) {
			if (i == count || i == other.count)
				return count == other.count;
			result = data[i] == other.data[i];
			++i;
		}
		return result;
	}

	constexpr OrderType operator<=>(List const& other) const
	requires Type::Comparable::Threeway<DataType, DataType> {
		OrderType result = OrderType::EQUAL;
		IndexType i = 0;
		while (result == OrderType::EQUAL) {
			if (i == count || i == other.count)
				return count <=> other.count;
			result = data[i] <=> other.data[i];
			++i;
		}
		return result;
	}

	constexpr List operator+(List const& other) const {
		List result(*this);
		return result.appendBack(other);
	}

	constexpr List& operator+=(List const& other) {
		return appendBack(other);
	}

	constexpr List operator*(SizeType const& times) const {
		List result(count * times);
		for SSRANGE(i, 0, times)
			result += (*this);
		return result;
	}

	constexpr List& operator*=(SizeType const& times) {
		*this = (*this) * times;
	}

	// TODO: rename this
	constexpr bool isTighterThanBarkOnATree() const {return count == maximum;}

private:
	template <typename T>
	friend class BaseString;

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
		magnitude = 0;
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
		throw Error::FailedAction(toString("Invalid size of '", index, "'!"), __FILE_);
	}

	[[noreturn]] constexpr void atItsLimitError() {
		throw Error::FailedAction("Maximum array capacity reached!", __FILE_);
	}

	[[noreturn]] constexpr void outOfBoundsError(IndexType const& index) {
		throw Error::OutOfBounds(toString("Index '", index, "' is out of bounds!"), __FILE_);
	}

	[[noreturn]] constexpr void emptyError() {
		throw Error::OutOfBounds("Container is empty!", __FILE__);
	}

	SizeType	magnitude	= 1;
	SizeType	maximum		= 0;
	SizeType	count		= 0;
	DataType*	data		= nullptr;
};

template<typename T, Type::Integer I = size_t>
class LinkedList {
public:
	// Types
	typedef T								DataType;
	typedef DataType const					ConstantType;
	typedef DataType&						ReferenceType;
	typedef DataType const&					ConstReferenceType;
	typedef std::initializer_list<DataType>	ArgumentListType;
	typedef DataType*						PointerType;
	typedef const DataType*					ConstPointerType;
	// Size types
	typedef I							SizeType;
	typedef std::make_signed<SizeType>	IndexType;
	// Constant values
	constexpr SizeType maxSize = std::numeric_limits<SizeType>::max;

	template<bool REVERSE = false>
	class NodeIterator {
	public:
		constexpr NodeIterator() {}

		constexpr NodeIterator(PointerType const& value): node(value)		{}
		constexpr NodeIterator(PointerType&& value): node(std::move(value))	{}

		constexpr NodeIterator(NodeIterator const& other): node(other.node)			{}
		constexpr NodeIterator(NodeIterator&& other): node(std::move(other.node))	{}

		constexpr ReferenceType operator*()	{return node->data;		}
		constexpr PointerType operator->()	{return &node->data;	}

		constexpr ReferenceType operator*() const	{return node->data;		}
		constexpr PointerType operator->() const	{return &node->data;	}

		NodeIterator& operator++()		{step(); return *this;								}
		NodeIterator operator++(int)	{NodeIterator copy = *this; ++(*this); return copy;	}

		constexpr operator PointerType() const			{return &node->data;	}
		constexpr operator ReferenceType()				{return node->data;		}
		constexpr operator ConstReferenceType() const	{return node->data;		}

		constexpr bool operator==(NodeIterator const& other) const {return node == other.node;	}

	private:
		constexpr void step() {
			if constexpr (REVERSE)	node = node->previous;
			else					node = node->next;
		}

		Node* node = nullptr;
	};

	// Iterators
	typedef NodeIterator<false>	IteratorType;
	typedef NodeIterator<false>	ConstIteratorType;
	typedef NodeIterator<true>	ReverseIteratorType;
	typedef NodeIterator<true>	ConstReverseIteratorType;

	struct Node {
		DataType	value		= nullptr;
		Node*		previous	= nullptr;
		Node*		next		= nullptr;
	};

	constexpr ~LinkedList() {
		while (!empty()) popBack();
	}

	constexpr LinkedList& pushBack(DataType const& value) {
		assertNotAtItsLimit();
		Node* newTail = new Node{value};
		if (tail) {
			tail->next = newTail;
			newTail->previous = tail;
		}
		tail = newTail;
		if (!head)
			head = newTail;
		count++;
		return *this;
	}

	constexpr LinkedList& pushFront(DataType const& value) {
		assertNotAtItsLimit();
		Node* newHead = new Node{value};
		if (head) {
			head->previous = newHead;
			newHead->next = head;
		}
		head = newHead;
		if (!tail)
			tail = newHead;
		count++;
		return *this;
	}

	constexpr DataType popBack() {
		if (empty()) emptyContainerError();
		DataType value = tail->value;
		Node* newTail = tail->previous;
		if (tail->previous)
			tail->previous->next = nullptr;
		if (head == tail)
			head = newTail;
		delete tail;
		tail = newTail;
		count--;
		return value;
	}

	constexpr DataType popFront() {
		if (empty()) emptyContainerError();
		DataType value = head->value;
		Node* newHead = head->next;
		if (head->next)
			head->next->previous = nullptr;
		if (tail == head)
			tail = newHead;
		delete head;
		head = newHead;
		count--;
		return value;
	}

	constexpr List& insert(DataType const& value, IndexType index) {
		assertIsInBounds(index);
		while (index < 0) index += count;
		Node* current = head;
		for (SizeType i = 0; i < index; ++i)
			current = current->next;
		Node* newCurrent = new Node{data, current->previous, current};
		newCurrent->next = current;
		current->previous = newCurrent;
		return *this;
	}

	constexpr ValueType at(IndexType index) const {
		while (index < 0) index += count;
		assertIsInBounds(index);
		Node* current = head;
		for (SizeType i = 0; i < index; ++i)
			current = current->next;
		return current->value;
	}

	constexpr ValueType operator[](IndexType const& index) const {return at(index);}

	constexpr SizeType size() const		{return count;		}
	constexpr SizeType empty() const	{return count == 0;	}

	constexpr IteratorType		begin()			{return head;		}
	constexpr IteratorType		end()			{return nullptr;	}
	constexpr ConstIteratorType	begin() const	{return head;		}
	constexpr ConstIteratorType	end() const		{return nullptr;	}

	constexpr ReverseIteratorType		rbegin()		{return head;		}
	constexpr ReverseIteratorType		rend()			{return nullptr;	}
	constexpr ConstReverseIteratorType	rbegin() const	{return head;		}
	constexpr ConstReverseIteratorType	rend() const	{return nullptr;	}

	constexpr ReferenceType			front()			{return head->data;	}
	constexpr ReferenceType 		back()			{return tail->data;	}
	constexpr ReferenceType const	front() const	{return head->data;	}
	constexpr ReferenceType const	back() const	{return tail->data;	}

private:
	void assertIsInBounds(IndexType const& index) {
		if (index > count-1) outOfBoundsError();
	}

	void assertNotAtItsLimit() {
		if (count >= maxSize) atItsLimitError();
	}

	[[noreturn]] constexpr void invalidSizeError(SizeType const& size) {
		throw Error::FailedAction(toString("Invalid size of '", index, "'!"), __FILE_);
	}

	[[noreturn]] constexpr void atItsLimitError() {
		throw Error::FailedAction("Maximum array capacity reached!", __FILE_);
	}

	[[noreturn]] constexpr void outOfBoundsError(IndexType const& index) {
		throw Error::OutOfBoundsError(toString("Index '", index, "' is out of bounds!"), __FILE_);
	}

	[[noreturn]] constexpr void emptyContainerError() {
		throw Error::OutOfBounds("Container is empty!", __FILE__);
	}

	SizeType count = 0;

	Node* head = nullptr;
	Node* tail = nullptr;
};

typedef List<uint8> BinaryData;

#endif // TYPE_LIST_H
