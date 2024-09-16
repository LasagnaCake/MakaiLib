#include "file.hpp"

#include "../tool/archive/archive.hpp"

#include <fstream>
#include <sstream>
#include <filesystem>

#if !(defined(MAKAILIB_DEBUG) || defined(MAKAILIB_ARCHIVE_DISABLED))
#define _IMPL_ARCHIVE_
#endif

using Makai::Tool::Arch::FileArchive;

#ifdef _IMPL_ARCHIVE_
enum class ArchiveState {
	FAS_CLOSED,
	FAS_LOADING,
	FAS_OPEN
} state = ArchiveState::FAS_CLOSED;

FileArchive 		arc;
Error::ErrorPointer	arcfail			= nullptr;
#endif

using Makai::File::BinaryData;

[[noreturn]] inline void fileLoadError(String const& path, String const& reason) {
	throw Makai::File::FileLoadError(
		"Could not load file '" + path + "'!",
		__FILE__,
		"unspecified",
		"unspecified",
		reason
	);
}

[[noreturn]] inline void fileSaveError(String const& path, String const& reason) {
	throw Makai::File::FileLoadError(
		"Could not save file '" + path + "'!",
		__FILE__,
		"unspecified",
		"unspecified",
		reason
	);
}

// Until this puzzle is figured, this shall do
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"

/*#undef _ARCHIVE_SYSTEM_DISABLED_
#define _ARCHIVE_SYSTEM_DISABLED_*/

void Makai::File::attachArchive(String const& path, String const& password) {
	#ifdef _IMPL_ARCHIVE_
	DEBUGLN("Attaching archive...");
	if (state == ArchiveState::FAS_LOADING)
		fileLoadError(path, "Other archive is being loaded!");
	try {
		arcfail = nullptr;
		state = ArchiveState::FAS_LOADING;
		arc.close();
		arc.open(path, password);
		state = ArchiveState::FAS_OPEN;
		DEBUGLN("Archive Attached!");
	} catch (...) {
		DEBUGLN("Archive attachment failed!");
		arcfail = Error::current();
	}
	#endif
}

bool Makai::File::isArchiveAttached() {
	#ifdef _IMPL_ARCHIVE_
	return state == ArchiveState::FAS_OPEN;
	#else
	return false;
	#endif
}

[[gnu::destructor]] void Makai::File::detachArchive() {
	#ifdef _IMPL_ARCHIVE_
	DEBUGLN("Detaching archive...");
	arc.close();
	state = ArchiveState::FAS_CLOSED;
	DEBUGLN("Archive detached!");
	#endif
}

#ifdef _IMPL_ARCHIVE_
void assertArchive(String const& path) {
	if (arcfail) Error::rethrow(arcfail);
	if (!Makai::File::isArchiveAttached())
		fileLoadError(path, "Archive is not attached!");
}

[[noreturn]] void fileGetError(String const& path, String const& fe, String const& ae) {
	fileLoadError(
		path,
		toString(
			"\nMultiple possibilities!\n\n",
			"[[ FOLDER ]]\n", fe, "\n",
			"[[ ARCHIVE ]]\n", ae, "\n"
		)
	);
}
#endif

inline void assertFileExists(String const& path) {
	if (!FileSystem::exists(path))
		fileLoadError(path, toString("File or directory '", path, "' does not exist!"));
}

#define IFSTREAM_EXCEPTIONS std::ifstream::failbit /*| std::ifstream::badbit*/
#define OFSTREAM_EXCEPTIONS std::ifstream::failbit /*| std::ofstream::badbit*/

String Makai::File::loadText(String const& path) {
	// Ensure directory exists
	assertFileExists(path);
	// The file
	std::ifstream file;
	try {
		// Ensure ifstream object can throw exceptions (nevermind, then)
		#ifndef MAKAILIB_FILE_GET_NO_EXCEPTIONS
		file.exceptions(IFSTREAM_EXCEPTIONS);	// This line errors
		#endif // MAKAILIB_FILE_GET_NO_EXCEPTIONS
		// The file and its contents
		String content;
		// Open file
		file.open(path);
		std::stringstream stream;
		// Read file’s buffer contents into stringstream
		stream << file.rdbuf();
		// Close file handler
		file.close();
		// Convert stream into string
		content = stream.str();
		// Return contents
		return content;
	} catch (std::exception const& e) {
		fileLoadError(path, e.what());
	}
	// Return contents
	return "";
}

BinaryData Makai::File::loadBinary(String const& path) {
	// Ensure directory exists
	assertFileExists(path);
	// The file
	std::ifstream file;
	// Try and load binary
	try {
		// Ensure ifstream object can throw exceptions (nevermind, then)
		#ifndef MAKAILIB_FILE_GET_NO_EXCEPTIONS
		file.exceptions(IFSTREAM_EXCEPTIONS);	// This line errors
		#endif // MAKAILIB_FILE_GET_NO_EXCEPTIONS
		// Preallocate data
		size_t fileSize = std::filesystem::file_size(path);
		BinaryData data(fileSize);
		// Open and read file
		file.open(path, std::ios::binary);
		file.read((char*)&data[0], fileSize);
		file.close();
		return data;
	} catch (std::exception const& e) {
		fileLoadError(path, e.what());
	}
	return BinaryData();
}

Makai::File::CSVData Makai::File::loadCSV(String const& path, char const& delimiter) {
	// The file and its contents
	String content = Makai::File::loadText(path);
	// Get values
	Makai::File::CSVData csvs;
	std::istringstream cData(content);
	String s;
	while (std::getline(cData, s, delimiter))
		// Remove invalid lines
		if(!s.empty())
			csvs.push_back(s);
	// Return contents
	return csvs;
}

