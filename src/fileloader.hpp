#ifndef MAKAI_FILE_LOADER
#define MAKAI_FILE_LOADER

#include "collection/helper.hpp"
#include "collection/filehandler.hpp"
#include <nlohmann/json.hpp>
#include <zip_utils/unzip.h>

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

	struct ZIPFile {
		constexpr ZIPFile() {}

		constexpr ZIPFile(String const& path) {open(path);}

		constexpr ~ZIPFile() {close();}

		constexpr ZIPFile& open(String const& path, String const& password = "") {
			if (isOpen()) return (*this);
			if (!FileSystem::exists(path))
				fileLoadError(path, "Archive does not exist!");
			file	= OpenZip(path.c_str(), password.c_str());
			if (!isOpen())
				fileLoadError(path, "Could not load archive!");
			name	= FileSystem::getFileName(path, true);
			ext		= regexFindFirst(FileSystem::getFileName(path, false), "(\\.[^.]+)$");
			fileOpen = true;
			return (*this);
		}

		constexpr ZIPFile& close() {
			if (!isOpen())  return (*this);
			CloseZip(file);
			file = nullptr;
			fileOpen = false;
			return (*this);
		};

		String getTextFile(String const& path) const {
			auto [idx, entry] = getFileInfo(path);
			String contents;
			contents.reserve(entry.unc_size);
			assertOK(UnzipItem(file, idx, contents.data(), contents.size()), path);
			return contents;
		}

		BinaryData getBinaryFile(String const& path) const {
			auto [idx, entry] = getFileInfo(path);
			BinaryData contents;
			contents.reserve(entry.unc_size);
			assertOK(UnzipItem(file, idx, contents.data(), contents.size()), path);
			return contents;
		}

		constexpr bool isOpen() const	{return fileOpen;}

		constexpr String getName() const		{return name;		}
		constexpr String getExtension() const	{return ext;		}
		constexpr String getFullName() const	{return name + ext;	}

	private:
		bool fileOpen = false;

		constexpr void assertOK(int const& res, String const& path) const {
			if (res != ZR_OK) {
				char error[1024];
				FormatZipMessage(res, error, 1024);
				fileLoadError(toString(name, ext, "/", path), error);
			}
		}

		Pair<int, ZIPENTRY> getFileInfo(String const& path) const {
			if (!isOpen()) fileLoadError(toString(path), "Archive not loaded!");
			int idx = -1;
			ZIPENTRY entry;
			assertOK(FindZipItem(file, path.c_str(), true, &idx, &entry), path);
			return Pair<int, ZIPENTRY>(idx, entry);
		}

		String name, ext;

		HZIP file = nullptr;
	};

	namespace {
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_)) || 1
		bool				loadingArchive	= false;
		#endif
		ZIPFile 			arc;
		Thread				arcLoader;
		Error::ErrorPointer	arcfail			= nullptr;
	}

	// Until this puzzle is figured, this shall do
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wreturn-type"

	/*#undef _ARCHIVE_SYSTEM_DISABLED_
	#define _ARCHIVE_SYSTEM_DISABLED_*/

	inline void attachArchive(string const& path, string const& password = "") {
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_)) || 1
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
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_)) || 1
		if (loadingArchive) return false;
		return arc.isOpen();
		#else
		return false;
		#endif
	}

	[[gnu::destructor]] inline void detachArchive() {
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_)) || 1
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
			if (FileSystem::getRootDirectory(path) != arc.getName())
				fileLoadError(path, "Attached archive name does not match root name!", "fileloader.hpp");
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
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_)) || 1
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
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_)) || 1
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
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_)) || 1
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
		#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_)) || 1
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
