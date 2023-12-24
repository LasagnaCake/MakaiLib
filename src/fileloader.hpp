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
		arc.name = FileSystem::getFileName(path, true);
	}

	inline bool isArchiveAttached() {
		return arc.name.empty();
	}

	void detachArchive() {
	}
	namespace {
		inline void assertArchive(String const& path) {
			if (!isArchiveAttached())
				fileLoadError(path, "Archive is not attached!");
			if (FileSystem::getRootDirectory(path) != arc.name)
				fileLoadError(path, "Attached archive name does not match root name!");
		}

		[[noreturn]] inline void fileGetError(String const& path, String const& fe, String const& ae) {
			fileLoadError(
				path,
				toString(
					"Multiple possibilities:\n\n",
					"FOLDER: ", ae, "\n\n",
					"ARCHIVE: ", fe, "\n"
				)
			);
		}
	}

	BinaryData loadDataFromArchive(string const& path) {
		assertArchive(path);
		fileLoadError(path, "Unimplemented archive functionality!");
	}

	string loadTextFileFromArchive(string const& path) {
		assertArchive(path);
		fileLoadError(path, "Unimplemented archive functionality!");
	}

	BinaryData loadBinaryFileFromArchive(string const& path) {
		assertArchive(path);
		fileLoadError(path, "Unimplemented archive functionality!");
	}

	inline CSVData loadCSVFileFromArchive(string const& path) {
		assertArchive(path);
		fileLoadError(path, "Unimplemented archive functionality!");
	}

	inline nlohmann::ordered_json loadJSONFromArchive(string const& path) {
		assertArchive(path);
		fileLoadError(path, "Unimplemented archive functionality!");
	}

	#define _IGNORE_AND_MOVE_ON_

	inline string getTextFile(string const& path) {
		string res;
		#if (!defined(_DEBUG_OUTPUT_) && !defined(_IGNORE_AND_MOVE_ON_))
		try {
		#endif // _DEBUG_OUTPUT_
			res = loadTextFile(path);
		#if (!defined(_DEBUG_OUTPUT_) && !defined(_IGNORE_AND_MOVE_ON_))
		} catch (Error::FailedAction fe) {
			try {
				res = loadTextFileFromArchive(path);
			} catch (Error::FailedAction ae) {
				fileGetError(path, fe.info, ae.info);
			}
		}
		#endif // _DEBUG_OUTPUT_
		return res;
	}

	inline BinaryData getBinaryFile(string const& path) {
		BinaryData res;
		#if (!defined(_DEBUG_OUTPUT_) && !defined(_IGNORE_AND_MOVE_ON_))
		try {
		#endif // _DEBUG_OUTPUT_
			res = loadBinaryFile(path);
		#if (!defined(_DEBUG_OUTPUT_) && !defined(_IGNORE_AND_MOVE_ON_))
		} catch (Error::FailedAction fe) {
			try {
				res = loadBinaryFileFromArchive(path);
			} catch (Error::FailedAction ae) {
				fileGetError(path, fe.info, ae.info);
			}
		}
		#endif // _DEBUG_OUTPUT_
		return res;
	}

	inline CSVData getCSVFile(string const& path) {
		CSVData res;
		#if (!defined(_DEBUG_OUTPUT_) && !defined(_IGNORE_AND_MOVE_ON_))
		try {
		#endif // _DEBUG_OUTPUT_
			res = loadCSVFile(path);
		#if (!defined(_DEBUG_OUTPUT_) && !defined(_IGNORE_AND_MOVE_ON_))
		} catch (Error::FailedAction fe) {
			try {
				res = loadCSVFileFromArchive(path);
			} catch (Error::FailedAction ae) {
				fileGetError(path, fe.info, ae.info);
			}
		}
		#endif // _DEBUG_OUTPUT_
		return res;
	}

	inline nlohmann::ordered_json getJSON(string const& path) {
		nlohmann::ordered_json res;
		#if (!defined(_DEBUG_OUTPUT_) && !defined(_IGNORE_AND_MOVE_ON_))
		try {
		#endif // _DEBUG_OUTPUT_
			res = loadJSON(path);
		#if (!defined(_DEBUG_OUTPUT_) && !defined(_IGNORE_AND_MOVE_ON_))
		} catch (Error::FailedAction fe) {
			try {
				res = loadJSONFromArchive(path);
			} catch (Error::FailedAction ae) {
				fileGetError(path, fe.info, ae.info);
			}
		}
		#endif // _DEBUG_OUTPUT_
		return res;
	}

	#undef _IGNORE_AND_MOVE_ON_

	#pragma GCC diagnostic pop
}

#endif // MAKAI_FILE_LOADER
