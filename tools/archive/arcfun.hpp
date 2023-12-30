#ifndef MAKAILIB_TOOL_ARCHIVE_FUNCTIONALITY_H
#define MAKAILIB_TOOL_ARCHIVE_FUNCTIONALITY_H

#include <collection/helper.hpp>
#include <collection/algebra.hpp>
#include <collection/filehandler.hpp>
#include <nlohmann/json.hpp>
#include <cryptopp/aes.h>
#include <cryptopp/zlib.h>
#include <cryptopp/modes.h>
#include <cppcodec/base64_rfc4648.hpp>
#include <cppcodec/base32_rfc4648.hpp>
#include <filesystem>

#ifdef ARCSYS_APLLICATION_
#define _ARCDEBUG(...)		DEBUG(__VA_ARGS__)
#define _ARCDEBUGLN(...)	DEBUGLN(__VA_ARGS__)
#define _ARCEXIT exit(-1)
#else
#define _ARCDEBUG(...)
#define _ARCDEBUGLN(...)
#define _ARCEXIT
#endif // ARCSYS_ARCHIVE_APLLICATION_

namespace ArcSys {
	using JSON = nlohmann::json;
	typedef nlohmann::ordered_json JSONData;

	namespace {
		namespace fs = std::filesystem;
		namespace FLD = FileLoader;
		using namespace CryptoPP;
		using FLD::BinaryData;
	}

	enum class EncryptionMethod: uint64 {
		AEM_NONE,
		AEM_AES256,
	};

	enum class CompressionMethod: uint64 {
		ACM_NONE,
		ACM_ZIP,
	};

	//cppcodec::base64_rfc4648::decode
	//cppcodec::base64_rfc4648::encode

	String encoded(uint64 const& v) {
		List<uint8> data(8, 0);
		for SSRANGE(i, 0, 8) data[i] = uint8((v >> (8 * i)) & 0xFF);
		return cppcodec::base64_rfc4648::encode(data);
	}

	uint64 decoded(String const& v) {
		List<uint8> data = cppcodec::base64_rfc4648::decode(v);
		size_t result = 0;
		for (auto [i, b]: Helper::enumerate(data))
			result |= (uint64(b) << (8 * i));
		return result;
	}

	template<class T>
	BinaryData cbcTransform(
		BinaryData const&		data,
		String					password	= ""
	) try {
		String result;
		T tf;
		uint8* iv = new uint8[16];
		memset(iv, 0, 16);
		while (password.size() < tf.MaxKeyLength())
			password += " ";
		if (password.size() > 32)
			password = password.substr(0, 32);
		tf.SetKeyWithIV((uint8*)password.data(), password.length(), iv, 16);
		StringSource ss(
			data.data(),
			data.size(),
			true,
			new StreamTransformationFilter(
				tf,
                new StringSink(result)
            )
        );
        delete[] iv;
		return BinaryData(result.begin(), result.end());
	} catch (Exception const& e) {
		throw Error::FailedAction(
			e.what()
		);
	}

	template<Type::Equal<Deflator> T>
	T* getFlator(String& result, uint8 const& level) {
		return new T(new StringSink(result), level);
	}

	template<Type::Equal<Inflator> T>
	T* getFlator(String& result, uint8 const& level) {
		return new T(new StringSink(result));
	}

	template<class T>
	BinaryData flate(
		BinaryData	const&			data,
		CompressionMethod const&	method	= CompressionMethod::ACM_ZIP,
		uint8 const&				level	= 9
	) try {
		String result;
		switch (method) {
		case CompressionMethod::ACM_NONE: return data;
		case CompressionMethod::ACM_ZIP: {
				StringSource ss(
					data.data(),
					data.size(),
					true,
					getFlator<T>(result, Math::clamp<uint8>(level, 0, 9))
				);
			}
		}
		return BinaryData(result.begin(), result.end());
	} catch (Exception const& e) {
		throw Error::FailedAction(
			e.what()
		);
	}
	namespace {
		template<typename T>
		BinaryData cbcEncrypt(
			BinaryData const&		data,
			String const&			password	= ""
		) {
			return cbcTransform<typename CBC_Mode<T>::Encryption>(data, password);
		}

		template<typename T>
		BinaryData cbcDecrypt(
			BinaryData const&		data,
			String const&			password	= ""
		) {
			return cbcTransform<typename CBC_Mode<T>::Decryption>(data, password);
		}
	}

