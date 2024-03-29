class SaveDataView: public JSONView {
public:
	using JSONView::JSONView;

	SaveDataView const& save(String const& path, String const& pass = "") const {
		saveToFile(path, pass);
		return (*this);
	}

	SaveDataView& save(String const& path, String const& pass = "") {
		saveToFile(path, pass);
		return (*this);
	}

	SaveDataView& load(String const& path) {
		view() = FileLoader::getJSON(path);
		return (*this);
	}

private:
	void saveToFile(String const& path, String const& pass = "") const {
		String const content = value().dump(1, '\t', false, JSON::error_handler_t::replace);
		if (pass.empty()) FileLoader::saveTextFile(path, content);
		else ArcSys::saveEncryptedTextFile(path, content, pass);
	}
};

class SaveFile {
public:
	SaveFile() {}

	SaveFile(JSONData const& data) {this->data = data;}
	SaveFile(JSONView const& data) {this->data = data;}

	SaveFile(String const& path, String const& pass = "") {load(path, pass);}

	SaveFile& load(String const& path, String const& pass = "") {
		if (pass.empty()) data = FileLoader::loadJSON(path);
		else data = FileLoader::parseJSON(ArcSys::loadEncryptedTextFile(path, pass));
		return (*this);
	}

	inline SaveFile& close(String const& path, String const& pass = "") {return save(path, pass).destroy();}

	SaveFile& destroy() {
		data = JSON::object();
		return (*this);
	}

	SaveFile& save(String const& path, String const& pass = "") {
		String content = file().dump(1, '\t', false, JSON::error_handler_t::replace);
		if (pass.empty()) FileLoader::saveTextFile(path, content);
		else ArcSys::saveEncryptedTextFile(path, content, pass);
		return (*this);
	}

	template<typename T>
	T get(String const& key, T const& fallback) {
		try {
			return (*this)[key].get<T>();
		} catch (JSON::exception const& e) {
			(*this)[key] = fallback;
			return fallback;
		}
	}

	SaveDataView operator[](String const& key) {
		if (data[key].is_null()) data[key] = JSON::object();
		return SaveDataView(data[key], key);
	}

	SaveDataView getView() {return SaveDataView(file(), "@root");}

	JSONData value() {return file();}

	SaveFile& operator=(JSONView const& data) {
		if (!data.isObject()) throw Error::InvalidValue("Save data must be explicitly a JSON object!");
		this->data = data;
		return (*this);
	}

	SaveFile& operator=(JSONData const& data) {
		if (!data.is_object()) throw Error::InvalidValue("Save data must be explicitly a JSON object!");
		this->data = data;
		return (*this);
	}

	SaveFile& operator()(String const& path, String const& pass = "") {
		return save(path, pass);
	}

	inline bool exists() {return data.is_object();}

private:
	JSONData& file() {
		if (!exists()) throw Error::NonexistentValue("Save file wasn't created!");
		return data;
	}

	JSONData	data	= JSONData::object();
};
