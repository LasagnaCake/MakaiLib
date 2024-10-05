#ifndef CTL_OS_SYSTEM_H
#define CTL_OS_SYSTEM_H

#include "../namespace.hpp"
#include "../container/string.hpp"
#include "../container/error.hpp"
#include "../regex/core.hpp"
#include "filesystem.hpp"

#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__) && !defined(_NO_WINDOWS_PLEASE_)
#include <windows.h>
#include <winapifamily.h>
#include <commdlg.h>
#else

#endif

CTL_NAMESPACE_BEGIN

namespace OS {
	inline String sanitizedArgument(String arg) {
		arg = Regex::replace(arg, "\\\\+", "\\\\");
		#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__) && !defined(_NO_WINDOWS_PLEASE_)
		arg = Regex::replace(arg, "\\\\+\"", "\\\"");
		return "\"" + arg + "\"";
		#else
		arg = Regex::replace(arg, "\\\\+'", "\\'");
		arg = Regex::replace(arg, "'", "\\'");
		return "'" + arg + "'";
		#endif
	}

	inline int launch(String const& path, String const& directory = "", StringList args = StringList()) {
		if (!FS::exists(path))
			throw Error::InvalidValue(
				"File [" + path + "] does not exist!",
				__FILE__,
				toString(__LINE__),
				"CTL::OS::launch"
			);
		#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__) && !defined(_NO_WINDOWS_PLEASE_)
		String prgArgs = "";
		if (!args.empty())
			for (String const& arg: args)
				prgArgs += sanitizedArgument(arg) + " ";
		prgArgs = sanitizedArgument(path) + (args.empty() ? "" : (" " + prgArgs));
		// This is a nightmare, but the other option pops up a command prompt.
		STARTUPINFOA sInfo;
		PROCESS_INFORMATION pInfo;
		memset(&sInfo, 0, sizeof(sInfo));
		sInfo.cb = sizeof(sInfo);
		memset(&pInfo, 0, sizeof(pInfo));
		auto proc = CreateProcessA(
			(LPCSTR)path.cstr(),
			(LPSTR)prgArgs.cstr(),
			NULL,
			NULL,
			FALSE,
			0,
			NULL,
			(LPCSTR)directory.empty() ? NULL : directory.cstr(),
			(LPSTARTUPINFOA)&sInfo,
			&pInfo
		);
		if (!proc) throw Error::FailedAction(toString("could not find '", path,"!"));
		proc = WaitForSingleObject(pInfo.hProcess, INFINITE);
		DWORD res;
		GetExitCodeProcess(pInfo.hProcess, &res);
		CloseHandle(pInfo.hProcess);
		CloseHandle(pInfo.hThread);
		return (int)res;
		#else
		List<const char*> prgArgs;
		prgArgs.pushBack(FS::fileName(path));
		for (String& arg: args)
			prgArgs.pushBack(arg.cstr());
		prgArgs.pushBack(NULL);
		return execv(path.cstr(), prgArgs.data());
		#endif
	}

	inline String openFileDialog(String filter = "All\0*.*\0") {
		#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__) && !defined(_NO_WINDOWS_PLEASE_)
		OPENFILENAMEA ofn;
		char szFile[260] = {0};
		memset(&ofn, 0, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = (LPSTR)szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = (LPSTR)filter.cstr();
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags =
			OFN_PATHMUSTEXIST
		|	OFN_FILEMUSTEXIST
		|	OFN_NONETWORKBUTTON
		|	OFN_NOCHANGEDIR
		|	OFN_EXPLORER
		;
		if (GetOpenFileNameA(&ofn))
			return String((cstring)ofn.lpstrFile);
		#endif
		return "";
	}

	inline String saveFileDialog(String filter = "All\0*.*\0") {
		#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__) && !defined(_NO_WINDOWS_PLEASE_)
		OPENFILENAMEA ofn;
		char szFile[260] = {0};
		memset(&ofn, 0, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = (LPSTR)szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = (LPSTR)filter.cstr();
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags =
			OFN_PATHMUSTEXIST
		|	OFN_OVERWRITEPROMPT
		|	OFN_NONETWORKBUTTON
		|	OFN_NOCHANGEDIR
		|	OFN_EXPLORER
		;
		if (GetSaveFileNameA(&ofn))
			return String((cstring)ofn.lpstrFile);
		#endif
		return "";
	}
}

CTL_NAMESPACE_END

#endif // CTL_OS_SYSTEM_H