	BinaryData encrypt(
		BinaryData const&		data,
		String const&			password	= "",
		EncryptionMethod const&	method		= EncryptionMethod::AEM_AES256
	) {
		switch (method) {
			default: throw Error::InvalidValue("Invalid encryption method!");
			case EncryptionMethod::AEM_NONE:	return data;
			case EncryptionMethod::AEM_AES256:	return cbcEncrypt<AES>(data, password);
		}
		return data;
	}

	BinaryData decrypt(
		BinaryData const&		data,
		String const&			password	= "",
		EncryptionMethod const&	method		= EncryptionMethod::AEM_AES256
	) {
		switch (method) {
			default: throw Error::InvalidValue("Invalid decryption method!");
			case EncryptionMethod::AEM_NONE:	return data;
			case EncryptionMethod::AEM_AES256:	return cbcDecrypt<AES>(data, password);
		}
		return data;
	}

	BinaryData compress(
		BinaryData	const&			data,
		CompressionMethod const&	method	= CompressionMethod::ACM_ZIP,
		uint8 const&				level	= 9
	) {
		return flate<Deflator>(data, method, level);
	}

	BinaryData decompress(
		BinaryData	const&			data,
		CompressionMethod const&	method	= CompressionMethod::ACM_ZIP,
		uint8 const&				level	= 9
	) {
		return flate<Inflator>(data, method, level);
	}

	// TODO: CRC Stuff
	uint32 generateCRC(BinaryData const& data) {
		return 0;
	}

	bool calculateCRC(BinaryData const& data, uint32 const& crc) {
		return true;
	}

	JSONData getStructure(fs::path const& path, StringList& files, String const& root = "") {
		JSONData dir = JSON::object();
		for (auto const& e : fs::directory_iterator(path)) {
			if (e.is_directory()) {
				String dirname = e.path().stem().string();
				dir[dirname] = getStructure(e, files, root + "/" + dirname);
			}
			else {
				String filename = e.path().filename().string();
				String filepath = root + "/" + filename;
				dir[filename] = filepath;
				files.push_back(filepath);
			}
		}
		return dir;
	}

	StringList getFileInfo(JSONData const& filestruct) {
		StringList res;
		for (auto& [name, data]: filestruct.items()) {
			if (data.is_string())
				res.push_back(data);
			else if (data.is_object() && !data.empty())
				for (String& s: getFileInfo(data))
					res.push_back(s);
		}
		return res;
	}

	size_t populateTree(JSONData& tree, List<uint64> const& values, size_t const& start = 0) {
		if (!tree.is_object())
			throw Error::FailedAction("file tree is not a JSON object!");
		size_t idx = start;
		for (auto& [name, data]: tree.items()) {
			if (data.is_string()) data = encoded(values[idx++]);
			else if (data.is_object()) idx = populateTree(data, values, idx);
			else throw Error::FailedAction("Invalid data type in file tree!");
		}
		return idx;
	}

	void populateTree(JSONData& tree, String const& root = "") {
		if (!tree.is_object())
			throw Error::FailedAction("file tree is not a JSON object!");
		for (auto& [name, data]: tree.items()) {
			String path = FileSystem::concatenatePath(root, name);
			if (data.is_string()) data = path;
			else if (data.is_object()) populateTree(data, path);
			else throw Error::FailedAction("Invalid data type in file tree!");
		}
	}

	struct FileHeader {
		uint64 uncSize;
		uint64 compSize;
		uint32 crc;
		// Put new things BELOW this line
	};

	struct ArchiveHeader {
		uint64	const headerSize		= sizeof(ArchiveHeader);
		uint64	const fileHeaderSize	= sizeof(FileHeader);
		uint64	dirInfoSize;
		uint64	version			= 0;
		uint64	minVersion		= 0;
		uint16	encryption		= (uint16) EncryptionMethod::AEM_AES256;
		uint16	compression		= (uint16)CompressionMethod::ACM_ZIP;
		uint8	level			= 9;
		// Put new things BELOW this line
	};

