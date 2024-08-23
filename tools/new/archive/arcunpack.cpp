#define ARCSYS_APPLICATION_

#include <archive.hpp>

int main(int argc, char** argv) {
	DEBUGLN("Starting...");
	if (argc < 3)
		DEBUGLN(
			"\n\nHow to use ArcUnpack:\n\n"
			"arcunpack.exe \"RELATIVE_ARCHIVE_PATH\" \"RELATIVE_FOLDER_PATH\" \"PASSWORD\"\n\n"
		);
	else if (argc == 3)	{
		DEBUGLN("\n\n\n<<<  MODE 2  >>>\n\n\n");
		Makai::Tool::Arch::unpack(argv[1], argv[2], " ");
	}
	else if (argc > 3) {
		DEBUGLN("\n\n\n<<<  MODE 3  >>>\nPassword: ", argv[3], "\n\n\n");
		Makai::Tool::Arch::unpack(argv[1], argv[2], argv[3]);
	}
	return 0;
}
