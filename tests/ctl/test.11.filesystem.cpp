#include <ctl/ctl.hpp>
#include <ctlex/ctlex.hpp>

int main() {
	if (CTL::OS::FS::exists("test"))
		CTL::OS::FS::remove("test");
	CTL::OS::FS::makeDirectory(
		"test/dir1/a",
		"test/dir2/b",
		"test/dir3/c",
		"test/dir2/d",
		"test/dir1/e"
	);
	DEBUGLN("Filesystem test passed!");
	return 0;
}