	void pack(
			String const& archivePath,
			String const& folderPath,
			String password = "",
			EncryptionMethod const& enc = EncryptionMethod::AEM_AES256,
			CompressionMethod const& comp = CompressionMethod::ACM_ZIP,
			uint8 const& complvl = 9
	) try {
		if (enc != EncryptionMethod::AEM_NONE && password.empty())
			throw Error::InvalidValue("Missing password for encrypted file!");
		_ARCDEBUGLN("FOLDER: ", folderPath, "\nARCHIVE: ", archivePath);
		// Get file structure
		_ARCDEBUGLN("Getting file structure...");
		JSONData dir;
		StringList files;
		JSONData& tree = dir["tree"];
		tree = getStructure(fs::path(folderPath), files, fs::path(folderPath).stem().string());
		_ARCDEBUGLN("\n", dir.dump(2, ' ', false, JSON::error_handler_t::replace), "\n");
		// Populate with temporary values
		List<uint64> locations(files.size(), 0);
		populateTree(tree, locations);
		// Get directory information
		String dirInfo = dir.dump(-1, ' ', false, JSON::error_handler_t::replace);
		// Open file
		std::ofstream file(archivePath, std::ios::binary | std::ios::trunc);
		file.exceptions(std::ofstream::badbit | std::ofstream::failbit);
		// Compressor & encryptor
		while (password.size() < CryptoPP::Rijndael::DEFAULT_KEYLENGTH)
			password += " ";
		ZlibCompressor			zipper;
		// Populate header
		_ARCDEBUGLN("Creating header...\n");
		// Preliminary parameters
		constexpr uint64 version	= 0;
		constexpr uint64 minVersion	= 0;
		// Header
		ArchiveHeader header;
		// Set main header params
		header.dirInfoSize	= dirInfo.length();	// directory info size
		header.version		= version;			// file format version
		header.minVersion	= minVersion;		// file format minimum version
		header.encryption	= (uint16)enc;		// encryption mode
		header.compression	= (uint16)comp;		// compression mode
		header.level		= complvl;			// compression level
		_ARCDEBUGLN("             HEADER SIZE: ", (uint64)header.headerSize,		"B"	);
		_ARCDEBUGLN("        FILE HEADER SIZE: ", (uint64)header.fileHeaderSize,	"B"	);
		_ARCDEBUGLN("     DIRECTORY INFO SIZE: ", (uint64)header.dirInfoSize,		"B"	);
		_ARCDEBUGLN("     FILE FORMAT VERSION: ", (uint64)header.version				);
		_ARCDEBUGLN(" FILE FORMAT MIN VERSION: ", (uint64)header.minVersion				);
		_ARCDEBUGLN("         ENCRYPTION MODE: ", (uint64)header.encryption				);
		_ARCDEBUGLN("        COMPRESSION MODE: ", (uint64)header.compression			);
		_ARCDEBUGLN("       COMPRESSION LEVEL: ", (uint64)header.level					);
		// Write header
		file.write((char*)&header, header.headerSize);
		// Write temp dir info
		file.seekp(header.headerSize);
		file.write(dirInfo.data(), dirInfo.size());
		// Write file info
		_ARCDEBUGLN("\nWriting files...\n");
		for (auto const& [i, f]: Helper::enumerate(files)) {
			locations[i] = file.tellp();
			// Read file
			FLD::BinaryData contents = FLD::loadBinaryFile(f);
			// Prepare header
			FileHeader fheader;
			fheader.uncSize = contents.size();				// Uncompressed file size
			// Process file
			if (!contents.empty()) {
				contents = compress(
					contents,
					comp,
					complvl
				);
				_ARCDEBUGLN("Before encryption: ", contents.size());
				contents = encrypt(
					contents,
					password,
					enc
				);
				_ARCDEBUGLN("After encryption: ", contents.size());
			}
			fheader.compSize	= contents.size();			// Compressed file size
			fheader.crc			= generateCRC(contents);	// CRC
			// Debug info
			_ARCDEBUGLN("'", files[i], "':");
			_ARCDEBUGLN("          FILE INDEX: ", i					);
			_ARCDEBUGLN("       FILE LOCATION: ", locations[i]		, " (", encoded(locations[i]), ")");
			_ARCDEBUGLN("   UNCOMPRESSED SIZE: ", fheader.uncSize	);
			_ARCDEBUGLN("     COMPRESSED SIZE: ", fheader.compSize	);
			_ARCDEBUGLN("               CRC32: ", fheader.crc		);
			_ARCDEBUGLN("");
			// Copy header & file data
			file.write((char*)&fheader, header.fileHeaderSize);
			file.write((char*)contents.data(), contents.size());
		}
		// Return & write proper directory info
		_ARCDEBUGLN("\nWriting directory info...\n");
		populateTree(tree, locations);
		_ARCDEBUGLN("\n", dir.dump(2, ' ', false, JSON::error_handler_t::replace), "\n");
		dirInfo = dir.dump(-1, ' ', false, JSON::error_handler_t::replace);
		file.seekp(header.headerSize);
		file.write(dirInfo.data(), dirInfo.size());
		// Close file
		file.flush();
		file.close();
	} catch (Error::Error const& e) {
		_ARCDEBUGLN(e.report());
		_ARCEXIT;
	} catch (std::runtime_error const& e) {
		_ARCDEBUGLN("ERROR: ", e.what());
		_ARCEXIT;
	}

