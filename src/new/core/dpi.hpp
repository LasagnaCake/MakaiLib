#ifndef MAKAILIB_CORE_DPI_H
#define MAKAILIB_CORE_DPI_H

#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
#define WIN_ONLY_DPI_WARNING
#else
#define WIN_ONLY_DPI_WARNING [[gnu::warning("DPI awareness is not supported on other platforms!")]]
#endif // MAKAILIB_WINDOWS_8_OR_LATER

namespace Makai {
	WIN_ONLY_DPI_WARNING
	void setDPIAware();
	#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
	#ifndef MAKAILIB_WINDOWS_8_1_OR_LATER
	[[gnu::warning("Disabling DPI awareness is not available for Windows 8.0 and earlier!")]]
	#endif // MAKAILIB_WINDOWS_8_1_OR_LATER
	#else
	WIN_ONLY_DPI_WARNING
	#endif // MAKAILIB_WINDOWS_8_OR_LATER
	void setDPIUnaware();
}

#endif // MAKAILIB_CORE_DPI_H
