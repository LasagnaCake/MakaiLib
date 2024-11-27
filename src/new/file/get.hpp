#ifndef MAKAILIB_FILE_GET_H
#define MAKAILIB_FILE_GET_H

#include "../compat/ctl.hpp"

namespace Makai::File {
	// Until this puzzle is figured, this shall do
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wreturn-type"

	DEFINE_ERROR_TYPE(FileLoadError);
	DEFINE_ERROR_TYPE(FileSaveError);

	using CSVData		= StringList;

	void attachArchive(DataBuffer& buffer, String const& password = "");
	void attachArchive(String const& path, String const& password = "");

	bool isArchiveAttached();

	[[gnu::destructor]] void detachArchive();

	String loadText(String const& path);
	void saveText(String const& path, String const& text);

	BinaryData<> loadBinary(String const& path);
	void saveBinary(String const& path, ByteSpan<> const& data);
	void saveBinary(String const& path, BinaryData<> const& data);

	template<Makai::Type::NonVoid T>
	inline void saveBinary(String const& path, T* const data, usize const count) {
		saveBinary(path, ByteSpan((ubyte*)data, count * sizeof(T)));
	}

	template<Makai::Type::NonVoid T>
	inline void saveBinary(String const& path, Span<T> const& data) {
		saveBinary(path, data.data(), data.size());
	}

	template<Makai::Type::NonVoid T>
	inline void saveBinary(String const& path, List<T> const& data) {
		saveBinary(path, data.data(), data.size());
	}

	template<Makai::Type::NonVoid T>
	inline void saveBinary(String const& path, T const& data) {
		saveBinary(path, ByteSpan(&data, sizeof(T)));
	}

	CSVData loadCSV(String const& path, char const delimiter = ',');
	//CSVData saveCSV(String const& path, CSVData const& data, char const delimiter = ',');

	String loadTextFromArchive(String const& path);

	BinaryData<> loadBinaryFromArchive(String const& path);

	CSVData loadCSVFromArchive(String const& path, char const delimiter = ',');

	String getText(String const& path);

	BinaryData<> getBinary(String const& path);

	CSVData getCSV(String const& path, char const delimiter = ',');

	#pragma GCC diagnostic pop
}

#endif // MAKAILIB_FILE_GET_H
