class SaveFile {
public:
	SaveFile() {}

	SaveFile(string path) {
		load(path);
	}

	void load(string path) {
		if (loaded) return;
		loaded = true;
		source = new mINI::INIFile(path);
		if (!source->read(data)) {
			source->generate(data, true);
		}
	}

	void unload() {
		if (!loaded) return;
		loaded = false;
		delete source;
	}

	void save() {
		if (!loaded) return;
		source->write(data, true);
	}

	auto get(string key) {
		return structure().get(key);
	}

	void remove(string section) {
		structure().remove(section);
	}

	size_t size() {
		return structure().size();
	}

	auto& operator[](string key) {
		return structure()[key];
	}

	void operator()() {
		save();
	}

private:
	mINI::INIStructure& structure() {
		if (!source) throw std::runtime_error("Save file wasn't loaded!");
		return data;
	}

	bool loaded			= false;
	mINI::INIFile*		source = nullptr;
	mINI::INIStructure	data;
};