void Makai::File::saveBinary(String const& path, Makai::File::ByteSpan const& data) {
	FileSystem::makeDirectory(FileSystem::getDirectoryFromPath(path));
	// Try and save data
	try {
		std::ofstream file(path.c_str(), std::ios::binary);
		// Ensure ofstream object can throw exceptions
		#ifndef MAKAILIB_FILE_GET_NO_EXCEPTIONS
		file.exceptions(OFSTREAM_EXCEPTIONS);
		#endif // MAKAILIB_FILE_GET_NO_EXCEPTIONS
		// Write data to file
		file.write((char*)data.data(), data.size());
		file.flush();
		file.close();
	} catch (std::exception const& e) {
		fileSaveError(path, e.what());
	}
}

void Makai::File::saveBinary(String const& path, BinaryData const& data) {
	Makai::File::saveBinary(path, Makai::File::ByteSpan((ubyte*)data.data(), data.size()));
}

void Makai::File::saveText(String const& path, String const& text) {
	FileSystem::makeDirectory(FileSystem::getDirectoryFromPath(path));
	// Try and save data
	try {
		std::ofstream file(path.c_str(), std::ios::trunc);
		// Ensure ofstream object can throw exceptions
		#ifndef MAKAILIB_FILE_GET_NO_EXCEPTIONS
		file.exceptions(OFSTREAM_EXCEPTIONS);
		#endif // MAKAILIB_FILE_GET_NO_EXCEPTIONS
		// Write data to file
		file.write(text.data(), text.size());
		file.flush();
		file.close();
	} catch (std::exception const& e) {
		fileSaveError(path, e.what());
	}
}

String Makai::File::loadTextFromArchive(String const& path) {
	#ifdef _IMPL_ARCHIVE_
	assertArchive(path);
	return arc.getTextFile(FileSystem::getChildPath(path));
	#else
	fileLoadError(path, "Archive functionality disabled!");
	#endif
}

BinaryData Makai::File::loadBinaryFromArchive(String const& path) {
	#ifdef _IMPL_ARCHIVE_
	assertArchive(path);
	return arc.getBinaryFile(FileSystem::getChildPath(path));
	#else
	fileLoadError(path, "Archive functionality disabled!");
	#endif
}

Makai::File::CSVData Makai::File::loadCSVFromArchive(String const& path, char const& delimiter) {
	#ifdef _IMPL_ARCHIVE_
	assertArchive(path);
	return Helper::splitString(arc.getTextFile(FileSystem::getChildPath(path)), delimiter);
	#else
	fileLoadError(path, "Archive functionality disabled!");
	#endif
}

String Makai::File::getText(String const& path) {
	#ifdef _IMPL_ARCHIVE_
	String res;
	if (isArchiveAttached())
		try {
			DEBUGLN("[ARC] Loading text file...");
			res = Makai::File::loadTextFromArchive(path);
		} catch (FileLoadError const& ae) {
			try {
				DEBUGLN("[FLD-2] Loading text file...");
				res = Makai::File::loadText(path);
			} catch (FileLoadError const& fe) {
				fileGetError(path, fe.summary(), ae.summary());
			}
		}
	else try {
		DEBUGLN("[FLD-1] Loading text file...");
		res = Makai::File::loadText(path);
	} catch (FileLoadError const& e) {
		fileGetError(path, e.summary(), "Archive not attached!");
	}
	return res;
	#else
	return Makai::File::loadText(path);
	#endif
}

BinaryData Makai::File::getBinary(String const& path) {
	#ifdef _IMPL_ARCHIVE_
	BinaryData res;
	if (isArchiveAttached())
		try {
			DEBUGLN("[ARC] Loading binary file...");
			res = Makai::File::loadBinaryFromArchive(path);
		} catch (FileLoadError const& ae) {
			try {
				DEBUGLN("[FLD-2] Loading binary file...");
				res = Makai::File::loadBinary(path);
			} catch (FileLoadError const& fe) {
				fileGetError(path, fe.summary(), ae.summary());
			}
		}
	else try {
		DEBUGLN("[FLD-1] Loading binary file...");
		res = Makai::File::loadBinary(path);
	} catch (FileLoadError const& e) {
		fileGetError(path, e.summary(), "Archive not attached!");
	}
	return res;
	#else
	return Makai::File::loadBinary(path);
	#endif
}

Makai::File::CSVData Makai::File::getCSV(String const& path, char const& delimiter) {
	#ifdef _IMPL_ARCHIVE_
	CSVData res;
	if (isArchiveAttached())
		try {
			DEBUGLN("[ARC] Loading CSV file...");
			res = Makai::File::loadCSVFromArchive(path);
		} catch (FileLoadError const& ae) {
			try {
				DEBUGLN("[FLD-2] Loading CSV file...");
				res = Makai::File::loadCSV(path);
			} catch (FileLoadError const& fe) {
				fileGetError(path, fe.summary(), ae.summary());
			}
		}
	else try {
		DEBUGLN("[FLD-1] Loading CSV file...");
		res = Makai::File::loadCSV(path);
	} catch (FileLoadError const& e) {
		fileGetError(path, e.summary(), "Archive not attached!");
	}
	return res;
	#else
	return Makai::File::loadCSV(path);
	#endif
}

#pragma GCC diagnostic pop
