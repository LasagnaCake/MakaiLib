#ifndef MAKAILIB_FILE_JSON_H
#define MAKAILIB_FILE_JSON_H

#define JSON_NO_IO
#include "_lib/nlohmann/json.hpp"
#include "../compat/ctl.hpp"
#include "get.hpp"

namespace Makai::JSON {
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
		requires (!Makai::Type::Convertible<T, JSONView>) {
			view() = value;
			return (*this);
		}

		template<typename T> operator T() const {return get<T>();}

		String getName() const;

		String toString(int const& indent = -1, char const& ch = '\t') const;

		inline bool has(String const& key) const {return view().contains(key.stdView()); }

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

		template<Makai::Type::Primitive T>
		bool tryGet(T& out) const try {
			out = view().get<T>();
			err = "";
			return true;
		} catch (Extern::Nlohmann::exception const& e) {
			err = e.what();
			return false;
		}

		template<Makai::Type::Enumerator T>
		bool tryGet(T& out) const try {
			out = view().get<T>();
			err = "";
			return true;
		} catch (Extern::Nlohmann::exception const& e) {
			err = e.what();
			return false;
		}

		template <Makai::Type::Equal<String> T>
		bool tryGet(T& out) const try {
			out = view().get<std::string>();
			err = "";
			return true;
		} catch (Extern::Nlohmann::exception const& e) {
			err = e.what();
			return false;
		}

		template <Makai::Type::Container::List T>
		bool tryGet(T& out) const
		requires (
			Makai::Type::Different<typename T::DataType, String>
		&&	Makai::Type::Different<typename T::DataType, JSONView>
		&&	Makai::Type::Different<typename T::DataType, JSONValue>
		) try {
			out = T(view().get<std::vector<typename T::DataType>>());
			err = "";
			return true;
		} catch (Extern::Nlohmann::exception const& e) {
			err = e.what();
			return false;
		}

		template <Makai::Type::Container::List T>
		bool tryGet(T& out) const
		requires Makai::Type::Equal<typename T::DataType, String>
		try {
			out = T(view().get<std::vector<std::string>>());
			err = "";
			return true;
		} catch (Extern::Nlohmann::exception const& e) {
			err = e.what();
			return false;
		}

		String error() {return err;}

	private:
		Extern::JSONData const&	cdata;
		Extern::JSONData		dummy;

		mutable String err = "";

		String const name;

		friend class JSONValue;
	};

	struct JSONValue: public JSONView {
		JSONValue(String const& name = "<anonymous>");

		JSONValue(String const& name, Extern::JSONData const& data);

		JSONValue(Extern::JSONData const& data);

		JSONValue(JSONView const& view);

		JSONValue(JSONValue const& other);

		template <Makai::Type::Container::List T>
		bool tryGet(T& out) const
		requires Makai::Type::Equal<typename T::DataType, JSONValue>
		try {
			out = T(view().get<std::vector<Extern::JSONData>>());
			err = "";
			return true;
		} catch (Extern::Nlohmann::exception const& e) {
			err = e.what();
			return false;
		}

		template <Makai::Type::Container::SimpleMap T>
		bool tryGet(T& out) const
		requires (
			Makai::Type::Equal<typename T::KeyType, String>
		&&	Makai::Type::Equal<typename T::ValueType, JSONValue>
		) try {
			T res;
			for (auto [k, v]: view().items())
				res[k] = v;
			err = "";
			return true;
		} catch (Extern::Nlohmann::exception const& e) {
			err = e.what();
			return false;
		}

		template <Makai::Type::Container::SimpleMap T>
		bool tryGet(T& out) const
		requires (
			Makai::Type::Equal<typename T::KeyType, String>
		&&	Makai::Type::Different<typename T::ValueType, JSONValue>
		) try {
			T res;
			for (auto [k, v]: view().items())
				res[k] = v.get<typename T::ValueType>();
			err = "";
			return true;
		} catch (Extern::Nlohmann::exception const& e) {
			err = e.what();
			return false;
		}

		template<class T> T get() const						{return JSONView::get<T>();			}
		template<class T> T get(T const& fallback) const	{return JSONView::get<T>(fallback);	}

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
