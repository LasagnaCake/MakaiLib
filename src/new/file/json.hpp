#ifndef MAKAILIB_FILE_JSON_H
#define MAKAILIB_FILE_JSON_H

#define JSON_NO_IO
#include "_lib/nlohmann/json.hpp"
#include "../compat/ctl.hpp"
#include "get.hpp"

namespace Makai::JSON {
	namespace Compat {
		template<class TIter, class... TArgs>
		struct IteratorAdaptor: public TIter {
			using iterator_category	= std::bidirectional_iterator_tag;
			using value_type		= typename TIter::DataType;
			using difference_type	= typename TIter::IndexType;
			using pointer			= typename TIter::PointerType;
			using reference			= typename TIter::ReferenceType;
			using TIter::TIter;
		};

		template<class TClass, typename... TArgs>
		struct ClassAdaptor: public TClass {
			using ClassType = TClass;
			using value_type		= typename ClassType::DataType;
			using reference			= typename ClassType::ReferenceType;
			using const_reference	= typename ClassType::ConstReferenceType;
			using pointer			= typename ClassType::PointerType;
			using const_pointer		= typename ClassType::ConstPointerType;
			using size_type			= typename ClassType::SizeType;
			using difference_type	= typename ClassType::IndexType;
			using iterator			= IteratorAdaptor<typename ClassType::IteratorType>;
			using const_iterator	= IteratorAdaptor<typename ClassType::ConstIteratorType>;
			using value_compare		= std::equal_to<value_type>;
			using ClassType::ClassType;
		};

		template<template <class, class> class TMap, class TKey, class TValue, class... TArgs>
		struct MapAdaptor: public TMap<TKey, TValue> {
			using MapType = TMap<TKey, TValue>;
			using key_type			= typename MapType::KeyType;
			using mapped_type		= typename MapType::ValueType;
			using value_type		= typename MapType::PairType;
			using reference			= value_type&;
			using const_reference	= value_type const&;
			using pointer			= value_type*;
			using const_pointer		= value_type const*;
			using size_type			= typename MapType::SizeType;
			using difference_type	= typename MapType::IndexType;
			using iterator			= IteratorAdaptor<typename MapType::IteratorType>;
			using const_iterator	= IteratorAdaptor<typename MapType::ConstIteratorType>;
			using key_compare		= std::less<key_type>;
			using mapped_compare	= std::equal_to<mapped_type>;
			using value_compare		= std::equal_to<value_type>;
			using MapType::MapType;
		};

		template<class T, typename... TArgs>
		using Proxy = T;

		template<typename TData, typename... TArgs>
		using ListAdaptor = ClassAdaptor<List<TData>, TData>;

		template<typename TKey, typename TValue, class, class>
		using OrderedMapAdaptor = MapAdaptor<OrderedMap, TKey, TValue>;

		using StringAdaptor = ClassAdaptor<String>;
	}

	namespace Extern {
		using Nlohmann = nlohmann::json;
		using JSONData = nlohmann::ordered_json;
	}

	using JSONType = Extern::JSONData;
	
	struct JSONValue;

	class JSONView: public View<Extern::JSONData> {
	public:
		JSONView(Extern::JSONData& data, String const& name = "<anonymous>");
		JSONView(Extern::JSONData const& data, String const& name = "<anonymous>");
		JSONView(JSONView const& other);
		JSONView(JSONView&& other);

		Extern::JSONData json() const;

		template<class T>
		inline T get() const {
			T result;
			if (!tryGet<T>(result))
				throw Error::FailedAction(
					"Parameter '" + name + "' is not of type '"
					+ TypeInfo<T>::name() + "'!",
					__FILE__,
					CTL::toString(__LINE__),
					CTL::toString("get<", TypeInfo<T>::name(), ">"),
					err
				);
			return result;
		}

		template<class T>
		inline T get(T const& fallback) const {
			T result;
			if (!tryGet<T>(result))
				return fallback;
			return result;
		}

		JSONView operator[](String const& key);
		const JSONView operator[](String const& key) const;

		JSONView operator[](usize const& index);
		const JSONView operator[](usize const& index) const;

		inline usize size() const {return view().size();}

		JSONView& operator=(JSONView const& v);

		template<class T>
		inline JSONView& operator=(T const& value)
		requires (!Type::Convertible<T, JSONView>) {
			view() = value;
			return (*this);
		}

