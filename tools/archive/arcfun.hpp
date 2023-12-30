#include <helper.hpp>
#include <filehandler.hpp>
#include <nlohmann/json.hpp>
#include <cryptopp/aes.h>
#include <filesystem>

namespace Arc {
	using JSON = nlohmann::json;
	typedef nlohmann::unordered_json JSONData;

	namespace {
		using fs = std::filesystem
	}

	JSON getStructure(fs::path const& path) {
		JSON result;
		for (auto const& e : fs::directory_iterator(path)) {
			JSON& folder = result[path.root_name().string()];
			if (e.is_directory())
				folder[e.root_name().string()] = getStructure(e);
			else
				folder[e.root_name().string()] = e.filename().string();
		}
		return result;
	}

	void pack(String const& folderPath, String const& archivePath) {
		try {
			JSON dir = getStructure(fs::path(folderPath));
			DEBUGLN(dir.dump(2, ' ', false, JSON::error_handler_t::replace));
		} catch (std::runtime_error const& e) {
			DEBUGLN("ERROR: " e.what());
		}
	}

	void unpack(String const& archivePath, String const folderPath) {
	}
}
