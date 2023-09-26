#ifndef HELPING_HAND_H
#define HELPING_HAND_H

#include <string>
#include <vector>
#include <filesystem>
#include <locale>
#include <codecvt>
#include <any>
#include <variant>
#include <optional>
#ifdef _USE_CPP20_FORMAT_
#include <format>
#endif // _USE_CPP20_FORMAT_
#include "conceptual.hpp"
#include "definitions.hpp"

namespace Helper {
	namespace {
		using
			std::vector,
			std::map,
			std::unordered_map,
			std::string,
			std::wstring,
			std::initializer_list,
			std::pair,
			std::function,
			std::any,
			std::variant,
			std::optional
		;
	}

	typedef string String;

	typedef wstring WideString;

	template<typename T>
	using Arguments = initializer_list<T>;

	template<typename T>
	using List = vector<T>;

	template<typename K, typename V>
	using HashMap = map<K, V>;

	template<typename K, typename V>
	using FuzzyHashMap = unordered_map<K, V>;

	template<typename A, typename B>
	using Pair = pair<A, B>;

	template<typename T>
	using Dictionary = map<String, T>;

	template<typename T>
	using FuzzyDictionary = unordered_map<String, T>;

	template<typename T>
	using Entry = Pair<String, T>;

	template<typename T>
	using Function = function<T>;

	template<typename T>
	using Operation = Function<T(T const&)>;

	template<typename... Types>
	using PolyType = variant<Types...>;

	template<typename T>
	using Option = optional<T>;

	typedef any Any;

	typedef List<String>		StringList;
	typedef Arguments<String>	StringArguments;

	template<typename T, typename T2>
	List<T> convertList(List<T2> const& source) {
		List<T> res;
		res.reserve(source.size());
		for $each(i, source)
			res.push_back((T)i);
		return res;
	}

	template<typename T, typename T2>
	List<T> getKeys(HashMap<T, T2> const& lst) {
		List<T> keys;
		keys.reserve(lst.size());
		for (auto i = lst.begin(); i != lst.end(); i++) {
			keys.push_back(i->first);
		}
		return keys;
	}

	template<typename T, typename T2>
	List<T2> getValues(HashMap<T, T2> const& lst) {
		List<T2> values;
		values.reserve(lst.size());
		for (auto i = lst.begin(); i != lst.end(); i++) {
			values.push_back(i->second);
		}
		return values;
	}

	template<typename T, typename T2>
	List<T> getKeys(FuzzyHashMap<T, T2> const& lst) {
		List<T> keys;
		keys.reserve(lst.size());
		for (auto i = lst.begin(); i != lst.end(); i++) {
			keys.push_back(i->first);
		}
		return keys;
	}

	template<typename T, typename T2>
	List<T2> getValues(FuzzyHashMap<T, T2> const& lst) {
		List<T2> values;
		values.reserve(lst.size());
		for (auto i = lst.begin(); i != lst.end(); i++) {
			values.push_back(i->second);
		}
		return values;
	}
	// If you're using a compiler that supports it, please enable it.
	// God fucking dammit, MSYS2.
	#ifdef _USE_CPP20_FORMAT_
	template<Type::Number T>
	String floatString(T val, size_t precision = 2) {
		return std::format(std::format("{{:.{}f}}", precision), val);
	}
	#else
	template<Type::Number T>
	String floatString(T const& val, size_t const& precision = 1) {
		if (!precision)
			return std::to_string((long long)val);
		std::stringstream ss;
		ss << std::fixed << std::setprecision(precision) << val;
		return ss.str();
	}
	#endif // _USE_CPP20_FORMAT_

	template<Type::Number T>
	using FloatFormat = Pair<T, size_t>;

	template<Type::Number T>
	String floatString(FloatFormat<T> const& fmt) {
		return floatString(fmt->first, fmt->second);
	}

	StringList splitString(String const& str, char const& sep) {
		StringList res;
		std::istringstream stream(str);
		String current;
		while (std::getline(stream, current, sep))
			res.push_back(current);
		return res;
	}

	template<typename T>
	using Enumerated = HashMap<size_t, T>;

	template<typename T>
	Enumerated<T> enumerate(List<T> lst) {
		Enumerated<T> res;
		size_t i = 0;
		for(auto& elem: lst)
			res[i++] = elem;
		return res;
	}
}

using Helper::String;
using Helper::WideString;
using Helper::List;
using Helper::HashMap;
using Helper::FuzzyHashMap;
using Helper::StringList;
using Helper::Arguments;
using Helper::Dictionary;
using Helper::FuzzyDictionary;
using Helper::Pair;
using Helper::Entry;
using Helper::Function;
using Helper::Operation;
using Helper::PolyType;
using Helper::Any;
using Helper::Option;

WideString toWideString(String const& str){
	using convert_typeX = std::codecvt_utf8_utf16<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.from_bytes(str);
}

String toString(WideString const& wstr){
	using convert_typeX = std::codecvt_utf8_utf16<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.to_bytes(wstr);
}

