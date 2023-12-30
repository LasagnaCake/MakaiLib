#include "arcfun.hpp"

int main(int argc, char** argv) {
	DEBUGLN("Starting...");
	DEBUGLN("ENC_TEST1: ", ArcSys::encoded(0x000000));
	DEBUGLN("ENC_TEST2: ", ArcSys::encoded(0x0000FF));
	DEBUGLN("ENC_TEST3: ", ArcSys::encoded(0x00FFFF));
	DEBUGLN("ENC_TEST4: ", ArcSys::encoded(0xFFFFFF));
	DEBUGLN("ENC_TEST5: ", ArcSys::encoded(0xFFFFFF));
	if (argc < 3)
		DEBUGLN(
			"\n\nHow to use ArcPack:\n\n"
			"arcpack.exe \"RELATIVE_ARCHIVE_PATH\" \"RELATIVE_FOLDER_PATH\" \"PASSWORD\"\n\n"
		);
	else if (argc == 3)	ArcSys::pack(argv[1], argv[2]);
	else if (argc > 3)	ArcSys::pack(argv[1], argv[2], argv[3]);
	return 0;
}
