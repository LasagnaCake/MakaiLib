#include <helper.hpp>
#include <filehandler.hpp>
#include <nlohmann/json.hpp>
#include <cryptopp/aes.h>
#include <filesystem>

namespace ArcSys {
	using JSON = nlohmann::json;
	typedef nlohmann::ordered_json JSONData;

	namespace {
		namespace fs = std::filesystem;
	}

	JSON getStructure(fs::path const& path, String const& root = "") {
		JSON result = JSON::object();
		for (auto const& e : fs::directory_iterator(path)) {
			String eName = e.path().stem().string();
			if (e.is_directory())
				result[eName] = getStructure(e, root + "/" + eName);
			else
				result[eName] = root + "/" + e.path().filename().string();
		}
		return result;
	}

	void pack(String const& archivePath, String const& folderPath, String const& password = "")
	try {
		DEBUGLN("FOLDER: ", folderPath, "\nARCHIVE: ", archivePath);
		DEBUGLN("Getting file structure...");
		JSON dir = getStructure(fs::path(folderPath), fs::path(folderPath).stem().string());
		DEBUGLN("\n", dir.dump(2, ' ', false, JSON::error_handler_t::replace), "\n");
	} catch (Error::Error const& e) {
		DEBUGLN(e.report());
	} catch (std::runtime_error const& e) {
		DEBUGLN("ERROR: ", e.what());
	}

	void unpack(String const& archivePath, String const folderPath, String const& password = "")
	try {
	} catch (Error::Error const& e) {
		DEBUGLN(e.report());
	} catch (std::runtime_error const& e) {
		DEBUGLN("ERROR: ", e.what());
	}
}