	struct FileArchive {
		struct FileEntry {
			uint64 const	index;
			String const	path;
			FileHeader		header;
			BinaryData		data;
		};

		FileArchive() {}
		FileArchive(String const& path, String const& password = "") {open(path, password);}

		~FileArchive() {close();}

		FileArchive& open(String const& path, String const& password) try {
			if (streamOpen) return *this;
			// Set password
			pass = password;
			// Set exceptions
			archive.exceptions(std::ifstream::badbit | std::ifstream::failbit);
			// Open file
			archive.open(path, std::ios::binary | std::ios::in);
			// Read header
			auto lp = archive.tellg();
			archive.read((char*)&header, sizeof(ArchiveHeader));
			// Read file info
			String fs(header.dirInfoSize, ' ');
			archive.read(fs.data(), fs.size());
			archive.seekg(lp);
			try {
				fstruct = JSON::parse(fs);
			} catch (JSON::exception const& e) {
			}
			// Set open flag
			streamOpen = true;
			return *this;
		} catch (std::runtime_error const& e) {
			throw FileLoader::FileLoadError(e.what());
		}

		FileArchive& close() try {
			if (!streamOpen) return *this;
			archive.close();
			streamOpen = false;
			return *this;
		} catch (std::runtime_error const& e) {
			throw FileLoader::FileLoadError(e.what());
		}

		String getTextFile(String const& path) try {
			assertOpen();
			FileEntry fe = getFileEntry(path);
			processFileEntry(fe);
			return String(fe.data.begin(), fe.data.end());
		} catch (Error::FailedAction const& e) {
			throw FileLoader::FileLoadError(
				"could not load file '" + path + "'!",
				__FILE__,
				"unspecified",
				"unspecified",
				e.message
			);
		}

		BinaryData getBinaryFile(String const& path) try {
			assertOpen();
			FileEntry fe = getFileEntry(path);
			processFileEntry(fe);
			return fe.data;
		} catch (Error::FailedAction const& e) {
			throw FileLoader::FileLoadError(
				"could not load file '" + path + "'!",
				__FILE__,
				"unspecified",
				"unspecified",
				e.message
			);
		}

		JSONData getFileTree(String const& root = "") const {
			assertOpen();
			JSONData dir = fstruct["tree"];
			populateTree((!root.empty()) ? dir[root] : dir, root);
			return dir;
		}

		FileArchive& unpackTo(String const& path) {
			if (!streamOpen) return *this;
			JSONData ftree = getFileTree();
			_ARCDEBUGLN(ftree.dump(2, ' ', false, JSON::error_handler_t::replace), "\n");
			unpackLayer(ftree, path);
			return *this;
		}

		bool isOpen() const {return streamOpen;}

	private:

		void unpackLayer(JSONData& layer, String const& path) {
			assertOpen();
			for (auto& [name, data]: layer.items()) {
				if (data.is_string()) {
					String filepath = FileSystem::concatenatePath(path, data.get<String>());
					_ARCDEBUGLN(path, " + ", data.get<String>(), " = ", filepath);
					_ARCDEBUGLN(
						"'", name, "': ",
						filepath,
						" (dir: ", FileSystem::getDirectoryFromPath(filepath), ")"
					);
					FileSystem::makeDirectory(FileSystem::getDirectoryFromPath(filepath));
					FileLoader::saveBinaryFile(filepath, getBinaryFile(data));
				}
				else if (data.is_object()) unpackLayer(data, path);
				else directoryTreeError();
			}
		}

		void processFileEntry(FileEntry& entry) const {
			BinaryData data = entry.data;
			if (entry.header.uncSize == 0) return;
			_ARCDEBUGLN("Before decryption: ", data.size());
			data = decrypt(
				data,
				pass,
				(EncryptionMethod)header.encryption
			);
			_ARCDEBUGLN("After decryption: ", data.size());
			data = decompress(
				data,
				(CompressionMethod)header.compression,
				header.level
			);
			if (data.size() != entry.header.uncSize)
				corruptedFileError(entry.path);
			if (!calculateCRC(data, entry.header.crc))
				crcFailError(entry.path);
			entry.data = data;
		}

