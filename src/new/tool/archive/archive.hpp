#ifndef MAKAILIB_TOOL_ARCHIVE_H
#define MAKAILIB_TOOL_ARCHIVE_H

#include <fstream>
#include "../../compat/ctl.hpp"
#include "../../file/get.hpp"
#include "../../file/json.hpp"

namespace Makai::Tool::Arch {
	enum class EncryptionMethod: uint64 {
		AEM_NONE,
		AEM_AES256,
	};

	enum class CompressionMethod: uint64 {
		ACM_NONE,
		ACM_ZIP,
	};

	constexpr String truncate(String const& str);

	String hashPassword(String const& str);

	BinaryData<> encrypt(
		BinaryData<> const&		data,
		String const&			password	= "",
		EncryptionMethod const&	method		= EncryptionMethod::AEM_AES256,
		uint8* const&			block		= nullptr
	);

	BinaryData<> decrypt(
		BinaryData<> const&		data,
		String const&			password	= "",
		EncryptionMethod const&	method		= EncryptionMethod::AEM_AES256,
		uint8* const&			block		= nullptr
	);

	BinaryData<> compress(
		BinaryData<>	const&		data,
		CompressionMethod const&	method	= CompressionMethod::ACM_ZIP,
		uint8 const				level	= 9
	);

	BinaryData<> decompress(
		BinaryData<>	const&		data,
		CompressionMethod const&	method	= CompressionMethod::ACM_ZIP,
		uint8 const				level	= 9
	);

	struct [[gnu::packed]] FileHeader {
		uint64	uncSize;
		uint64	compSize;
		uint32	crc			= 0;
		uint8	block[16]	= {0};
		// Put new things BELOW this line
	};

	struct [[gnu::packed]] DirectoryHeader {
		uint64	uncSize;
		uint64	compSize;
		uint32	crc			= 0;
		uint8	block[16]	= {0};
	};
	
	constexpr uint64 ARCHIVE_VERSION		= 1;
	constexpr uint64 ARCHIVE_MIN_VERSION	= 1;

	struct [[gnu::packed]] ArchiveHeader {
		uint64	const headerSize		= sizeof(ArchiveHeader);
		uint64	const fileHeaderSize	= sizeof(FileHeader);
		uint64	const dirHeaderSize		= sizeof(DirectoryHeader);
		uint64	version			= ARCHIVE_VERSION;
		uint64	minVersion		= ARCHIVE_MIN_VERSION;
		uint16	encryption		= (uint16)EncryptionMethod::AEM_AES256;
		uint16	compression		= (uint16)CompressionMethod::ACM_ZIP;
		uint8	level			= 9;
		uint64	flags			= 0;
		uint64	dirHeaderLoc	= 0;
		// Put new things BELOW this line
	};

	namespace Flags {
		constexpr uint64 SINGLE_FILE_ARCHIVE_BIT	= (1 << 0);
		constexpr uint64 SHOULD_CHECK_CRC_BIT		= (1 << 1);
	}

	void pack(
			String const& archivePath,
			String const& folderPath,
			String const& password = "",
			EncryptionMethod const& enc = EncryptionMethod::AEM_AES256,
			CompressionMethod const& comp = CompressionMethod::ACM_ZIP,
			uint8 const complvl = 9
	);

	void unpack(
		String const& archivePath,
		String const folderPath,
		String const& password = ""
	);

	struct FileArchive {
		struct FileEntry {
			uint64 const	index;
			String const	path;
			FileHeader		header;
			BinaryData<>	data;
		};

		struct ArchiveVersion {
			uint64 const version;
			uint64 const minimum;
		};

		FileArchive() {}

		FileArchive(DataBuffer& buffer, String const& password = "");

		~FileArchive();

		FileArchive& open(DataBuffer& buffer, String const& password);

		FileArchive& close();

		String getTextFile(String const& path);

		BinaryData<> getBinaryFile(String const& path);

		JSON::JSONData getFileTree(String const& root = "") const;

		static ArchiveHeader getHeader(String const& path);

		FileArchive& unpackTo(String const& path);

		bool isOpen() const;

	private:

		void parseFileTree();

		void demangleData(BinaryData<>& data, uint8* const& block) const;

		void unpackLayer(JSON::Extern::JSONData const& layer, String const& path);

		void processFileEntry(FileEntry& entry) const;

		FileEntry getFileEntry(String const& path);

		BinaryData<> getFileEntryData(uint64 const index, FileHeader const& fh);

		FileHeader getFileEntryHeader(uint64 const index);

		uint64 getFileEntryLocation(String const& path, String const& origpath);

		void assertOpen() const;

		bool					streamOpen	= false;
		String					pass;
		DataStream				archive		= DataStream(nullptr);
		ArchiveHeader			header;
		JSON::Extern::JSONData	fstruct;
	};

	String loadEncryptedTextFile(String const& path, String const& password = "");

	BinaryData<> loadEncryptedBinaryFile(String const& path, String const& password = "");

	template<typename T>
	void saveEncryptedBinaryFile(
		String const&				path,
		T* const&					data,
		usize const				size,
		String const&				password	= "",
		EncryptionMethod const&		enc			= EncryptionMethod::AEM_AES256,
		CompressionMethod const&	comp		= CompressionMethod::ACM_ZIP,
		uint8 const				lvl			= 9
	);

	void saveEncryptedTextFile(
		String const&				path,
		BinaryData<> const&			data,
		String const&				password	= "",
		EncryptionMethod const&		enc			= EncryptionMethod::AEM_AES256,
		CompressionMethod const&	comp		= CompressionMethod::ACM_ZIP,
		uint8 const				lvl			= 9
	);

	template<typename T>
	void saveEncryptedBinaryFile(
		String const&				path,
		List<T> const&				data,
		String const&				password	= "",
		EncryptionMethod const&		enc			= EncryptionMethod::AEM_AES256,
		CompressionMethod const&	comp		= CompressionMethod::ACM_ZIP,
		uint8 const				lvl			= 9
	);

	void saveEncryptedTextFile(
		String const&				path,
		String const&				data,
		String const&				password	= "",
		EncryptionMethod const&		enc			= EncryptionMethod::AEM_AES256,
		CompressionMethod const&	comp		= CompressionMethod::ACM_ZIP,
		uint8 const				lvl			= 9
	);
}

#endif // MAKAILIB_TOOL_ARCHIVE_H
