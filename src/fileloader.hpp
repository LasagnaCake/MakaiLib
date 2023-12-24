#ifndef MAKAI_FILE_LOADER
#define MAKAI_FILE_LOADER

#include "collection/filehandler.hpp"
#include <nlohmann/json.hpp>

namespace FileLoader {
	inline nlohmann::ordered_json loadJSON(string const& path) {
		try {
			return nlohmann::ordered_json::parse(loadTextFile(path));
		} catch (nlohmann::json::exception e) {
			throw Error::FailedAction(
				"Failed at loading JSON file!",
				__FILE__,
				toString(__LINE__),
				"loadJSON",
				e.what(),
				"Please check to see if values are correct!"
			);
		}
	}

	namespace {
		struct Archive {
			string name;
		} arc;
	}

	// Until this puzzle is figured, this shall do
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wreturn-type"

	void attachArchive(string const& path, string const& key) {
		arc.name = FileSystem::getRootDirectory(path);
	}

	void detachArchive() {
	}

	BinaryData loadDataFromArchive(string const& path) {
	}

	string loadTextFileFromArchive(string const& path) {
	}

	BinaryData loadBinaryFileFromArchive(string const& path) {
	}

	inline CSVData loadCSVFileFromArchive(string const& path) {
	}

	inline string getTextFile() {
	}

	inline BinaryData getBinaryFile() {
	}

	inline CSVData getCSVFile() {
	}

	#pragma GCC diagnostic pop
}

#endif // MAKAI_FILE_LOADER