		template<typename T> operator T() const {return get<T>();}

		String getName() const;

		String toString(int const& indent = -1, char const& ch = '\t') const;

		inline bool has(String const& key) const {return view().contains(key.toSTL()); }

		inline operator Extern::JSONData() {return view();}

		bool isNull() const;
		bool isInt() const;
		bool isFloat() const;
		bool isNumber() const;
		bool isObject() const;
		bool isUnsigned() const;
		bool isArray() const;
		bool isBool() const;
		bool isString() const;
		bool isPrimitive() const;
		bool isStructured() const;
		bool isDiscarded() const;

		template<Type::Primitive T>
		inline bool tryGet(T& out) const try {
			out = view().get<T>();
			return true;
		} catch (Extern::Nlohmann::exception const& e) {
			err = e.what();
			return false;
		}

		template<Type::Enumerator T>
		inline bool tryGet(T& out) const try {
			out = view().get<T>();
			return true;
		} catch (Extern::Nlohmann::exception const& e) {
			err = e.what();
			return false;
		}

		template <Type::Equal<String> T>
		inline bool tryGet(T& out) const try {
			out = view().get<std::string>();
			return true;
		} catch (Extern::Nlohmann::exception const& e) {
			err = e.what();
			return false;
		}

		template <Type::Container::List T>
		inline bool tryGet(T& out) const
		requires (
			Type::Different<typename T::DataType, String>
		&&	Type::Different<typename T::DataType, JSONView>
		&&	Type::Different<typename T::DataType, JSONValue>
		) try {
			out = T(view().get<std::vector<typename T::DataType>>());
			return true;
		} catch (Extern::Nlohmann::exception const& e) {
			err = e.what();
			return false;
		}

		template <Type::Container::List T>
		inline bool tryGet(T& out) const
		requires Type::Equal<typename T::DataType, String>
		try {
			out = T(view().get<std::vector<std::string>>());
			return true;
		} catch (Extern::Nlohmann::exception const& e) {
			err = e.what();
			return false;
		}

	private:
		Extern::JSONData const&	cdata;
		Extern::JSONData		dummy;

		mutable String err = "";

		String const name;
	};

	struct JSONValue: public JSONView {
		JSONValue(String const& name = "<anonymous>");

		JSONValue(String const& name, Extern::JSONData const& data);

		JSONValue(Extern::JSONData const& data);

		JSONValue(JSONView const& view);

		JSONValue(JSONValue const& other);

		template <Type::Container::List T>
		inline bool tryGet(T& out) const
		requires Type::Equal<typename T::DataType, JSONValue>
		try {
			out = T(view().get<std::vector<Extern::JSONData>>());
			return true;
		} catch (Extern::Nlohmann::exception const& e) {
			err = e.what();
			return false;
		}

		template <Type::Container::SimpleMap T>
		inline bool tryGet(T& out) const
		requires (
			Type::Equal<typename T::KeyType, String>
		&&	Type::Equal<typename T::ValueType, JSONValue>
		) try {
			T res;
			for (auto [k, v]: view().items())
				res[k] = v;
			return true;
		} catch (Extern::Nlohmann::exception const& e) {
			err = e.what();
			return false;
		}

		template <Type::Container::SimpleMap T>
		inline bool tryGet(T& out) const
		requires (
			Type::Equal<typename T::KeyType, String>
		&&	Type::Different<typename T::ValueType, JSONValue>
		) try {
			T res;
			for (auto [k, v]: view().items())
				res[k] = v.get<typename T::ValueType>();
			return true;
		} catch (Extern::Nlohmann::exception const& e) {
			err = e.what();
			return false;
		}

		JSONValue& clear();

		//inline operator JSONView() {return JSONView(data, getName());}

	private:
		Extern::JSONData data;
	};

	JSONValue object(String const& name = "<anonymous>");

	JSONValue array(String const& name = "<anonymous>");

	using JSONData = JSONValue;

	JSONData parse(String const& data);
	JSONData loadFile(String const& path);
	JSONData getFile(String const& path);
}

namespace MkJSON = Makai::JSON;

namespace Makai::File {
	inline JSON::JSONData loadJSON(String const& path)	{return JSON::loadFile(path);	}
	inline JSON::JSONData getJSON(String const& path)	{return JSON::getFile(path);	}
}

#endif // MAKAILIB_FILE_JSON_H
