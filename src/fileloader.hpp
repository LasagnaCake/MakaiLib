#ifndef MAKAI_FILE_LOADER
#define MAKAI_FILE_LOADER

#include "anchors.hpp"

namespace FileLoader {
	namespace {
		using namespace std;
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

	typedef vector<string> CSVData;

	/**
	* Loads a CSV file as a list of strings.
	*/
	CSVData loadCSVFile(string path, char delimiter = ',') {
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
			return CSVData();
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
};

#endif // MAKAI_FILE_LOADER
