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

	#pragma push_macro("_ARCHIVE_SYSTEM_DISABLED_")

	#undef _ARCHIVE_SYSTEM_DISABLED_
	#define _ARCHIVE_SYSTEM_DISABLED_

	void attachArchive(string const& path, string const& key) {
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_))
		arc.name = FileSystem::getFileName(path, true);
		#endif
	}

	inline bool isArchiveAttached() {
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_))
		return arc.name.empty();
		#else
		return true;
		#endif
	}

	void detachArchive() {
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_))
		#endif
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
		fileLoadError(path, "Unimplemented archive functionality!");
		assertArchive(path);
	}

	string loadTextFileFromArchive(string const& path) {
		fileLoadError(path, "Unimplemented archive functionality!");
		assertArchive(path);
	}

	BinaryData loadBinaryFileFromArchive(string const& path) {
		fileLoadError(path, "Unimplemented archive functionality!");
		assertArchive(path);
	}

	inline CSVData loadCSVFileFromArchive(string const& path) {
		fileLoadError(path, "Unimplemented archive functionality!");
		assertArchive(path);
	}

	inline nlohmann::ordered_json loadJSONFromArchive(string const& path) {
		fileLoadError(path, "Unimplemented archive functionality!");
		assertArchive(path);
	}

	inline string getTextFile(string const& path) {
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_))
		string res;
		try {
			res = loadTextFile(path);
		} catch (Error::FailedAction fe) {
			try {
				res = loadTextFileFromArchive(path);
			} catch (Error::FailedAction ae) {
				fileGetError(path, fe.info, ae.info);
			}
		}
		return res;
		#else
		return loadTextFile(path);
		#endif
	}

	inline BinaryData getBinaryFile(string const& path) {
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_))
		BinaryData res;
		try {
			res = loadBinaryFile(path);
		} catch (Error::FailedAction fe) {
			try {
				res = loadBinaryFileFromArchive(path);
			} catch (Error::FailedAction ae) {
				fileGetError(path, fe.info, ae.info);
			}
		}
		return res;
		#else
		return loadBinaryFile(path);
		#endif
	}

	inline CSVData getCSVFile(string const& path) {
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_))
		CSVData res;
		try {
			res = loadCSVFile(path);
		} catch (Error::FailedAction fe) {
			try {
				res = loadCSVFileFromArchive(path);
			} catch (Error::FailedAction ae) {
				fileGetError(path, fe.info, ae.info);
			}
		}
		return res;
		#else
		return loadCSVFile(path);
		#endif
	}

	inline nlohmann::ordered_json getJSON(string const& path) {
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_))
		nlohmann::ordered_json res;
		try {
			res = loadJSON(path);
		} catch (Error::FailedAction fe) {
			try {
				res = loadJSONFromArchive(path);
			} catch (Error::FailedAction ae) {
				fileGetError(path, fe.info, ae.info);
			}
		}
		return res;
		#else
		return loadJSON(path);
		#endif
	}

	#pragma pop_macro("_ARCHIVE_SYSTEM_DISABLED_")

	#pragma GCC diagnostic pop
}

#endif // MAKAI_FILE_LOADER
