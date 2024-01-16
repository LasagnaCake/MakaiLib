class JSONView: public DataView<JSONData> {
public:
	JSONView(JSONData& _data, String const& _name):	DataView(_data), name(_name)		{}
	JSONView(JSONView const& other):				DataView(other), name(other.name)	{}
	JSONView(JSONView&& other):						DataView(other), name(other.name)	{}

	template<typename T>
	T get() {
		try {
			return value().get<T>();
		} catch (JSON::exception e) {
			throw Error::FailedAction(
				"Parameter '" + name + "' is not of type '"
				+ typeid(T).name() + "'!",
				__FILE__,
				toString(__LINE__),
				toString("get<", typeid(T).name(), ">"),
				e.what()
			);
		}
	}

	template<typename T>
	T get(T const& fallback) {
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
		return JSONView(view()[key], key);
	}

	JSONView operator[](size_t const& index) {
		if (isNull()) view() = JSON::array();
		else if (!isArray()) throw Error::InvalidAction("Parameter '" + name + "' is not an array!");
		return JSONView(view()[index], toString("index:", toString(index)));
	}

	template<typename T>
	JSONView& operator=(T const& v) {
		view() = v;
		return (*this);
	}

	inline bool isNull()		{return view().is_null();				}
	inline bool isInt()			{return view().is_number_integer();		}
	inline bool isFloat()		{return view().is_number_float();		}
	inline bool isNumber()		{return view().is_number();				}
	inline bool isObject()		{return view().is_object();				}
	inline bool isUnsigned()	{return view().is_number_unsigned();	}
	inline bool isArray()		{return view().is_array();				}
	inline bool isBool()		{return view().is_boolean();			}
	inline bool isString()		{return view().is_string();				}
	inline bool isPrimitive()	{return view().is_primitive();			}
	inline bool isStructured()	{return view().is_structured();			}
	inline bool isDiscarded()	{return view().is_discarded();			}

private:
	String const name;
};
