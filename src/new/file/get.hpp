#ifndef MAKAILIB_FILE_GET_H
#define MAKAILIB_FILE_GET_H

#include "../ctl/ctl.hpp"
#include "json.hpp"

namespace Makai::File {
	// Until this puzzle is figured, this shall do
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wreturn-type"

	using BinaryData	= FileLoader::BinaryData;
	using CSVData		= FileLoader::CSVData;

	void attachArchive(String const& path, String const& password = "");

	bool isArchiveAttached();

	[[gnu::destructor]] void detachArchive();

	String loadTextFileFromArchive(String const& path);

	BinaryData loadBinaryFileFromArchive(String const& path);

	CSVData loadCSVFileFromArchive(String const& path, char const& delimiter = ',');

	JSON::JSONData loadJSONFileFromArchive(String const& path);

	String getTextFile(String const& path);

	BinaryData getBinaryFile(String const& path);

	CSVData getCSVFile(String const& path, char const& delimiter = ',');

	JSON::JSONData getJSONFile(String const& path);

	#pragma GCC diagnostic pop
}

#endif // MAKAILIB_FILE_GET_H
