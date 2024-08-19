#include "json.hpp"

using JSON = nlohmann::json;

template<typename T>
constexpr T Makai::JSON::JSONView::get() const {
	try {
		return value().get<T>();
	} catch (JSON::exception const& e) {
		throw Error::FailedAction(
			"Parameter '" + name + "' is not of type '"
			+ NAMEOF(typeid(T)) + "'!",
			__FILE__,
			toString(__LINE__),
			toString("get<", NAMEOF(typeid(T)), ">"),
			e.what()
		);
	}
}

template<typename T>
constexpr T Makai::JSON::JSONView::get(T const& fallback) const {
	try {
		return value().get<T>();
	} catch (JSON::exception const& e) {
		return fallback;
	}
}

constexpr Makai::JSON::JSONView Makai::JSON::JSONView::operator[](String const& key) {
	if (isNull()) view() = JSON::object();
	else if (!isObject()) throw Error::InvalidAction("Parameter '" + name + "' is not an object!");
	return Makai::JSON::JSONView(view()[key], name + "/" + key);
}

constexpr const Makai::JSON::JSONView Makai::JSON::JSONView::operator[](String const& key) const {
	else if (!isObject()) throw Error::InvalidAction("Parameter '" + name + "' is not an object!");
	return Makai::JSON::JSONView(view()[key], name + "/" + key);
}

constexpr Makai::JSON::JSONView Makai::JSON::JSONView::operator[](size_t const& index) {
	if (isNull()) view() = JSON::array();
	else if (!isArray()) throw Error::InvalidAction("Parameter '" + name + "' is not an array!");
	return Makai::JSON::JSONView(view()[index], toString(name, "[", toString(index), "]"));
}

constexpr const Makai::JSON::JSONView Makai::JSON::JSONView::operator[](size_t const& index) const {
	else if (!isArray()) throw Error::InvalidAction("Parameter '" + name + "' is not an array!");
	return Makai::JSON::JSONView(view()[index], toString(name, "[", toString(index), "]"));
}

template<typename T>
constexpr Makai::JSON::JSONView& Makai::JSON::JSONView::operator=(T const& v) {
	view() = v;
	return (*this);
}

template<typename T>
constexpr Makai::JSON::JSONView::operator T() const {
	return get<T>(T());
}

constexpr String Makai::JSON::JSONView::getName() const {return name;}

constexpr bool Makai::JSON::JSONView::isNull() const		{return view().is_null();				}
constexpr bool Makai::JSON::JSONView::isInt() const			{return view().is_number_integer();		}
constexpr bool Makai::JSON::JSONView::isFloat() const		{return view().is_number_float();		}
constexpr bool Makai::JSON::JSONView::isNumber() const		{return view().is_number();				}
constexpr bool Makai::JSON::JSONView::isObject() const		{return view().is_object();				}
constexpr bool Makai::JSON::JSONView::isUnsigned() const	{return view().is_number_unsigned();	}
constexpr bool Makai::JSON::JSONView::isArray() const		{return view().is_array();				}
constexpr bool Makai::JSON::JSONView::isBool() const		{return view().is_boolean();			}
constexpr bool Makai::JSON::JSONView::isString() const		{return view().is_string();				}
constexpr bool Makai::JSON::JSONView::isPrimitive() const	{return view().is_primitive();			}
constexpr bool Makai::JSON::JSONView::isStructured() const	{return view().is_structured();			}
constexpr bool Makai::JSON::JSONView::isDiscarded() const	{return view().is_discarded();			}


constexpr Makai::JSON::JSONValue::JSONValue(String const& name) {}

constexpr Makai::JSON::JSONValue::JSONValue(String const& name, Makai::JSON::Extern::JSONData const& data) {this->data = data;}

constexpr Makai::JSON::JSONValue::JSONValue(Makai::JSON::Extern::JSONData const& data) {}

constexpr Makai::JSON::JSONValue::JSONValue(Makai::JSON::JSONValue const& other) {}

constexpr Makai::JSON::JSONValue& clear() {
	data = JSON::object();
	return *this;
}

constexpr Makai::JSON::JSONValue Makai::JSON::object(String const& name = "") {return JSONValue(name, JSON::object());}

constexpr Makai::JSON::JSONValue Makai::JSON::array(String const& name = "") {return JSONValue(name, JSON::array());}

constexpr Makai::JSON::JSONData Makai::JSON::parseJSON(String const& data) try {
	return Makai::Extern::JSONData::parse(data);
} catch (JSON::exception const& e) {
	throw Error::FailedAction(
		"Failed at parsing JSON!",
		__FILE__,
		toString(__LINE__),
		"parseJSON",
		e.what(),
		"Please check to see if values are correct!"
	);
}

constexpr Makai::JSON::JSONData Makai::JSON::loadFile(String const& path) {
	return Makai::JSON::parseJSON(FileLoader::loadTextFile(path));
}
