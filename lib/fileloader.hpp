#ifndef MAKAI_FILE_LOADER
#define MAKAI_FILE_LOADER

#include "../anchors.hpp"

namespace FileLoader {
	namespace {
		using
		std::string,
		std::runtime_error,
		std::ifstream,
		std::stringstream;
	}

	/// Loads a text file as a string.
	string loadTextFile(string path) {
		// The file and its contents
		string content;
		ifstream file;
		// ensure ifstream object can throw exception:
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
			throw runtime_error(string("File could not be opened: ") + path);
		}
		// Return contents
		return content;
	}
};

#endif // MAKAI_FILE_LOADER