		FileEntry getFileEntry(String const& path) try {
			if (!fstruct["tree"].is_object())
				directoryTreeError();
			_ARCDEBUGLN("Getting file entry root...");
			String root = FileSystem::getParentDirectory(path), rest = "";
			if (root != path)
				rest = FileSystem::getChildPath(path);
			_ARCDEBUGLN("Getting file entry location...");
			uint64		idx	= getFileEntryLocation(root, rest, fstruct["tree"]);
			_ARCDEBUGLN("ENTRY LOCATION: ", idx);
			_ARCDEBUGLN("Getting file entry header...");
			FileHeader	fh	= getFileEntryHeader(idx);
			_ARCDEBUGLN("   UNCOMPRESSED SIZE: ", fh.uncSize	);
			_ARCDEBUGLN("     COMPRESSED SIZE: ", fh.compSize	);
			_ARCDEBUGLN("               CRC32: ", fh.crc		);
			_ARCDEBUGLN("Getting file entry data...");
			return FileEntry{idx, path, fh, getFileEntryData(idx, fh)};
		} catch (std::runtime_error const& e) {
			throw FileLoader::FileLoadError(
				"Failed at getting file entry '" + path + "'!",
				__FILE__,
				"unspecified",
				"unspecified",
				e.what()
			);
		}

		BinaryData getFileEntryData(uint64 const& index, FileHeader const& fh) {
			BinaryData fd(fh.compSize, 0);
			auto lp = archive.tellg();
			archive.seekg(index + header.fileHeaderSize);
			archive.read((char*)fd.data(), fh.compSize);
			archive.seekg(lp);
			return fd;
		}

		FileHeader getFileEntryHeader(uint64 const& index) {
			FileHeader fh;
			auto lp = archive.tellg();
			archive.seekg(index);
			archive.read((char*)&fh, header.fileHeaderSize);
			archive.seekg(lp);
			return fh;
		}

		uint64 getFileEntryLocation(String const& root, String const& path, JSONData& folder) try {
			if (folder.is_string())
				return decoded(folder.get<String>());
			if (!folder.is_object())
				doesNotExistError(root);
			if (path == "") {
				_ARCDEBUGLN("Getting file location...");
				return decoded(folder[root].get<String>());
			}
			String newRoot = FileSystem::getParentDirectory(path), newPath = "";
			if (newRoot != path)
				newPath = FileSystem::getChildPath(path);
			return getFileEntryLocation(newRoot, newPath, folder[root]);
		} catch (JSON::exception const& e) {
			doesNotExistError(root);
		}

		void assertOpen() const {if (!streamOpen) notOpenError();}

		[[noreturn]] void notOpenError() const {
			throw FileLoader::FileLoadError(
				"Archive is not open!"
			);
		}

		[[noreturn]] void doesNotExistError(String const& file) const {
			throw FileLoader::FileLoadError(
				"Directory or file '" + file + "' does not exist!",
				__FILE__
			);
		}

		[[noreturn]] void directoryTreeError() const {
			throw FileLoader::FileLoadError(
				"Missing or corrupted directory tree info!",
				__FILE__
			);
		}

		[[noreturn]] void corruptedFileError(String const& path) const {
			throw FileLoader::FileLoadError(
				"Corrupted file '" + path + "'",
				__FILE__
			);
		}

		[[noreturn]] void crcFailError(String const& path) const {
			throw FileLoader::FileLoadError(
				"CRC check failed for file '" + path + "'",
				__FILE__
			);
		}

		bool			streamOpen	= false;
		String			pass;
		std::ifstream	archive;
		ArchiveHeader	header;
		JSONData		fstruct;
	};

	void unpack(
		String const& archivePath,
		String const folderPath,
		String const& password = ""
	) try {
		_ARCDEBUGLN("\nOpening archive...\n");
		FileArchive arc(archivePath, password);
		_ARCDEBUGLN("\nExtracting data...\n");
		arc.unpackTo(folderPath);
	} catch (Error::Error const& e) {
		_ARCDEBUGLN(e.report());
		_ARCEXIT;
	} catch (std::runtime_error const& e) {
		_ARCDEBUGLN("ERROR: ", e.what());
		_ARCEXIT;
	}
}


#undef _ARCDEBUG
#undef _ARCDEBUGLN
#undef _ARCEXIT

#endif // MAKAILIB_TOOL_ARCHIVE_FUNCTIONALITY_H
