#ifndef MAKAI_FILE_LOADER
#define MAKAI_FILE_LOADER

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>

namespace FileLoader {
	namespace {
		using namespace std;
	}

	inline void fileLoadError(string path, string reason) {
		throw runtime_error("Could not load file '" + path + "'!\n\n[ERROR MESSAGE]\n" + reason);
	}

	typedef vector<unsigned char> BinaryData;

	/// Loads a binary file as an array;
	BinaryData loadBinaryFile(string path) {
		ifstream file;
		// Ensure ifstream object can throw exceptions
		file.exceptions(ifstream::failbit | ifstream::badbit);
		// Try and load binary
		try {
			// Preallocate data
			size_t fileSize = filesystem::file_size(path);
			BinaryData data(fileSize * 4);
			// Open and read file
			file.open(path, ios::binary);
			file.read((char*)&data[0], fileSize);
			file.close();
			return data;
		}
		catch (runtime_error e) {
			fileLoadError(path, e.what());
		}
		return BinaryData();
	}

	/// Loads a text file as a string.
	string loadTextFile(string path) {
		// The file and its contents
		string content;
		ifstream file;
		// Ensure ifstream object can throw exceptions
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
			fileLoadError(path, e.what());
		}
		// Return contents
		return content;
	}

	typedef vector<string> CSVData;

	/**
	* Loads a CSV file as a list of strings.
	*/
	CSVData loadCSVFile(string path, char delimiter = ',') {
		// The file and its contents
		string content;
		ifstream file;
		// Ensure ifstream object can throw exceptions
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
			fileLoadError(path, e.what());
		}
		// Get values
		CSVData csvs;
		istringstream cData(content);
		string s;
		while (getline(cData, s, delimiter)) {
			// Remove invalid lines
			if(s.size() > 3)
				csvs.push_back(s);
		}
		// Return contents
		return csvs;
	}

	/// Saves an array of data as a binary file (Non-destructive).
	template <typename T>
	void saveBinaryFile(string path, T* data, size_t size) {
		ofstream file(path.c_str() , ios::binary);
		// Ensure ifstream object can throw exceptions
		file.exceptions(ofstream::failbit | ofstream::badbit);
		// Try and save data
		file.write((char*)data, size * sizeof(T));
		file.flush();
		file.close();
	}
};

#define $fld FileLoader::

#endif // MAKAI_FILE_LOADER
