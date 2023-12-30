#include <helper.hpp>
#include <filehandler.hpp>
#include <nlohmann/json.hpp>
#include <cryptopp/aes.h>
#include <cryptopp/zlib.h>
#include <filesystem>

namespace ArcSys {
	using JSON = nlohmann::json;
	typedef nlohmann::ordered_json JSONData;

	namespace {
		namespace fs = std::filesystem;
	}

	enum class EncryptionMethod: size_t {
		EM_NONE,
		EM_AES128,
		EM_AES192,
		EM_AES256,
	};

	enum class CompressionMethod: size_t {
		CM_NONE,
		CM_ZIP
	};

	JSON getStructure(fs::path const& path, String const& root = "") {
		JSON result = JSON::object();
		for (auto const& e : fs::directory_iterator(path)) {
			if (e.is_directory()) {
				String dirname = e.path().stem().string();
				result[dirname] = getStructure(e, root + "/" + dirname);
			}
			else {
				String filename = e.path().filename().string();
				result[filename] = root + "/" + filename;
			}
		}
		return result;
	}

	void pack(
			String const& archivePath,
			String const& folderPath,
			String const& password = "",
			EncryptionMethod const& enc = EncryptionMethod::EM_AES256,
			CompressionMethod const& comp = CompressionMethod::CM_ZIP,
			uint16 const& complvl = 64
	) try {
		DEBUGLN("FOLDER: ", folderPath, "\nARCHIVE: ", archivePath);
		// Get file structure
		DEBUGLN("Getting file structure...");
		JSON dir = getStructure(fs::path(folderPath), fs::path(folderPath).stem().string());
		DEBUGLN("\n", dir.dump(2, ' ', false, JSON::error_handler_t::replace), "\n");
		// Get directory information
		String dirInfo = dir.dump(-1, ' ', false, JSON::error_handler_t::replace);
		// Open file
		std::ofstream file(archivePath, std::ios::binary | std::ios::trunc);
		file.exceptions(std::ofstream::badbit | std::ofstream::failbit);
		// Compressor
		CryptoPP::ZlibCompressor zipper;
		// Populate header
		DEBUGLN("Creating header...");
		// Preliminary parameters
		constexpr uint64 headerSize	= 128;
		constexpr uint64 fhSize		= 128;
		constexpr uint64 version	= 0;
		constexpr uint64 minVersion	= 0;
		// Header
		uint8 header[headerSize];
		// Accessors
		uint64*	hptr64	= (uint64*)header;
		uint16*	hptr16	= (uint16*)header;
		uint8*	hptr8	= (uint8*)header;
		// Set main header params
		hptr64[0]	= headerSize;		// header size
		hptr64[1]	= fhSize;			// file header size
		hptr64[2]	= dirInfo.length();	// directory info size
		hptr64[3]	= version;			// file format version
		hptr64[4]	= minVersion;		// file format minimum version
		hptr16	= (uint16*)&hptr64[4];
		hptr16[0]	= (uint16)enc;		// encryption mode
		hptr16[1]	= (uint16)comp;		// compression mode
		hptr8	= (uint8*)&hptr16[3];
		hptr16[2]	= complvl;			// compression level
		// Write header
		file.write(header, headerSize);
		// Write directory info
		file.write(dirInfo.data(), dirInfo.size())
		// Close file
		file.flush();
		file.close();
	} catch (Error::Error const& e) {
		DEBUGLN(e.report());
	} catch (std::runtime_error const& e) {
		DEBUGLN("ERROR: ", e.what());
	}

	void unpack(
		String const& archivePath,
		String const folderPath,
		String const& password = ""
	) try {
	} catch (Error::Error const& e) {
		DEBUGLN(e.report());
	} catch (std::runtime_error const& e) {
		DEBUGLN("ERROR: ", e.what());
	}
}
