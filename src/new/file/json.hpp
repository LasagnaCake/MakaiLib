#ifndef MAKAILIB_FILE_JSON_H
#define MAKAILIB_FILE_JSON_H

#define JSON_NO_IO
#include "_lib/nlohmann/json.hpp"
#include "../ctl/ctl.hpp"

namespace Makai::JSON {
	namespace Extern {
		using JSONData = nlohmann::ordered_json;
	}

	class JSONView: public DataView<Extern::JSONData> {
	public:
		constexpr JSONView(Extern::JSONData& _data, String const& _name);
		constexpr JSONView(JSONView const& other);
		constexpr JSONView(JSONView&& other);

		template<typename T> constexpr T get() const;

		template<typename T> constexpr T get(T const& fallback) const;

		constexpr JSONView operator[](String const& key);
		constexpr const JSONView operator[](String const& key) const;

		constexpr JSONView operator[](size_t const& index);
		constexpr const JSONView operator[](size_t const& index) const;

		template<typename T> constexpr JSONView& operator=(T const& v);

		template<typename T> constexpr operator T() const;

		constexpr String getName() const;

		constexpr bool isNull() const;
		constexpr bool isInt() const;
		constexpr bool isFloat() const;
		constexpr bool isNumber() const;
		constexpr bool isObject() const;
		constexpr bool isUnsigned() const;
		constexpr bool isArray() const;
		constexpr bool isBool() const;
		constexpr bool isString() const;
		constexpr bool isPrimitive() const;
		constexpr bool isStructured() const;
		constexpr bool isDiscarded() const;

	private:
		String const name;
	};

	struct JSONValue: public JSONView {
		constexpr JSONValue(String const& name): JSONView(data, name);

		constexpr JSONValue(String const& name, Extern::JSONData const& data): JSONValue(name);

		constexpr JSONValue(Extern::JSONData const& data): JSONValue("", data);

		constexpr JSONValue(JSONValue const& other): JSONValue(other.getName(), other.data);

		constexpr JSONValue& clear();

	private:
		Extern::JSONData data;
	};

	constexpr JSONValue object();

	constexpr JSONValue array();

	typedef JSONValue JSONData;

	constexpr JSONData parseJSON(String const& data);
	JSONData loadFile(String const& path);
}