template<typename T>
constexpr inline String toString(const T& val) {
	return std::to_string(val);
}

template<typename T>
constexpr inline String toWideString(const T& val) {
	return std::to_wstring(val);
}

inline int			toInt(String s)			{return std::stoi(s);	}
inline long			toLong(String s)		{return std::stol(s);	}
inline long long	toLongLong(String s)	{return std::stoll(s);	}
inline float		toFloat(String s)		{return std::stof(s);	}
inline double		toDouble(String s)		{return std::stod(s);	}

namespace FileSystem {
	namespace {
		namespace fs = std::filesystem;
		using namespace Helper;
	}

	bool isDirectory(String const& dir) {
		return fs::is_directory(dir);
	}

	bool exists(String const& path) {
		return fs::exists(path);
	}

	void makeDirectory(String const& dir) {
		if (!isDirectory(dir) || !exists(dir)) {
			fs::create_directories(dir);
		}
	}

	void makeDirectory(StringList const& dirs) {
		for (auto& d: dirs)
			makeDirectory(d);
	}

	void makeDirectory(StringArguments const& dirs) {
		for (auto& d: dirs)
			makeDirectory(d);
	}

	String concatenatePath(String const& root, String const& path) {
		String res = root;
		if (!path.empty()) res += "/" + path;
		return res;
	}

	String concatenatePath(String const& root, StringList const& paths) {
		String res = root;
		for(auto& path: paths) {
			if (!path.empty()) res += "/" + path;
		}
		return res;
	}

	String concatenatePath(String const& root, StringArguments const& paths) {
		String res = root;
		for(auto& path: paths) {
			if (!path.empty()) res += "/" + path;
		}
		return res;
	}

	inline String getFileName(string path, bool removeExtension = false) {
		return toString(WideString((removeExtension ? fs::path(path).stem() : fs::path(path).filename()).c_str()));
	}

	String getDirectoryFromPath(String const& path) {
		return std::regex_replace(
			path,
			std::regex("(?!([A-z].*)[/])([A-z].*\\.[A-z].*)"),
			""
		);
	}
}

#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
#include <winapifamily.h>
#include <commdlg.h>
#endif

namespace System {
	int launchApp(String const& path, String const& directory = "") {
		// This is a nightmare, but the other option pops up a command prompt.
		#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
		STARTUPINFO sInfo;
		PROCESS_INFORMATION pInfo;
		ZeroMemory(&sInfo, sizeof(sInfo));
		sInfo.cb = sizeof(sInfo);
		ZeroMemory(&pInfo, sizeof(pInfo));
		auto proc = CreateProcess(
			path.c_str(),
			NULL,
			NULL,
			NULL,
			FALSE,
			0,
			NULL,
			directory.empty() ? NULL : directory.c_str(),
			&sInfo,
			&pInfo
		);
		if (!proc) throw Error::FailedAction("could not find 'data/subsys/winres.exe'!");
		proc = WaitForSingleObject(pInfo.hProcess, INFINITE);
		DWORD res;
		GetExitCodeProcess(pInfo.hProcess, &res);
		CloseHandle(pInfo.hProcess);
		CloseHandle(pInfo.hThread);
		return (int)res;
		#else
		return system("data\\subsys\\winres.exe");
		#endif
	}

	String openFileDialog(String filter = "All\0*.*\0") {
		#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
		OPENFILENAME ofn;
		char szFile[260]	= {0};
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter.c_str();
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
		if (GetOpenFileName(&ofn))
			return String(ofn.lpstrFile);
		#endif
		return "";
	}

	String saveFileDialog(String filter = "All\0*.*\0") {
		#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__)
		OPENFILENAME ofn;
		char szFile[260]	= {0};
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter.c_str();
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
		if (GetSaveFileName(&ofn))
			return String(ofn.lpstrFile);
		#endif
		return "";
	}
}

namespace Time {
	namespace {namespace chrono = std::chrono;}

	typedef chrono::system_clock	Clock;

	namespace {auto start = Clock::now();}

	typedef chrono::years			Years;
	typedef chrono::months			Months;
	typedef chrono::weeks			Weeks;
	typedef chrono::days			Days;
	typedef chrono::hours			Hours;
	typedef chrono::minutes			Minutes;
	typedef chrono::seconds			Seconds;
	typedef chrono::milliseconds	Millis;
	typedef chrono::microseconds	Micros;
	typedef chrono::nanoseconds		Nanos;

	/// Gets the current time, since the epoch (Jan 1st, 1970).
	template <class T = Micros>
	constexpr inline size_t sinceEpoch() {
		return chrono::duration_cast<T>(Clock::now().time_since_epoch()).count();
	}

	/// Gets the current time, since program start.
	template <class T = Micros>
	constexpr inline size_t sinceStart() {
		return chrono::duration_cast<T>(Clock::now() - start).count();
	}
}

#endif // HELPING_HAND_H
