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
	#ifndef _ARCHIVE_SYSTEM_DISABLED_
	#define _ARCHIVE_SYSTEM_DISABLED_
	#endif // _ARCHIVE_SYSTEM_DISABLED_
	//*/

	#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_)) || defined(_TESTING_ARCHIVE_SYS_)
	#define _IMPL_ARCHIVE_
	#endif

	namespace {
		#ifdef _IMPL_ARCHIVE_
		bool				loadingArchive	= false;
		FileArchive 		arc;
		Thread				arcLoader;
		Error::ErrorPointer	arcfail			= nullptr;
		#endif
	}

	// Until this puzzle is figured, this shall do
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wreturn-type"

	/*#undef _ARCHIVE_SYSTEM_DISABLED_
	#define _ARCHIVE_SYSTEM_DISABLED_*/

	inline void attachArchive(string const& path, string const& password = "") {
		#ifdef _IMPL_ARCHIVE_
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
		#ifdef _IMPL_ARCHIVE_
		if (arcLoader.joinable()) {
			DEBUGLN("Awaiting archive load...");
			arcLoader.join();
		}
		#endif
	}

	inline bool isArchiveAttached() {
		#ifdef _IMPL_ARCHIVE_
		if (loadingArchive) return false;
		return arc.isOpen();
		#else
		return false;
		#endif
	}

	[[gnu::destructor]] inline void detachArchive() {
		#ifdef _IMPL_ARCHIVE_
		DEBUGLN("Detaching archive...");
		arc.close();
		DEBUGLN("Archive detached!");
		#endif
	}

	#ifdef _IMPL_ARCHIVE_
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
	#endif

	inline String loadTextFileFromArchive(String const& path) {
		#ifdef _IMPL_ARCHIVE_
		assertArchive(path);
		return arc.getTextFile(path);
		#else
		fileLoadError(path, "Archive functionality disabled!");
		#endif
	}

	inline BinaryData loadBinaryFileFromArchive(String const& path) {
		#ifdef _IMPL_ARCHIVE_
		assertArchive(path);
		return arc.getBinaryFile(path);
		#else
		fileLoadError(path, "Archive functionality disabled!");
		#endif
	}

	inline CSVData loadCSVFileFromArchive(String const& path, char const& delimiter = ',') {
		#ifdef _IMPL_ARCHIVE_
		assertArchive(path);
		return Helper::splitString(arc.getTextFile(path), delimiter);
		#else
		fileLoadError(path, "Archive functionality disabled!");
		#endif
	}

	inline nlohmann::ordered_json loadJSONFromArchive(String const& path) {
		#ifdef _IMPL_ARCHIVE_
		assertArchive(path);
		return parseJSON(arc.getTextFile(path));
		#else
		fileLoadError(path, "Archive functionality disabled!");
		#endif
	}

	inline String getTextFile(String const& path) {
		#ifdef _IMPL_ARCHIVE_
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
		#ifdef _IMPL_ARCHIVE_
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
		#ifdef _IMPL_ARCHIVE_
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
		#ifdef _IMPL_ARCHIVE_
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
