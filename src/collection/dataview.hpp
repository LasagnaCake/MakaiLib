#include "helper.hpp"

namespace View {
	template<typename T>
	class Reference {
	public:
		Reference(T& _data):					data(&_data)		{}
		Reference(Reference<T> const& other):	data(&other.data)	{}
		Reference(Reference<T>&& other):		data(&other.data)	{}

		inline Reference& rebind(T& data)	{this->data = &data; return (*this);}

		bool exists() {return data != nullptr;};

		inline Reference& operator=(T const& val)				{if (exists()) (*data) = val; return (*this);	}
		inline Reference& operator=(Reference<T> const& val)	{if (exists()) data = val.data; return (*this);	}

		inline operator T&() {return (*data);}
	private:
		T* data = nullptr;
	};

	template<typename T>
	class DataView {
	public:
		DataView(T& _data):					data(_data)			{}
		DataView(DataView<T> const& other):	data(other.data)	{}
		DataView(DataView<T>&& other):		data(other.data)	{}

		inline DataView& modify(Operation<T> op)		{data = op(data); return (*this);	}
		inline DataView& operator()(Operation<T> op)	{return modify(op);					}

		inline DataView& operator=(T const& val) {data = val; return (*this);}

		inline operator T()	{return data;}
		inline T value()	{return data;}

	protected:
		inline T& view()	{return data;}

	private:
		T& data;
	};
}

using namespace View;
