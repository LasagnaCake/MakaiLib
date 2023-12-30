#include <helper.hpp>
#include <filehandler.hpp>
#include <nlohmann/json.hpp>
#include <cryptopp/aes.h>
#include <cryptopp/zlib.h>
#include <cppcodec/base64_rfc4648.hpp>
#include <cppcodec/base32_rfc4648.hpp>
#include <filesystem>

namespace ArcSys {
	using JSON = nlohmann::json;
	typedef nlohmann::ordered_json JSONData;

	namespace {
		namespace fs = std::filesystem;
	}

	enum class EncryptionMethod: uint64 {
		AEM_NONE,
		AEM_AES128,
		AEM_AES192,
		AEM_AES256,
	};

	enum class CompressionMethod: uint64 {
		ACM_NONE,
		ACM_ZIP,
	};

	//cppcodec::base64_rfc4648::decode
	//cppcodec::base64_rfc4648::encode

	String encoded(uint64 const& v) {
		List<uint8> data;
		data.reserve(8);
		for SSRANGE(i, 0, 8) data[i] = (v >> (8 * i)) & 0xFF;
		for (uint8& b: data) DEBUG((uint16)b, "");
		DEBUGLN("");
		return cppcodec::base64_rfc4648::encode(data);
	}

	uint64 decoded(List<uint8> const& v) {
		List<uint8> data = cppcodec::base64_rfc4648::decode(v);
		size_t result = 0;
		for (auto [i, b]: Helper::enumerate(data))
			result |= (b << (8 * i));
		return result;
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

	size_t populateTree(JSONData& tree, List<uint64> values, size_t const& start = 0) {
		size_t idx = start;
		for (auto& [name, data]: tree.items()) {
			if (data.is_string()) data = encoded(idx++);
			else idx = populateTree(data, values, idx);
		}
		return idx;
	}

	void pack(
			String const& archivePath,
			String const& folderPath,
			String const& password = "",
			EncryptionMethod const& enc = EncryptionMethod::AEM_AES256,
			CompressionMethod const& comp = CompressionMethod::ACM_ZIP,
			uint8 const& complvl = 9
	) try {
		DEBUGLN("FOLDER: ", folderPath, "\nARCHIVE: ", archivePath);
		// Get file structure
		DEBUGLN("Getting file structure...");
		JSONData dir;
		StringList files;
		JSONData& tree = dir["tree"];
		tree = getStructure(fs::path(folderPath), files, fs::path(folderPath).stem().string());
		DEBUGLN("\n", dir.dump(2, ' ', false, JSON::error_handler_t::replace), "\n");
		// Populate with temporary values
		List<uint64> locations;
		for SSRANGE(i, 0, files.size())
			locations.push_back(0);
		populateTree(tree, locations);
		// Get directory information
		String dirInfo = dir.dump(-1, ' ', false, JSON::error_handler_t::replace);
		// Open file
		std::ofstream file(archivePath, std::ios::binary | std::ios::trunc);
		file.exceptions(std::ofstream::badbit | std::ofstream::failbit);
		// Compressor
		CryptoPP::ZlibCompressor zipper;
		// Populate header
		DEBUGLN("Creating header...\n");
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
		hptr16	= (uint16*)&hptr64[5];
		hptr16[0]	= (uint16)enc;		// encryption mode
		hptr16[1]	= (uint16)comp;		// compression mode
		hptr8	= (uint8*)&hptr16[2];
		hptr8[0]	= complvl;			// compression level
		DEBUGLN("HEADER SIZE:             ", (uint64)hptr64[0], "B");
		DEBUGLN("FILE HEADER SIZE:        ", (uint64)hptr64[1], "B");
		DEBUGLN("DIRECTORY INFO SIZE:     ", (uint64)hptr64[2], "B");
		DEBUGLN("FILE FORMAT VERSION:     ", (uint64)hptr64[3]);
		DEBUGLN("FILE FORMAT MIN VERSION: ", (uint64)hptr64[4]);
		DEBUGLN("ENCRYPTION MODE:         ", (uint64)hptr16[0]);
		DEBUGLN("COMPRESSION MODE:        ", (uint64)hptr16[1]);
		DEBUGLN("COMPRESSION LEVEL:       ", (uint64)hptr8[0]);
		// Write header
		file.write((char*)header, headerSize);
		// Write temp dir info
		file.seekp(headerSize);
		file.write(dirInfo.data(), dirInfo.size());
		// Write file info
		for (auto const& [i, f]: Helper::enumerate(files)) {
			locations[i] = file.tellp();
		}
		populateTree(tree, locations);
		// Return & write proper directory info
		DEBUGLN("\nWriting directory info...\n");
		file.seekp(headerSize);
		file.write(dirInfo.data(), dirInfo.size());
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
