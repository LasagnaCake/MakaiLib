#ifndef MAKAI_FILE_LOADER
#define MAKAI_FILE_LOADER

#include "collection/filehandler.hpp"
#include <nlohmann/json.hpp>

namespace FileLoader {
	inline nlohmann::json loadJSON(string const& path) {
		try {
			return nlohmann::json::parse(loadTextFile(path));
		} catch (nlohmann::json::exception e) {
				throw Error::FailedAction(
					"Failed at loading JSON file!",
					"fileloader.hpp",
					"10",
					"loadJSON",
					e.what(),
					"Please check to see if values are correct!"
				);
			}
	}
}

#endif // MAKAI_FILE_LOADER
