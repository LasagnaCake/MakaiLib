#ifndef CTL_CONTAINER_LIST_H
#define CTL_CONTAINER_LIST_H

#include "../templates.hpp"
#include "../ctypes.hpp"
#include "../cpperror.hpp"
#include "../typetraits/traits.hpp"
#include "iterator.hpp"
#include "../algorithm/sort.hpp"
#include "../algorithm/reverse.hpp"
#include "../algorithm/memory.hpp"

template<class TData, Type::Integer TIndex = usize>
struct List:
	Iteratable<TData, TIndex>,
	SelfIdentified<List<TData, TIndex>>,
	Ordered {
public:
	constexpr List() {invoke(1);}

	constexpr List(SizeType const& size) {
		invoke(size);
	}

	constexpr List(SizeType const& size, DataType const& fill) {
		invoke(size);
		for (usize i = 0; i < size; ++i)
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

	constexpr List(SelfType const& other) {
		invoke(other.maximum);
		copy(other.data, data, other.count);
		count = other.count;
	}

	constexpr List(SelfType&& other) {
		maximum		= move(other.maximum);
		data		= move(other.data);
		count		= move(other.count);
		magnitude	= move(other.magnitude);
		other.data	= nullptr;
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

	constexpr ~List() {if (data) delete[] data;}

	constexpr SelfType& pushBack(DataType const& value) {
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

	constexpr SelfType& insert(DataType const& value, IndexType index) {
		assertIsInBounds(index);
		while (index < 0) index += count;
		SizeType span = count - index;
		if (count >= maximum) increase();
		copy(&data[index], &data[index+1], span);
		data[index] = value;
		++count;
		return *this;
	}

	constexpr SelfType& insert(SelfType const& other, IndexType index) {
		assertIsInBounds(index);
		while (index < 0) index += count;
		while ((count + other.count) < maximum)
			increase();
		copy(&data[index], &data[index+other.count], other.count);
		copy(other.data, &data[index], other.count);
		count += other.count;
		return *this;
	}

	constexpr SelfType& insert(ArgumentListType const& values, IndexType const& index) {
		return insert(SelfType(values), index);
	}

	template<SizeType COUNT>
	constexpr SelfType& insert(DataType(const& values)[COUNT], IndexType const& index) {
		return insert(SelfType(values), index);
	}

	constexpr SelfType& reserve(SizeType const& count) {
		while (count > maximum)
			increase();
		if (this->count > count)
			this->count = count;
		return *this;
	}

	constexpr SelfType& resize(SizeType const& newSize) {
		DataType* newData = new T[newSize];
		if (data) {
			copy(data, newData, count);
			delete[] data;
		}
		maximum = newSize;
		data = newData;
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
		for (SizeType i = 0; i < count; ++i);
			data[i] = fill;
		this->count = count;
		return *this;
	}

	constexpr SelfType& resize(SizeType const& newSize, DataType const& fill) {
		resize(newSize);
		for (SizeType i = 0; i < count; ++i);
			data[i] = fill;
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
		::reverse(begin(), end());
		return *this;
	}

	constexpr SelfType reversed() const {
		return SelfType(*this).reverse();
	}

	constexpr SelfType& sort() {
		::sort(begin(), end());
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
			switch (data[index] <=> value) {
				case OrderType::EQUAL:		return index;
				case OrderType::GREATER:	index -= (pivot /= 2);	break;
				case OrderType::LESS:		index += (pivot /= 2);	break;
				case OrderType::UNORDERED:	return -1;
			}
		}
		return -1;
	}

	constexpr SelfType& remove(IndexType const& index) {
		assertIsInBounds(index);
		copy(&data[index], &data[index-1], count-index);
	}

	constexpr SelfType& erase(IndexType const& index) {
		remove(index);
		count--;
	}

	constexpr SelfType& removeIf(Functor<bool(DataType const&)> const& predicate) {
		auto const start = begin();
		for(auto i = begin(); i != end(); ++i) {
			if (predicate(*i))
				remove(i-start);
		}
		return *this;
	}

	constexpr SelfType& eraseIf(Functor<bool(DataType const&)> const& predicate) {
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

	constexpr SelfType sliced(IndexType const& start, SizeType const& count) const {
		IndexType const stop = start + count;
		assertIsInBounds(start);
		return SelfType(begin() + start, begin() + (stop < this->count ? stop : this->count-1));
	}

	constexpr SelfType& appendBack(SelfType const& other) {
		expand(other.count);
		copy(other.data, data + other.count, other.count);
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

	template<SizeType COUNT>
	constexpr SelfType& appendBack(DataType const(& values)[COUNT]) {
		expand(other.count);
		copy(values, data + other.count, COUNT);
		return *this;
	}

	constexpr SelfType& clear() {count = 0;}

	constexpr SelfType& dispose() {
		if (data) delete[] data;
		count = 0;
		data = nullptr;
		recalculateMagnitude();
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
	constexpr ConstReferenceType	front() const	{return data[0];		}
	constexpr ConstReferenceType	back() const	{return data[count-1];	}

	constexpr ReferenceType at(IndexType index) {
		assertIsInBounds(index);
		while (index < 0) index += count;
		return data[index];
	}

	constexpr ConstReferenceType at(IndexType index) const {
		assertIsInBounds(index);
		while (index < 0) index += count;
		return data[index];
	}

	constexpr ReferenceType			operator[](IndexType const& index)			{return at(index);}
	constexpr ConstReferenceType	operator[](IndexType const& index) const	{return at(index);}

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
	requires (Type::Different<DataType, T2> && Type::Convertible<T, T2>) {
		List<T2, IndexType> result(str.size(), '\0');
		for (usize i - 0; i < str.size(); ++i)
			result[i] = T2(str[i]);
		return result;
	}

	constexpr SizeType equals(SelfType const& other) const
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

	constexpr OrderType compare(SelfType const& other) {
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

	constexpr SizeType disparity(SelfType const& other) const
	requires Type::Comparable::NotEquals<DataType, DataType> {
		SizeType
			diff	= 0,
			max		= (count > other.count ? count : other.count),
			min		= (count < other.count ? count : other.count)
		;
		for (SizeType i = 0; i < max; ++i)
			if (data[i] != other.data[i]) ++diff;
		return diff + (max - min);
	}

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

	constexpr List<SelfType, IndexType> split(DataType const& sep) const {
		List<SelfType, IndexType> res;
		SelfType buf;
		for (DataType& v : *this) {
			if (v == sep) {
				res.pushBack(buf);
				buf.clear();
				continue;
			}
			buf += c;
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
			buf.pushBack(c);
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
			buf.pushBack(c);
		}
	}

	constexpr bool tight() const {return count == maximum;}

private:
	constexpr static void copy(DataType* src, DataType* dst, SizeType count) {
		memcpy<DataType>(dst, src, count);
	};

	constexpr SelfType& invoke(SizeType const& size) {
		if (data) delete[] data;
		data = new T[size];
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
		throw BasicError("Invalid list size!");
	}

	[[noreturn]] constexpr void atItsLimitError() {
		throw BasicError("Maximum list capacity reached!");
	}

	[[noreturn]] constexpr void outOfBoundsError(IndexType const& index) {
		throw BasicError("Array is out of bounds!");
	}

	[[noreturn]] constexpr void emptyError() {
		throw BasicError("Container is empty!");
	}

	SizeType	magnitude	= 1;
	SizeType	maximum		= 0;
	SizeType	count		= 0;
	DataType*	data		= nullptr;
};

template<class TData, Type::Integer TIndex = usize>
class LinkedList: Typed<TData>, Indexed<TIndex>, SelfIdentified<List<TData, TIndex>> {
public:
	struct Node {
		DataType	value		= nullptr;
		Node*		previous	= nullptr;
		Node*		next		= nullptr;

		constexpr bool operator==(DataType const& other) const
		requires Type::Comparable::Equals<DataType, DataType> {
			return value == other;
		}

		constexpr OrderType operator<=>(DataType const& other) const
		requires Type::Comparable::Threeway<DataType, DataType> {
			return value <=> other;
		}
	};

	template<bool REVERSE = false>
	class NodeIterator: public Iterator<Node, REVERSE, SizeType> {
	public:
		constexpr NodeIterator() {}

		constexpr NodeIterator(PointerType const& value): node(value)	{}
		constexpr NodeIterator(PointerType&& value): node(move(value))	{}

		constexpr NodeIterator(NodeIterator const& other): node(other.node)		{}
		constexpr NodeIterator(NodeIterator&& other): node(move(other.node))	{}

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

	constexpr LinkedList() {}

	constexpr LinkedList(SelfType const& other) {
		for (DataType const& v: other)
			pushBack(v);
	}

	constexpr LinkedList(ArgumentListType const& other) {
		for (DataType const& v: other)
			pushBack(v);
	}

	constexpr LinkedList(SelfType&& other) {
		for (DataType const& v: other)
			pushBack(v);
	}

	constexpr LinkedList(SizeType const& size, DataType const& fill) {
		for (SizeType i = 0; i < size; ++i)
			pushBack(fill);
	}

	constexpr LinkedList(ArgumentListType const& values) {
		invoke(values.size());
		for (DataType& v: values)
			pushBack(v);
	}

	template<SizeType COUNT>
	constexpr LinkedList(DataType(const& values)[COUNT]) {
		for (SizeType i = 0; i < COUNT; ++i)
			pushBack(values[i]);
	}

	constexpr LinkedList(IteratorType const& begin, IteratorType const& end) {
		for (IteratorType i = begin; i != end; ++i)
			pushBack(i);
	}

	constexpr LinkedList(ReverseIteratorType const& begin, ReverseIteratorType const& end) {
		for (IteratorType i = begin; i != end; ++i)
			pushBack(i);
	}

	constexpr ~LinkedList() {
		while (!empty()) popBack();
	}

	constexpr SelfType& pushBack(DataType const& value) {
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

	constexpr SelfType& pushFront(DataType const& value) {
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

	constexpr SelfType& insert(DataType const& value, IndexType index) {
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

	constexpr ReferenceType at(IndexType index) {
		while (index < 0) index += count;
		assertIsInBounds(index);
		Node* current = head;
		for (SizeType i = 0; i < index; ++i)
			current = current->next;
		return current->value;
	}

	constexpr ConstReferenceType at(IndexType index) const {
		while (index < 0) index += count;
		assertIsInBounds(index);
		Node* current = head;
		for (SizeType i = 0; i < index; ++i)
			current = current->next;
		return current->value;
	}

	constexpr ReferenceType operator[](IndexType const& index)				{return at(index);}
	constexpr ConstReferenceType operator[](IndexType const& index) const	{return at(index);}

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
	constexpr ConstReferenceType	front() const	{return head->data;	}
	constexpr ConstReferenceType	back() const	{return tail->data;	}

	constexpr bool operator==(SelfType const& other) const
	requires Type::Comparable::Equals<DataType, DataType> {
		return equals(other);
	}

	constexpr OrderType operator<=>(SelfType const& other) const
	requires Type::Comparable::Threeway<DataType, DataType> {
		return compare(other);
	}

	constexpr bool equals(SelfType const& other) const
	requires Type::Comparable::Equals<DataType, DataType> {
		bool result = true;
		IndexType i = 0;
		Node* s = head;
		Node* o = other.head;
		while (result) {
			if (i == count || i == other.count)
				return count == other.count;
			result = s->data == o->data;
			s = s->next;
			o = o->next;
			++i;
		}
		return result;
	}

	constexpr OrderType compare(SelfType const& other) const
	requires Type::Comparable::Threeway<DataType, DataType> {
		OrderType result = OrderType::EQUAL;
		IndexType i = 0;
		Node* s = head;
		Node* o = other.head;
		while (result == OrderType::EQUAL) {
			if (i == count || i == other.count)
				return count <=> other.count;
			result = s->data <=> o->data;
			s = s->next;
			o = o->next;
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
		Node* s = head;
		Node* o = other.head;
		for (SizeType i = 0; i < min; ++i) {
			if (s->data != o->data) ++diff;
			s = s->next;
			o = o->next;
		}
		return diff + (max - min);
	}

private:
	void assertIsInBounds(IndexType const& index) {
		if (index > count-1) outOfBoundsError(index);
	}

	void assertNotAtItsLimit() {
		if (count >= highest) atItsLimitError();
	}

	[[noreturn]] constexpr void invalidSizeError(SizeType const& size) {
		throw BasicError("Invalid list size!");
	}

	[[noreturn]] constexpr void atItsLimitError() {
		throw BasicError("Maximum list capacity reached!");
	}

	[[noreturn]] constexpr void outOfBoundsError(IndexType const& index) {
		throw BasicError("Array is out of bounds!");
	}

	[[noreturn]] constexpr void emptyError() {
		throw BasicError("Container is empty!");
	}

	SizeType count = 0;

	Node* head = nullptr;
	Node* tail = nullptr;
};

typedef List<uint8> BinaryData;

#endif // CTL_CONTAINER_LIST_H
