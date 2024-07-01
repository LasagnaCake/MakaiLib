#ifndef SHADER_LAYOUT_FORMAT_FILE_LOADER_H
#define SHADER_LAYOUT_FORMAT_FILE_LOADER_H

#include <string>
#include <vector>
#include <array>
#include <functional>
#include <cstring>
#include <stdexcept>
#include <cstdlib>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>

#include "../fileloader.hpp"

namespace SLF {
	/**
	* Loads a SLF (Shader Layout Format) file as a list of strings.
	*/
	namespace {
		using namespace std;
	}

	enum class ShaderType {
		ST_INVALID = -1,
		ST_FRAGMENT,
		ST_VERTEX,
		ST_COMPUTE,
		ST_GEOMETRY,
		ST_TESS_CTRL,
		ST_TESS_EVAL
	};

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

	struct ShaderEntry {
		String		path;
		ShaderType	type;
	};

	struct SLFData {
		String				folder;
		List<ShaderEntry>	shaders;
	};

	SLFData parseFile(String const& path) {
		// Try and get the file
		String content = FileLoader::getTextFile(path);
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
}

#endif // SHADER_LAYOUT_FORMAT_FILE_LOADER_H
