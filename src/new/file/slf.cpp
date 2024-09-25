#include "slf.hpp"

namespace File = Makai::File;
using namespace Makai::SLF;
using namespace File;

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

SLFData Makai::SLF::parse(String const& slf, String const& srcFolder, bool const& pathOnly) {
	DEBUGLN("Parsing SLF file...");
	String content = slf;
	// Remove comments and empty lines
	content = regexReplace(content, "(:[<]([\\s\\S]*?)[>]:)|(::([\\s\\S]*?)(\\n|\\r|\\r\\n))", "");
	content = regexReplace(content, "((\\n|\\r|\\r\\n)+)", "|");
	// Get file location
	String dir = FileSystem::getDirectoryFromPath(srcFolder);
	DEBUGLN("Directory: ", dir);
	// Initialize type specifier here
	ShaderType type = ShaderType::ST_INVALID;
	// Remove specifier for processing
	content = regexReplace(content, "^[<](.*)[>]", "");
	// Process file
	SLFData result{dir};
	for (String shader: Helper::splitString(content, '|')) {
		DEBUGLN("Line: ", shader);
		// If line is a type specifier, try and get it
		String tt = regexFindFirst(shader, "^[<](.*)[>]");
		if (!tt.empty()) {
			type = fromFileExtension(
				regexReplace(tt, "<|>", "")
			);
			continue;
		}
		// If type is a valid shader type, use it instead of deducing
		if (isValidShaderType(type))
			result.shaders.push_back(ShaderEntry{shader, type});
		// Else, try and deduce it from shader file extension
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
					::toString("File extension is '", FileSystem::getFileExtension(shader), "'")
				);
			}
			result.shaders.push_back(ShaderEntry{shader, st});
		}
		if (!pathOnly)
			result.shaders.back().code = Makai::File::getText(result.shaders.back().path);
	}
	// Return result
	return result;
}

SLFData Makai::SLF::loadFile(String const& path, bool const& pathOnly) {
	// Try and get the file
	return Makai::SLF::parse(Makai::File::loadText(path), path);
}

SLFData Makai::SLF::getFile(String const& path, bool const& pathOnly) {
	// Try and get the file
	return Makai::SLF::parse(Makai::File::getText(path), path);
}
