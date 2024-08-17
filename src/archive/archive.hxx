module;

#include <nlohmann/json.hpp>
#include <cryptopp/aes.h>
#include <cryptopp/zlib.h>
#include <cryptopp/modes.h>
#include <cryptopp/sha3.h>
#include <cppcodec/base64_rfc4648.hpp>
#include <cppcodec/base32_rfc4648.hpp>
#include <filesystem>
#include <algorithm>

export module Makai;

export namespace Makai::Tool::Arch {
	using JSON = nlohmann::json;
	typedef nlohmann::ordered_json JSONData;

	enum class EncryptionMethod: uint64 {
		AEM_NONE,
		AEM_AES256,
	};

	enum class CompressionMethod: uint64 {
		ACM_NONE,
		ACM_ZIP,
	};

	String encoded(uint64 const& v);
	uint64 decoded(String const& v);

	template<class T>
	String hash(String const& str);

	constexpr String truncate(String const& str);

	String hashPassword(String const& str);

	template<class T>
	BinaryData cbcTransform(
		BinaryData const&	data,
		String				password	= "",
		uint8* const&		block		= nullptr
	);

	template<class T>
	T* getFlator(String& result, uint8 const& level);

	template<Type::Equal<Inflator> T>
	T* getFlator(String& result, uint8 const& level);

	BinaryData encrypt(
		BinaryData const&		data,
		String const&			password	= "",
		EncryptionMethod const&	method		= EncryptionMethod::AEM_AES256,
		uint8* const&			block		= nullptr
	);

	BinaryData decrypt(
		BinaryData const&		data,
		String const&			password	= "",
		EncryptionMethod const&	method		= EncryptionMethod::AEM_AES256,
		uint8* const&			block		= nullptr
	);

	BinaryData compress(
		BinaryData	const&			data,
		CompressionMethod const&	method	= CompressionMethod::ACM_ZIP,
		uint8 const&				level	= 9
	);

	BinaryData decompress(
		BinaryData	const&			data,
		CompressionMethod const&	method	= CompressionMethod::ACM_ZIP,
		uint8 const&				level	= 9
	);

	JSONData getStructure(fs::path const& path, StringList& files, String const& root = "");

	StringList getFileInfo(JSONData const& filestruct);

	usize populateTree(JSONData& tree, List<uint64> const& values, usize const& start = 0);

	void populateTree(JSONData& tree, String const& root = "");

	struct FileHeader {
		uint64	uncSize;
		uint64	compSize;
		uint32	crc			= 0;
		uint8	block[16]	= {0};
		// Put new things BELOW this line
	};

	struct DirectoryHeader {
		uint64	uncSize;
		uint64	compSize;
		uint32	crc			= 0;
		uint8	block[16]	= {0};
	};

	constexpr uint64 ARCHIVE_VERSION		= 1;
	constexpr uint64 ARCHIVE_MIN_VERSION	= 1;

	#pragma pack(1)
	struct ArchiveHeader {
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
		constexpr uint64 SHOULD_CHECK_HASH_BIT		= (1 << 1);
	}

	void generateBlock(uint8 const(& block)[16]);

	void pack(
			String const& archivePath,
			String const& folderPath,
			String const& password = "",
			EncryptionMethod const& enc = EncryptionMethod::AEM_AES256,
			CompressionMethod const& comp = CompressionMethod::ACM_ZIP,
			uint8 const& complvl = 9
	);

	struct FileArchive {
		struct FileEntry {
			uint64 const	index;
			String const	path;
			FileHeader		header;
			BinaryData		data;
		};

		struct ArchiveVersion {
			uint64 const version;
			uint64 const minimum;
		};

		FileArchive() {}

		FileArchive(String const& path, String const& password = "");

		~FileArchive();

		FileArchive& open(String const& path, String const& password);

		FileArchive& close();

		String getTextFile(String const& path);

		BinaryData getBinaryFile(String const& path);

		JSONData getFileTree(String const& root = "") const;

		static ArchiveHeader getHeader(String const& path);

		FileArchive& unpackTo(String const& path);

		bool isOpen() const {return streamOpen;}

	private:

		void parseFileTree();

		void demangleData(BinaryData& data, uint8* const& block) const;

		void unpackLayer(JSONData& layer, String const& path);

		void processFileEntry(FileEntry& entry);

		FileEntry getFileEntry(String const& path);

		BinaryData getFileEntryData(uint64 const& index, FileHeader const& fh);

		FileHeader getFileEntryHeader(uint64 const& index);

		uint64 getFileEntryLocation(String const& path, String const& origpath);

		void assertOpen();

		bool			streamOpen	= false;
		String			pass;
		std::ifstream	archive;
		ArchiveHeader	header;
		JSONData		fstruct;
	};

	void unpackV1(
		String const&	archivePath,
		String const	folderPath,
		String const&	password = ""
	);

	void unpackV0(
		String const&	archivePath,
		String const	folderPath,
		String const&	password = ""
	);

	void unpack(
		String const& archivePath,
		String const folderPath,
		String const& password = ""
	);

	String loadEncryptedTextFile(String const& path, String const& password = "") {
		BinaryData fd = loadEncryptedBinaryFile(path, password);
		return String(fd.begin(), fd.end());
	}

	template<typename T>
	void saveEncryptedBinaryFile(
		String const&				path,
		T* const&					data,
		size_t const&				size,
		String const&				password	= "",
		EncryptionMethod const&		enc			= EncryptionMethod::AEM_AES256,
		CompressionMethod const&	comp		= CompressionMethod::ACM_ZIP,
		uint8 const&				lvl			= 9
	);

	void saveEncryptedTextFile(
		String const&				path,
		BinaryData const&			data,
		String const&				password	= "",
		EncryptionMethod const&		enc			= EncryptionMethod::AEM_AES256,
		CompressionMethod const&	comp		= CompressionMethod::ACM_ZIP,
		uint8 const&				lvl			= 9
	);

	template<typename T>
	void saveEncryptedBinaryFile(
		String const&				path,
		List<T> const&				data,
		String const&				password	= "",
		EncryptionMethod const&		enc			= EncryptionMethod::AEM_AES256,
		CompressionMethod const&	comp		= CompressionMethod::ACM_ZIP,
		uint8 const&				lvl			= 9
	);

	void saveEncryptedTextFile(
		String const&				path,
		String const&				data,
		String const&				password	= "",
		EncryptionMethod const&		enc			= EncryptionMethod::AEM_AES256,
		CompressionMethod const&	comp		= CompressionMethod::ACM_ZIP,
		uint8 const&				lvl			= 9
	);
}

export namespace MkTool = Makai::Tool;
