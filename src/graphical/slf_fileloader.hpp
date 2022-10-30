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

namespace SLF {
	/**
	* Loads a SLF (Shader Layout Format) file as a list of strings.
	*/
	namespace {
		using namespace std;
	}

	constexpr array<char[5], 6> validShaders = {
		"frag",
		"vert",
		"comp",
		"geom",
		"tsct",
		"tsev"
	};

	typedef vector<string> SLFData;

	SLFData parseFile(string path) {
		// The file and its contents
		string content;
		ifstream file;
		// Ensure ifstream object can throw exception:
		file.exceptions(ifstream::failbit | ifstream::badbit);
		try {
			// Open file
			file.open(path);
			stringstream stream;
			// Read file’s buffer contents into streams
			stream << file.rdbuf();
			// Close file handler
			file.close();
			// Convert stream into string
			content = stream.str();
		}
		catch (ifstream::failure e) {
			throw runtime_error(string("Could not load file '") + path + "'!\n\n" + e.what());
		}
		// Remove comments and empty lines
		content = regex_replace(
			content,
			regex("(:[<]([\\s\\S]*?)[>]:)|(::([\\s\\S]*?)(\\n|\\r|\\r\\n))"),
			""
		);
		content = regex_replace(
			content,
			regex("($|(\\n|\\r|\\r\\n)+)"),
			"|"
		);
		// Get file location
		smatch dir;
		regex_search(
			path,
			dir,
			regex("^(.*(\\/|\\\\))")
		);
		// Get file specifier, if any
		smatch type;
		string listType;
		regex_search(
			content,
			type,
			regex("^[<](.*)[>]")
		);
		listType = type[1];
		if (listType != ""){
			listType = regex_replace(
				listType,
				regex("<|>"),
				""
			);
		}
		else listType = "";
		// Remove specifier for processing
		content = regex_replace(
			content,
			regex("^[<](.*)[>]"),
			""
		);
		// Get values
		SLFData values;
		istringstream cData(content);
		string line;
		values.push_back(dir[1]);
		if (listType!= "")
			values.push_back(listType);
		while (getline(cData, line, '|')) {
			// Remove invalid lines
			if(line.size() > 3) {
				values.push_back(line);
				// Get shader type, if not specified
				if (listType == "") {
					bool valid = false;
					#pragma GCC unroll validShaders.size()
					for(auto vs : validShaders)
						if (line.find(vs) != string::npos) {
							values.push_back(vs);
							valid = true;
							break;
						}
					if (!valid)
						throw runtime_error(string("Invalid shader type for shader'") + values[-1] + "'!");
				}
			}
		}
		// Return contents
		return values;
	}
}

#endif // SHADER_LAYOUT_FORMAT_FILE_LOADER_H
