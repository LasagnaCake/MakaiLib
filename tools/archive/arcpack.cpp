#include "arcfun.hpp"

int main(int argc, char** argv) {
	if (argc < 3)
		DEBUGLN(
			"\n\nHow to use ArcPack:\n\n"
			"arcpack.exe \"RELATIVE_ARCHIVE_PATH\" \"RELATIVE_FOLDER_PATH\" \"PASSWORD\"\n\n"
		);
	else if (argc == 3)	ArcSys::pack(argv[1], argv[2], " ");
	else if (argc > 3)	ArcSys::pack(argv[1], argv[2], argv[3]);
	return 0;
}
