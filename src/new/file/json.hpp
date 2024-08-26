#ifndef MAKAILIB_FILE_JSON_H
#define MAKAILIB_FILE_JSON_H

#define JSON_NO_IO
#include "_lib/nlohmann/json.hpp"
#include "../ctl/ctl.hpp"

namespace Makai::JSON {
	namespace Extern {
		using Nlohmann = nlohmann::json;
		using JSONData = nlohmann::ordered_json;
	}

	class JSONView: public DataView<Extern::JSONData> {
	public:
		JSONView(Extern::JSONData& _data, String const& _name = "<anonymous>");
		JSONView(Extern::JSONData const& _data, String const& _name = "<anonymous>");
		JSONView(JSONView const& other);
		JSONView(JSONView&& other);

		Extern::JSONData json() const;

		template<typename T>
		inline T get() const {
			try {
				return view().get<T>();
			} catch (Extern::Nlohmann::exception const& e) {
				throw Error::FailedAction(
					"Parameter '" + name + "' is not of type '"
					+ NAMEOF(typeid(T)) + "'!",
					__FILE__,
					::toString(__LINE__),
					::toString("get<", NAMEOF(typeid(T)), ">"),
					e.what()
				);
			}
		}

		template<typename T>
		inline T get(T const& fallback) const {
			try {
				return view().get<T>();
			} catch (Extern::Nlohmann::exception const& e) {
				return fallback;
			}
		}

		JSONView operator[](String const& key);
		const JSONView operator[](String const& key) const;

		JSONView operator[](size_t const& index);
		const JSONView operator[](size_t const& index) const;

		template<typename T>
		inline JSONView& operator=(T const& v) {
			view() = v;
			return (*this);
		}

		JSONView& operator=(JSONView const& v);

		template<typename T> operator T() const;

		String getName() const;

		String toString(int const& indent = -1, char const& ch = '\t') const;

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

	private:
		Extern::JSONData const& cdata;
		Extern::JSONData dummy;

		String const name;
	};

	struct JSONValue: public JSONView {
		JSONValue(String const& name = "<anonymous>");

		JSONValue(String const& name, Extern::JSONData const& data);

		JSONValue(Extern::JSONData const& data);

		JSONValue(JSONValue const& other);

		JSONValue& clear();

	private:
		Extern::JSONData data;
	};

	JSONValue object(String const& name = "<anonymous>");

	JSONValue array(String const& name = "<anonymous>");

	using JSONData = JSONValue;

	JSONData parseJSON(String const& data);
	JSONData loadFile(String const& path);
}

#endif // MAKAILIB_FILE_JSON_H
