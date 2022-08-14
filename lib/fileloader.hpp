#ifndef MAKAI_FILE_LOADER
#define MAKAI_FILE_LOADER

#include "../anchors.hpp"

namespace FileLoader {
	namespace {
		using
		std::string,
		std::runtime_error,
		std::ifstream,
		std::stringstream,
		Vector::VecV2;
	}

	struct Image {
		VecV2 size;
		string data;
		unsigned int channels;
	};

	Image loadImageFile(string path) {
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
};

#endif // MAKAI_FILE_LOADER
