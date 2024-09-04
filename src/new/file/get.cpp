#include "file.hpp"

#include "../tool/archive/archive.hpp"

#if !(defined(MAKAILIB_DEBUG) || defined(MAKAILIB_ARCHIVE_DISABLED))
#define _IMPL_ARCHIVE_
#endif

using Makai::Tool::Arch::FileArchive;

using namespace FileLoader;

#ifdef _IMPL_ARCHIVE_
bool				loadingArchive	= false;
FileArchive 		arc;
Error::ErrorPointer	arcfail			= nullptr;
#endif

using Makai::File::BinaryData;

// Until this puzzle is figured, this shall do
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"

/*#undef _ARCHIVE_SYSTEM_DISABLED_
#define _ARCHIVE_SYSTEM_DISABLED_*/

void Makai::File::attachArchive(String const& path, String const& password) {
	#ifdef _IMPL_ARCHIVE_
	DEBUGLN("Attaching archive...");
	if (loadingArchive)
		fileLoadError(path, "Other archive is being loaded!");
	try {
		arcfail = nullptr;
		arc.close();
		arc.open(path, password);
		DEBUGLN("Archive Attached!");
	} catch (...) {
		DEBUGLN("Archive attachment failed!");
		arcfail = Error::current();
	}
	#endif
}

bool Makai::File::isArchiveAttached() {
	#ifdef _IMPL_ARCHIVE_
	return arc.isOpen();
	#else
	return false;
	#endif
}

[[gnu::destructor]] void Makai::File::detachArchive() {
	#ifdef _IMPL_ARCHIVE_
	DEBUGLN("Detaching archive...");
	arc.close();
	DEBUGLN("Archive detached!");
	#endif
}

#ifdef _IMPL_ARCHIVE_
void assertArchive(String const& path) {
	if (arcfail) Error::rethrow(arcfail);
	if (!Makai::File::isArchiveAttached())
		fileLoadError(path, "Archive is not attached!", "fileloader.hpp");
}

[[noreturn]] void fileGetError(String const& path, String const& fe, String const& ae) {
	fileLoadError(
		path,
		toString(
			"\nMultiple possibilities!\n\n",
			"[[ FOLDER ]]\n", fe, "\n",
			"[[ ARCHIVE ]]\n", ae, "\n"
		),
		"fileloader.hpp"
	);
}
#endif

String Makai::File::loadTextFileFromArchive(String const& path) {
	#ifdef _IMPL_ARCHIVE_
	assertArchive(path);
	return arc.getTextFile(FileSystem::getChildPath(path));
	#else
	fileLoadError(path, "Archive functionality disabled!");
	#endif
}

BinaryData Makai::File::loadBinaryFileFromArchive(String const& path) {
	#ifdef _IMPL_ARCHIVE_
	assertArchive(path);
	return arc.getBinaryFile(FileSystem::getChildPath(path));
	#else
	fileLoadError(path, "Archive functionality disabled!");
	#endif
}

CSVData Makai::File::loadCSVFileFromArchive(String const& path, char const& delimiter) {
	#ifdef _IMPL_ARCHIVE_
	assertArchive(path);
	return Helper::splitString(arc.getTextFile(FileSystem::getChildPath(path)), delimiter);
	#else
	fileLoadError(path, "Archive functionality disabled!");
	#endif
}

Makai::JSON::JSONData Makai::File::loadJSONFileFromArchive(String const& path) {
	#ifdef _IMPL_ARCHIVE_
	assertArchive(path);
	return Makai::JSON::parseJSON(arc.getTextFile(FileSystem::getChildPath(path)));
	#else
	fileLoadError(path, "Archive functionality disabled!");
	#endif
}

String Makai::File::getTextFile(String const& path) {
	#ifdef _IMPL_ARCHIVE_
	String res;
	if (isArchiveAttached())
		try {
			DEBUGLN("[ARC] Loading text file...");
			res = Makai::File::loadTextFileFromArchive(path);
		} catch (FileLoadError const& ae) {
			try {
				DEBUGLN("[FLD-2] Loading text file...");
				res = loadTextFile(path);
			} catch (FileLoadError const& fe) {
				fileGetError(path, fe.summary(), ae.summary());
			}
		}
	else try {
		DEBUGLN("[FLD-1] Loading text file...");
		res = loadTextFile(path);
	} catch (FileLoadError const& e) {
		fileGetError(path, e.summary(), "Archive not attached!");
	}
	return res;
	#else
	return loadTextFile(path);
	#endif
}

BinaryData Makai::File::getBinaryFile(String const& path) {
	#ifdef _IMPL_ARCHIVE_
	BinaryData res;
	if (isArchiveAttached())
		try {
			DEBUGLN("[ARC] Loading binary file...");
			res = Makai::File::loadBinaryFileFromArchive(path);
		} catch (FileLoadError const& ae) {
			try {
				DEBUGLN("[FLD-2] Loading binary file...");
				res = loadBinaryFile(path);
			} catch (FileLoadError const& fe) {
				fileGetError(path, fe.summary(), ae.summary());
			}
		}
	else try {
		DEBUGLN("[FLD-1] Loading binary file...");
		res = loadBinaryFile(path);
	} catch (FileLoadError const& e) {
		fileGetError(path, e.summary(), "Archive not attached!");
	}
	return res;
	#else
	return loadBinaryFile(path);
	#endif
}

CSVData Makai::File::getCSVFile(String const& path, char const& delimiter) {
	#ifdef _IMPL_ARCHIVE_
	CSVData res;
	if (isArchiveAttached())
		try {
			DEBUGLN("[ARC] Loading CSV file...");
			res = Makai::File::loadCSVFileFromArchive(path);
		} catch (FileLoadError const& ae) {
			try {
				DEBUGLN("[FLD-2] Loading CSV file...");
				res = loadCSVFile(path);
			} catch (FileLoadError const& fe) {
				fileGetError(path, fe.summary(), ae.summary());
			}
		}
	else try {
		DEBUGLN("[FLD-1] Loading CSV file...");
		res = loadCSVFile(path);
	} catch (FileLoadError const& e) {
		fileGetError(path, e.summary(), "Archive not attached!");
	}
	return res;
	#else
	return loadCSVFile(path);
	#endif
}

Makai::JSON::JSONData Makai::File::getJSONFile(String const& path) {
	#ifdef _IMPL_ARCHIVE_
	Makai::JSON::JSONData res(FileSystem::getFileName(path));
	if (isArchiveAttached())
		try {
			DEBUGLN("[ARC] Loading JSON file...");
			res = Makai::File::loadJSONFileFromArchive(path);
		} catch (FileLoadError const& ae) {
			try {
				DEBUGLN("[FLD-2] Loading JSON file...");
				res = Makai::JSON::loadFile(path);
			} catch (FileLoadError const& fe) {
				fileGetError(path, fe.summary(), ae.summary());
			}
		}
	else try {
		DEBUGLN("[FLD-1] Loading JSON file...");
		res = Makai::JSON::loadFile(path);
	} catch (FileLoadError const& e) {
		fileGetError(path, e.summary(), "Archive not attached!");
	}
	return res;
	#else
	return Makai::JSON::loadFile(path);
	#endif
}

#pragma GCC diagnostic pop
