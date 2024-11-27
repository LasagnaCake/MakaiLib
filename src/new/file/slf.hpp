#ifndef MAKAILIB_FILE_SLF_H
#define MAKAILIB_FILE_SLF_H

#include "../compat/ctl.hpp"
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
		String		code = "";
	};

	struct SLFData {
		String				folder;
		List<ShaderEntry>	shaders;
	};

	SLFData parse(String const& slf, String const& srcFolder, bool const pathOnly = true);
	SLFData loadFile(String const& path, bool const pathOnly = true);
	SLFData getFile(String const& path, bool const pathOnly = true);
}

namespace MkSLF = Makai::SLF;

namespace Makai::File {
	inline SLF::SLFData loadSLF(String const& path)	{return SLF::loadFile(path);	}
	inline SLF::SLFData getSLF(String const& path)	{return SLF::getFile(path);		}
}

#endif // MAKAILIB_FILE_SLF
