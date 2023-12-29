#ifndef MAKAI_FILE_LOADER
#define MAKAI_FILE_LOADER

#include "collection/helper.hpp"
#include "collection/filehandler.hpp"
#include <nlohmann/json.hpp>

namespace FileLoader {
	inline nlohmann::ordered_json parseJSON(String const& data) {try {
		return nlohmann::ordered_json::parse(data);
		} catch (nlohmann::json::exception e) {
			throw Error::FailedAction(
				"Failed at loading JSON file!",
				__FILE__,
				toString(__LINE__),
				"parseJSON",
				e.what(),
				"Please check to see if values are correct!"
			);
		}
	}

	inline nlohmann::ordered_json loadJSON(String const& path) {
		return parseJSON(loadTextFile(path));
	}

	struct FileArchive {

	};

	//*
	#ifndef _TESTING_ARCHIVE_SYS_
	#define _TESTING_ARCHIVE_SYS_
	#endif // _TESTING_ARCHIVE_SYS_
	//*/

	namespace {
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_)) || defined(_TESTING_ARCHIVE_SYS_)
		bool				loadingArchive	= false;
		#endif
		FileArchive 		arc;
		Thread				arcLoader;
		Error::ErrorPointer	arcfail			= nullptr;
	}

	// Until this puzzle is figured, this shall do
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wreturn-type"

	/*#undef _ARCHIVE_SYSTEM_DISABLED_
	#define _ARCHIVE_SYSTEM_DISABLED_*/

	inline void attachArchive(string const& path, string const& password = "") {
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_)) || defined(_TESTING_ARCHIVE_SYS_)
		DEBUGLN("Attaching archive...");
		if (loadingArchive)
			fileLoadError(path, "Other archive is being loaded!");
		loadingArchive = true;
		arcLoader = Thread(
			[&] {
				try {
					arcfail = nullptr;
					arc.close();
					arc.open(path, password);
					loadingArchive = false;
					DEBUGLN("Archive Attached!");
				} catch (...) {
					DEBUGLN("Archive attachment failed!");
					arcfail = Error::current();
				}
			}
		);
		#endif
	}

	inline void awaitArchiveLoad() {
		if (arcLoader.joinable()) {
			DEBUGLN("Awaiting archive load...");
			arcLoader.join();
		}
	}

	inline bool isArchiveAttached() {
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_)) || defined(_TESTING_ARCHIVE_SYS_)
		if (loadingArchive) return false;
		return arc.isOpen();
		#else
		return false;
		#endif
	}

	[[gnu::destructor]] inline void detachArchive() {
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_)) || defined(_TESTING_ARCHIVE_SYS_)
		DEBUGLN("Detaching archive...");
		arc.close();
		DEBUGLN("Archive detached!");
		#endif
	}

	namespace {
		inline void assertArchive(String const& path) {
			awaitArchiveLoad();
			if (arcfail) Error::rethrow(arcfail);
			if (!isArchiveAttached())
				fileLoadError(path, "Archive is not attached!", "fileloader.hpp");
		}

		[[noreturn]] inline void fileGetError(String const& path, String const& fe, String const& ae) {
			fileLoadError(
				path,
				toString(
					"\nMultiple possibilities!\n\n",
					"FOLDER: ", fe, "\n",
					"ARCHIVE: ", ae, "\n"
				),
				"fileloader.hpp"
			);
		}
	}

	inline String loadTextFileFromArchive(String const& path) {
		assertArchive(path);
		return arc.getTextFile(path);
	}

	inline BinaryData loadBinaryFileFromArchive(String const& path) {
		assertArchive(path);
		return arc.getBinaryFile(path);
	}

	inline CSVData loadCSVFileFromArchive(String const& path, char const& delimiter = ',') {
		assertArchive(path);
		return Helper::splitString(arc.getTextFile(path), delimiter);
	}

	inline nlohmann::ordered_json loadJSONFromArchive(String const& path) {
		assertArchive(path);
		return parseJSON(arc.getTextFile(path));
	}

	inline String getTextFile(String const& path) {
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_)) || defined(_TESTING_ARCHIVE_SYS_)
		String res;
		awaitArchiveLoad();
		if (isArchiveAttached())
			try {
				DEBUGLN("[ARC] Loading text file...");
				res = loadTextFileFromArchive(path);
			} catch (FileLoadError const& ae) {
				try {
					DEBUGLN("[FLD-2] Loading text file...");
					res = loadTextFile(path);
				} catch (FileLoadError const& fe) {
					fileGetError(path, fe.info, ae.info);
				}
			}
		else try {
			DEBUGLN("[FLD-1] Loading text file...");
			res = loadTextFile(path);
		} catch (FileLoadError const& e) {
			fileGetError(path, e.info, "Archive not attached!");
		}
		return res;
		#else
		return loadTextFile(path);
		#endif
	}

	inline BinaryData getBinaryFile(String const& path) {
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_)) || defined(_TESTING_ARCHIVE_SYS_)
		BinaryData res;
		awaitArchiveLoad();
		if (isArchiveAttached())
			try {
				DEBUGLN("[ARC] Loading binary file...");
				res = loadBinaryFileFromArchive(path);
			} catch (FileLoadError const& ae) {
				try {
					DEBUGLN("[FLD-2] Loading binary file...");
					res = loadBinaryFile(path);
				} catch (FileLoadError const& fe) {
					fileGetError(path, fe.info, ae.info);
				}
			}
		else try {
			DEBUGLN("[FLD-1] Loading binary file...");
			res = loadBinaryFile(path);
		} catch (FileLoadError const& e) {
			fileGetError(path, e.info, "Archive not attached!");
		}
		return res;
		#else
		return loadBinaryFile(path);
		#endif
	}

	inline CSVData getCSVFile(String const& path, char const& delimiter = ',') {
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_)) || defined(_TESTING_ARCHIVE_SYS_)
		CSVData res;
		awaitArchiveLoad();
		if (isArchiveAttached())
			try {
				DEBUGLN("[ARC] Loading CSV file...");
				res = loadCSVFileFromArchive(path);
			} catch (FileLoadError const& ae) {
				try {
					DEBUGLN("[FLD-2] Loading CSV file...");
					res = loadCSVFile(path);
				} catch (FileLoadError const& fe) {
					fileGetError(path, fe.info, ae.info);
				}
			}
		else try {
			DEBUGLN("[FLD-1] Loading CSV file...");
			res = loadCSVFile(path);
		} catch (FileLoadError const& e) {
			fileGetError(path, e.info, "Archive not attached!");
		}
		return res;
		#else
		return loadCSVFile(path);
		#endif
	}

	inline nlohmann::ordered_json getJSON(String const& path) {
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_)) || defined(_TESTING_ARCHIVE_SYS_)
		nlohmann::ordered_json res;
		awaitArchiveLoad();
		if (isArchiveAttached())
			try {
				DEBUGLN("[ARC] Loading JSON file...");
				res = loadJSONFromArchive(path);
			} catch (FileLoadError const& ae) {
				try {
					DEBUGLN("[FLD-2] Loading JSON file...");
					res = loadJSON(path);
				} catch (FileLoadError const& fe) {
					fileGetError(path, fe.info, ae.info);
				}
			}
		else try {
			DEBUGLN("[FLD-1] Loading JSON file...");
			res = loadJSON(path);
		} catch (FileLoadError const& e) {
			fileGetError(path, e.info, "Archive not attached!");
		}
		return res;
		#else
		return loadJSON(path);
		#endif
	}

	#pragma GCC diagnostic pop
}

#endif // MAKAI_FILE_LOADER
