#define ARCSYS_APPLICATION_

#include <makai/tool/archive/archive.hpp>

int main(int argc, char** argv) {
	DEBUGLN("Starting...");
	if (argc < 3)
		DEBUGLN(
			"\n\nHow to use ArcPack:\n\n"
			"arcpack.exe \"RELATIVE_ARCHIVE_PATH\" \"RELATIVE_FOLDER_PATH\" \"PASSWORD\"\n\n"
		);
	else if (argc == 3)	{
		DEBUGLN("\n\n\n<<<  MODE 2  >>>\n\n\n");
		Makai::Tool::Arch::pack(argv[1], argv[2], " ");
	}
	else if (argc > 3) {
		DEBUGLN("\n\n\n<<<  MODE 3  >>>\n\n\n");
		Makai::Tool::Arch::pack(argv[1], argv[2], argv[3]);
	}
	return 0;
}
