#ifndef MAKAILIB_FILE_GET_H
#define MAKAILIB_FILE_GET_H

#include "../ctl/ctl.hpp"
#include "../data/encdec.hpp"
#include "json.hpp"

namespace Makai::File {
	// Until this puzzle is figured, this shall do
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wreturn-type"

	using CSVData = FileLoader::CSVData;

	inline void attachArchive(String const& path, String const& password = "");

	inline bool isArchiveAttached();

	[[gnu::destructor]] inline void detachArchive();

	inline String loadTextFileFromArchive(String const& path);

	inline Data::BinaryData loadBinaryFileFromArchive(String const& path);

	inline CSVData loadCSVFileFromArchive(String const& path, char const& delimiter = ',');

	inline JSON::JSONData loadJSONFileFromArchive(String const& path);

	inline String getTextFile(String const& path);

	inline Data::BinaryData getBinaryFile(String const& path);

	inline CSVData getCSVFile(String const& path, char const& delimiter = ',');

	inline JSON::JSONData getJSONFile(String const& path);

	#pragma GCC diagnostic pop
}

#endif // MAKAILIB_FILE_GET_H
