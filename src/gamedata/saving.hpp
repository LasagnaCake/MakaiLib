class SaveFile {
public:
	SaveFile() {}

	SaveFile(string path) {
		load(path);
	}

	void load(string path) {
		if (loaded) return;
		loaded = true;
		data = FileLoader::loadJSON(path);
		this->path = path;
	}

	void close() {save();}

	void save() {
		if (!loaded) return;
		FileLoader::saveTextFile(path, data.dump(1, '\t', false, JSON::error_handler_t::replace));
	}

	template<typename T>
	T& get(string key) {
		return file().get<T>(key);
	}

	auto& operator[](string key) {
		return file()[key];
	}

	void operator()() {
		save();
	}

private:
	JSONData& file() {
		if (!loaded) throw Error::NonexistentValue("Save file wasn't loaded!");
		return data;
	}

	string		path	= "";
	bool		loaded	= false;
	JSONData	data;
};
