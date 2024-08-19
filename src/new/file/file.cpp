#include "file.hpp"

#if !(defined(_DEBUG_OUTPUT_) || defined(_ARCHIVE_SYSTEM_DISABLED_)) || defined(_TESTING_ARCHIVE_SYS_)
#define _IMPL_ARCHIVE_
#endif

using Makai::Tool::Arch::FileArchive;

using namespace FileLoader;

#include "../tool/archive.hpp"

#ifdef _IMPL_ARCHIVE_
bool				loadingArchive	= false;
FileArchive 		arc;
Error::ErrorPointer	arcfail			= nullptr;
#endif

// Until this puzzle is figured, this shall do
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"

/*#undef _ARCHIVE_SYSTEM_DISABLED_
#define _ARCHIVE_SYSTEM_DISABLED_*/

inline void Makai::File::attachArchive(String const& path, String const& password = "") {
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

inline bool Makai::File::isArchiveAttached() {
	#ifdef _IMPL_ARCHIVE_
	return arc.isOpen();
	#else
	return false;
	#endif
}

[[gnu::destructor]] inline void Makai::File::detachArchive() {
	#ifdef _IMPL_ARCHIVE_
	DEBUGLN("Detaching archive...");
	arc.close();
	DEBUGLN("Archive detached!");
	#endif
}

#ifdef _IMPL_ARCHIVE_
inline void assertArchive(String const& path) {
	if (arcfail) Error::rethrow(arcfail);
	if (!isArchiveAttached())
		fileLoadError(path, "Archive is not attached!", "fileloader.hpp");
}

[[noreturn]] inline void fileGetError(String const& path, String const& fe, String const& ae) {
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

inline String Makai::File::loadTextFileFromArchive(String const& path) {
	#ifdef _IMPL_ARCHIVE_
	assertArchive(path);
	return arc.getTextFile(FileSystem::getChildPath(path));
	#else
	fileLoadError(path, "Archive functionality disabled!");
	#endif
}

inline BinaryData Makai::File::loadBinaryFileFromArchive(String const& path) {
	#ifdef _IMPL_ARCHIVE_
	assertArchive(path);
	return arc.getBinaryFile(FileSystem::getChildPath(path));
	#else
	fileLoadError(path, "Archive functionality disabled!");
	#endif
}

inline CSVData Makai::File::loadCSVFileFromArchive(String const& path, char const& delimiter = ',') {
	#ifdef _IMPL_ARCHIVE_
	assertArchive(path);
	return Helper::splitString(arc.getTextFile(FileSystem::getChildPath(path)), delimiter);
	#else
	fileLoadError(path, "Archive functionality disabled!");
	#endif
}

inline Makai::JSON::JSONData Makai::File::loadJSONFileFromArchive(String const& path) {
	#ifdef _IMPL_ARCHIVE_
	assertArchive(path);
	return Makai::JSON::parseJSON(arc.getTextFile(FileSystem::getChildPath(path)));
	#else
	fileLoadError(path, "Archive functionality disabled!");
	#endif
}

inline String Makai::File::getTextFile(String const& path) {
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

inline BinaryData Makai::File::getBinaryFile(String const& path) {
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

inline CSVData Makai::File::getCSVFile(String const& path, char const& delimiter = ',') {
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

inline Makai::JSON::JSONData Makai::File::getJSONFile(String const& path) {
	#ifdef _IMPL_ARCHIVE_
	Makai::JSON::JSONData res;
	if (isArchiveAttached())
		try {
			DEBUGLN("[ARC] Loading JSON file...");
			res = Makai::File::loadJSONFromArchive(path);
		} catch (FileLoadError const& ae) {
			try {
				DEBUGLN("[FLD-2] Loading JSON file...");
				res = Makai::JSON::loadJSON(path);
			} catch (FileLoadError const& fe) {
				fileGetError(path, fe.summary(), ae.summary());
			}
		}
	else try {
		DEBUGLN("[FLD-1] Loading JSON file...");
		res = Makai::JSON::loadJSON(path);
	} catch (FileLoadError const& e) {
		fileGetError(path, e.summary(), "Archive not attached!");
	}
	return res;
	#else
	return loadJSON(path);
	#endif
}

#pragma GCC diagnostic pop

namespace File = Makai::File;
using namespace File;
using namespace SLF;

constexpr String toFileExtension(ShaderType const& type) {
	switch (type) {
		default:
		case ShaderType::ST_INVALID:	return "INVALID";
		case ShaderType::ST_FRAGMENT:	return "vert";
		case ShaderType::ST_VERTEX:		return "frag";
		case ShaderType::ST_COMPUTE:	return "comp";
		case ShaderType::ST_GEOMETRY:	return "geom";
		case ShaderType::ST_TESS_CTRL:	return "tsct";
		case ShaderType::ST_TESS_EVAL:	return "tsev";
	}
	return "INVALID";
}

constexpr ShaderType fromFileExtension(String const& str) {
	if (str == "frag") return ShaderType::ST_FRAGMENT;
	if (str == "vert") return ShaderType::ST_VERTEX;
	if (str == "comp") return ShaderType::ST_COMPUTE;
	if (str == "geom") return ShaderType::ST_GEOMETRY;
	if (str == "tsct") return ShaderType::ST_TESS_CTRL;
	if (str == "tsev") return ShaderType::ST_TESS_EVAL;
	return ShaderType::ST_INVALID;
}

constexpr ShaderType fromFilePath(String const& path) {
	return fromFileExtension(
		FileSystem::getFileExtension(
			Helper::toLower(path)
		)
	);
}

constexpr bool isValidShaderType(ShaderType const& type) {
	return type != ShaderType::ST_INVALID;
}

constexpr bool isValidShaderExtension(String const& path) {
	return isValidShaderType(fromFilePath(path));
}

SLFData File::SLF::getSLFFile(String const& path) {
	// Try and get the file
	String content = File::getTextFile(path);
	// Remove comments and empty lines
	content = regexReplace(content, "(:[<]([\\s\\S]*?)[>]:)|(::([\\s\\S]*?)(\\n|\\r|\\r\\n))", "");
	content = regexReplace(content, "((\\n|\\r|\\r\\n)+)", "|");
	// Get file location
	String dir = FileSystem::getDirectoryFromPath(path);
	// Get file specifier, if any
	ShaderType type = fromFileExtension(
		regexReplace(
			regexFindFirst(content, "^[<](.*)[>]"),
			"<|>",
			""
		)
	);
	// Remove specifier for processing
	content = regexReplace(content, "^[<](.*)[>]", "");
	// Process file
	SLFData result{dir};
	for (String shader: Helper::splitString(content, '|')) {
		if (isValidShaderType(type))
			result.shaders.push_back(ShaderEntry{shader, type});
		else {
			ShaderType st = fromFilePath(shader);
			if (!isValidShaderType(st)) {
				throw Error::InvalidValue(
					::toString(
						"Invalid shader type for shader'",
						FileSystem::concatenatePath(dir, shader),
						"'!"
					),
					__FILE__,
					::toString(__LINE__),
					"SLF::parseFile",
					::toString("File extension is '", FileSystem::getFileExtension(path), "'")
				);
			}
			result.shaders.push_back(ShaderEntry{shader, st});
		}
	}
	// Return result
	return result;
}
