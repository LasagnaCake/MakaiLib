#ifndef MAKAI_FILE_LOADER
#define MAKAI_FILE_LOADER

#include "../anchors.hpp"

namespace FileLoader {
	namespace {
		using
		std::string,
		std::vector,
		std::runtime_error,
		std::ifstream,
		std::stringstream,
		std::istringstream,
		Vector::VecV2;
	}

	struct Image {
		VecV2 size;
		string data;
		unsigned int channels;
	};

	Image loadImageFile(string path, char delimiter = '|') {
		int width, height, nrChannels;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		Image res;
		if (data) {
			res.data = string((char*)data);
			res.size = VecV2(width, height);
			res.channels = nrChannels;
			stbi_image_free(data);
		}
		return res;
	}

	/// Loads a text file as a string.
	string loadTextFile(string path) {
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
			return nullptr;
		}
		// Return contents
		return content;
	}

	/**
	* Loads a CSV file as a list of strings.
	*/
	vector<string> loadCSVFile(string path, delimiter = ',') {
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
			return nullptr;
		}
		// Get values
		vector<string> csvs;
		istringstream f(content);
		string s;
		while (getline(f, s, delimiter)) {
			// Remove invalid lines
			if(s.size() > 3)
				strings.push_back(s);
		}
		// Return contents
		return csvs;
	}

	/**
	* Loads a SLF (Shader Layout Format) file as a list of strings.
	*/
	vector<string> loadSLFFile(string path) {
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
			return nullptr;
		}
		// Remove comments and empty lines
		content = regex_replace(
			content,
			regex("(:<([\s\S]*?)>:)|(::([\s\S]*?)(\n|\r|\r\n))"),
			""
		);
		content = regex_replace(
			content,
			regex("($(\n|\r|\r\n)+)|(\|+)"),
			"||"
		);
		// Get values
		vector<string> csvs;
		istringstream f(content);
		string s;
		while (getline(f, s, '||')) {
			// Remove invalid lines
			if(s.size() > 3)
				strings.push_back(s);
		}
		// Return contents
		return csvs;
	}
};

#endif // MAKAI_FILE_LOADER
