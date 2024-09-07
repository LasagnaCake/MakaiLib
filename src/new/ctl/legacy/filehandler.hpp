#ifndef SYSTEM_FILE_LOADER_H
#define SYSTEM_FILE_LOADER_H

#include "errors.hpp"
#include "helper.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>

namespace FileLoader {
	DEFINE_ERROR_TYPE(FileLoadError);
	DEFINE_ERROR_TYPE(FileSaveError);

	[[noreturn]] inline void fileLoadError(String const& path, String const& reason, String const& file = "filehandler.hpp") {
		throw FileLoadError(
			"Could not load file '" + path + "'!",
			file,
			"unspecified",
			"unspecified",
			reason
		);
	}

	[[noreturn]] inline void fileSaveError(String const& path, String const& reason, String const& file = "filehandler.hpp") {
		throw FileLoadError(
			"Could not save file '" + path + "'!",
			file,
			"unspecified",
			"unspecified",
			reason
		);
	}
	namespace {
		using namespace std;

		inline void assertFileExists(String const& path) {
			if (!FileSystem::exists(path))
				fileLoadError(path, toString("File or directory '", path, "' does not exist!"));
		}
	}

	typedef List<uint8> BinaryData;

	/// Loads a binary file as an array;
	inline BinaryData loadBinaryFile(String const& path) {
		// Ensure directory exists
		assertFileExists(path);
		// Try and load binary
		try {
			// The file
			ifstream file;
			// Ensure ifstream object can throw exceptions (nevermind, then)
			// file.exceptions(ifstream::failbit | ifstream::badbit);	// This line errors
			// Preallocate data
			size_t fileSize = filesystem::file_size(path);
			BinaryData data(fileSize);
			// Open and read file
			file.open(path, ios::binary);
			file.read((char*)&data[0], fileSize);
			file.close();
			return data;
		} catch (std::exception const& e) {
			fileLoadError(path, e.what());
		}
		return BinaryData();
	}

	/// Loads a text file as a string.
	inline String loadTextFile(String const& path) {
		// Ensure directory exists
		assertFileExists(path);
		try {
			// The file and its contents
			String content;
			ifstream file;
			// Ensure ifstream object can throw exceptions (nevermind, then)
			// file.exceptions(ifstream::failbit | ifstream::badbit);	// This line errors
			// Open file
			file.open(path);
			stringstream stream;
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

	typedef StringList CSVData;

	/**
	* Loads a CSV file as a list of strings.
	*/
	inline CSVData loadCSVFile(String const& path, char delimiter = ',') {
		// The file and its contents
		String content = loadTextFile(path);
		// Get values
		CSVData csvs;
		istringstream cData(content);
		String s;
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
	inline void saveBinaryFile(String const& path, T* data, size_t size) {
		FileSystem::makeDirectory(FileSystem::getDirectoryFromPath(path));
		// Try and save data
		try {
			ofstream file(path.c_str(), ios::binary);
			// Ensure ifstream object can throw exceptions
			file.exceptions(ofstream::failbit | ofstream::badbit);
			// Write data to file
			file.write((char*)data, size * sizeof(T));
			file.flush();
			file.close();
		} catch (std::exception const& e) {
			fileSaveError(path, e.what());
		}
	}

	/// Saves an array of data as a binary file (Non-destructive).
	template <typename T>
	inline void saveBinaryFile(String const& path, List<T> data) {
		FileSystem::makeDirectory(FileSystem::getDirectoryFromPath(path));
		saveBinaryFile<T>(path, data.data(), data.size());
	}

	/// Saves an string as a text file (Non-destructive).
	inline void saveTextFile(String const& path, String const& text) {
		FileSystem::makeDirectory(FileSystem::getDirectoryFromPath(path));
		// Try and save data
		try {
			ofstream file(path.c_str(), ios::trunc);
			// Ensure ifstream object can throw exceptions
			file.exceptions(ofstream::failbit | ofstream::badbit);
			// Write data to file
			file.write(text.data(), text.size());
			file.flush();
			file.close();
		} catch (std::exception const& e) {
			fileSaveError(path, e.what());
		}
	}
};

#endif // SYSTEM_FILE_LOADER_H
