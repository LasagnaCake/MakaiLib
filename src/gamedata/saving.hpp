class SaveDataView: public DataView<JSONData> {
public:
	SaveDataView(JSONData& source, String const& _name):	DataView(source), name(_name)		{}
	SaveDataView(SaveDataView const& other):				DataView(other), name(other.name)	{}
	SaveDataView(SaveDataView&& other):						DataView(other), name(other.name)	{}

	template<typename T>
	T get(T const& fallback) {
		try {
			return value().get<T>();
		} catch (JSON::exception e) {
			view() = fallback;
			return fallback;
		}
	}

	SaveDataView operator[](String const& key) {
		if (isNull()) view()[key] = JSONData();
		else if (!isObject()) throw Error::InvalidAction("Parameter '" + name + "' is not an object!");
		return SaveDataView(view()[key], key);
	}

	SaveDataView operator[](size_t const& index) {
		if (isNull()) view()[index] = JSONData();
		else if (!isArray()) throw Error::InvalidAction("Parameter '" + name + "' is not an array!");
		return SaveDataView(view()[index], "index:"+toString(index));
	}

	template<typename T>
	SaveDataView& operator=(T const& v) {
		view() = v;
		return (*this);
	}

	SaveDataView& save(String const& path) {
		FileLoader::saveTextFile(path, value().dump(1, '\t', false, JSON::error_handler_t::replace));
		return (*this);
	}

	SaveDataView& load(String const& path) {
		view() = FileLoader::loadJSON(path);
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

class SaveFile {
public:
	SaveFile() {}

	SaveFile(JSONData const& data) {this->data = data;}

	SaveFile(String const& path) {load(path);}

	SaveFile& load(String const& path) {
		data = FileLoader::loadJSON(path);
		return (*this);
	}

	SaveFile& close(String const& path) {return save(path).destroy();}

	SaveFile& destroy() {
		data = JSONData();
		return (*this);
	}

	SaveFile& save(String const& path) {
		FileLoader::saveTextFile(path, file().dump(1, '\t', false, JSON::error_handler_t::replace));
		return (*this);
	}

	template<typename T>
	T get(String const& key, T const& fallback) {
		try {
			return (*this)[key].get<T>();
		} catch (JSON::exception e) {
			(*this)[key] = fallback;
			return fallback;
		}
	}

	SaveDataView operator[](string const& key) {
		if (data[key].is_null()) data[key] = JSONData();
		return SaveDataView(data[key], key);
	}

	SaveDataView getView() {return SaveDataView(file(), "@root");}

	JSONData value() {return file();}

	SaveFile& operator=(JSONData const& data) {
		if (!data.is_object()) throw Error::InvalidValue("Save data must be explicitly a JSON object!");
		this->data = data;
		return (*this);
	}

	SaveFile& operator()(String const& path) {
		return save(path);
	}

	bool exists() {return data.is_object();}

private:
	JSONData& file() {
		if (!exists()) throw Error::NonexistentValue("Save file wasn't created!");
		return data;
	}

	JSONData	data	= JSONData();
};
