#ifndef MAKAILIB_FILE_SLF_H
#define MAKAILIB_FILE_SLF_H

#include "../ctl/ctl.hpp"
#include "get.hpp"

namespace Makai::SLF {
	enum class ShaderType {
		ST_INVALID = -1,
		ST_FRAGMENT,
		ST_VERTEX,
		ST_COMPUTE,
		ST_GEOMETRY,
		ST_TESS_CTRL,
		ST_TESS_EVAL
	};

	struct ShaderEntry {
		String		path;
		ShaderType	type;
	};

	struct SLFData {
		String				folder;
		List<ShaderEntry>	shaders;
	};

	SLFData loadFile(String const& path);
}

#endif // MAKAILIB_FILE_SLF
