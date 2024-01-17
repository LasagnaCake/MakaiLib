class JSONView: public DataView<JSONData> {
public:
	constexpr JSONView(JSONData& _data, String const& _name):	DataView(_data), name(_name)		{}
	constexpr JSONView(JSONView const& other):					DataView(other), name(other.name)	{}
	constexpr JSONView(JSONView&& other):						DataView(other), name(other.name)	{}

	template<typename T>
	constexpr T get() {
		try {
			return value().get<T>();
		} catch (JSON::exception e) {
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
	constexpr T get(T const& fallback) {
		try {
			return value().get<T>();
		} catch (JSON::exception e) {
			view() = fallback;
			return fallback;
		}
	}

	JSONView operator[](String const& key) {
		if (isNull()) view() = JSON::object();
		else if (!isObject()) throw Error::InvalidAction("Parameter '" + name + "' is not an object!");
		return JSONView(view()[key], name + "/" + key);
	}

	JSONView operator[](size_t const& index) {
		if (isNull()) view() = JSON::array();
		else if (!isArray()) throw Error::InvalidAction("Parameter '" + name + "' is not an array!");
		return JSONView(view()[index], toString(name, "[", toString(index), "]"));
	}

	template<typename T>
	constexpr JSONView& operator=(T const& v) {
		view() = v;
		return (*this);
	}

	constexpr bool isNull() const		{return view().is_null();				}
	constexpr bool isInt() const		{return view().is_number_integer();		}
	constexpr bool isFloat() const		{return view().is_number_float();		}
	constexpr bool isNumber() const		{return view().is_number();				}
	constexpr bool isObject() const		{return view().is_object();				}
	constexpr bool isUnsigned() const	{return view().is_number_unsigned();	}
	constexpr bool isArray() const		{return view().is_array();				}
	constexpr bool isBool() const		{return view().is_boolean();			}
	constexpr bool isString() const		{return view().is_string();				}
	constexpr bool isPrimitive() const	{return view().is_primitive();			}
	constexpr bool isStructured() const	{return view().is_structured();			}
	constexpr bool isDiscarded() const	{return view().is_discarded();			}

private:
	String const name;
};
