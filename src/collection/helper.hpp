#ifndef HELPING_HAND_H
#define HELPING_HAND_H

#ifdef ENABLE_DEBUG_OUTPUT_
#include <iostream>
#endif
#include <syncstream>
#include <string>
#include <vector>
#include <filesystem>
#include <locale>
#include <codecvt>
#include <any>
#include <variant>
#include <optional>
#include <span>
#include <cctype>
#include <tuple>
#include <map>
#include <unordered_map>
#include <thread>
#include <regex>
#ifdef _USE_CPP20_FORMAT_
#include <format>
#endif // _USE_CPP20_FORMAT_
#include "conceptual.hpp"
#include "definitions.hpp"
#include "types.hpp"
#include "errors.hpp"

namespace std {
	constexpr std::string	to_string(std::string const& s)		{return s;	}
	constexpr std::wstring	to_wstring(std::wstring const& ws)	{return ws;	}

	template<typename T>
	constexpr std::string	to_string(T const& val)	requires Type::Convertible<T, string>	{return static_cast<string>(val);	}
	template<typename T>
	constexpr std::wstring	to_wstring(T const& val) requires Type::Convertible<T, wstring>	{return static_cast<wstring>(val);	}

	constexpr std::string	to_string(char const& c)	{return string() + c;	}
	constexpr std::wstring	to_wstring(wchar const& wc)	{return wstring() + wc;	}
}

namespace Fold {
	template<typename... Args>
	constexpr bool land(Args const&... args) {
		return (... && args);
	}

	template<typename... Args>
	constexpr bool lor(Args const&... args) {
		return (... || args);
	}

	template<typename T, typename... Args>
	constexpr T band(Args const&... args) {
		return (... & args);
	}

	template<typename T, typename... Args>
	constexpr T bor(Args const&... args) {
		return (... | args);
	}

	template<typename T, typename... Args>
	constexpr T bxor(Args const&... args) {
		return (... ^ args);
	}

	template<typename T, typename... Args>
	constexpr T add(Args const&... args) {
		return (... + args);
	}

	template<typename T, typename... Args>
	constexpr T sub(Args const&... args) {
		return (... - args);
	}

	template<typename T, typename... Args>
	constexpr T mul(Args const&... args) {
		return (... * args);
	}

	template<typename T, typename... Args>
	constexpr T div(Args const&... args) {
		return (... / args);
	}

	template<typename T, typename... Args>
	constexpr T& strins(T& stream, Args const&... args) {
		(stream << ... << args);
		return stream;
	}

	template<typename T, typename... Args>
	constexpr T avg(Args const&... args) {
		return (add(args...)) / sizeof...(Args);
	}
}

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
			std::optional,
			std::span,
			std::tuple
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

	template<typename T>
	using Procedure = Function<void(T)>;

	template<typename... Types>
	using Poly = variant<Types...>;

	template<typename T>
	using Nullable = optional<T>;

	template<typename T, size_t LEN>
	using Span = span<T, LEN>;

	template<typename T>
	using Tuple = tuple<T>;

	typedef any Any;

	typedef List<String>		StringList;
	typedef Arguments<String>	StringArguments;

	typedef std::thread			Thread;

	template<typename T, typename T2>
	constexpr List<T> convertList(List<T2> const& source) {
		List<T> res;
		res.reserve(source.size());
		for EACH(i, source)
			res.push_back((T)i);
		return res;
	}

	template<typename T, typename T2>
	constexpr List<T> getKeys(HashMap<T, T2> const& lst) {
		List<T> keys;
		keys.reserve(lst.size());
		for (auto i = lst.begin(); i != lst.end(); i++) {
			keys.push_back(i->first);
		}
		return keys;
	}

	template<typename T, typename T2>
	constexpr List<T2> getValues(HashMap<T, T2> const& lst) {
		List<T2> values;
		values.reserve(lst.size());
		for (auto i = lst.begin(); i != lst.end(); i++) {
			values.push_back(i->second);
		}
		return values;
	}

	template<typename T, typename T2>
	constexpr List<T> getKeys(FuzzyHashMap<T, T2> const& lst) {
		List<T> keys;
		keys.reserve(lst.size());
		for (auto i = lst.begin(); i != lst.end(); i++) {
			keys.push_back(i->first);
		}
		return keys;
	}

	template<typename T, typename T2>
	constexpr List<T2> getValues(FuzzyHashMap<T, T2> const& lst) {
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
	String floatString(T const& val, size_t const& precision = 2) {
		return std::format(std::format("{{:.{}f}}", precision), val);
	}
	#else
	template<Type::Number T>
	String floatString(T const& val, size_t const& precision = 1) {
		if (!precision)
			return std::to_string((long long)val);
		std::stringstream ss;
		Fold::strins(
			ss,
			std::fixed,
			std::setprecision(precision),
			val
		);
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

	StringList splitString(String const& str, Arguments<char> const& seps) {
		StringList res = {str};
		for (char const& sep: seps) {
			StringList buf1 = res;
			res.clear();
			for (String const& s1: buf1) {
				StringList buf2 = splitString(s1, sep);
				for (String s2: buf2) res.push_back(s2);
			}
		}
		return res;
	}

	template<typename T>
	using Enumerated = HashMap<size_t, T>;

	template<typename T>
	constexpr Enumerated<T> enumerate(List<T> const& lst) {
		Enumerated<T> res;
		size_t i = 0;
		for(auto& elem: lst)
			res[i++] = elem;
		return res;
	}

	template<typename T, typename... Args>
	constexpr Enumerated<T> enumerate(Args... args)
	requires (... && Type::Constructible<T, Args>) {
		Enumerated<T> res;
		size_t i = 0;
		((res[i++] = args), ...);
		return res;
	}

	constexpr bool isHexString(String const& str) {
		return std::all_of(str.begin(), str.end(), [](unsigned char const& c)->bool{return std::isxdigit(c);});
	}

	template<typename T>
	constexpr void byteCopy(T& from, T& to) {
		memcpy(&from, &to, sizeof(T));
	}

	template<typename T>
	constexpr void arrayCopy(T* from, T* to, size_t sz) {
		if (!Fold::land(from, to, sz))
			return;
		memcpy(from, to, sizeof(T) * sz);
	}

	#ifdef ENABLE_DEBUG_OUTPUT_
	template<typename... Args>
	constexpr void print(Args... args) {
		Fold::strins(std::cout, args...);
	}

	template<typename... Args>
	constexpr void println(Args... args) {
		print(args..., "\n");
	}

	template<typename... Args>
	constexpr void syncprint(Args... args) {
		Fold::strins(std::osyncstream(std::cout), args...);
	}

	template<typename... Args>
	constexpr void syncprintln(Args... args) {
		syncprint(args..., "\n");
	}
	#endif // ENABLE_DEBUG_OUTPUT_

	template<typename T, typename... Args>
	constexpr List<T> toList(Args const&... args)
	requires (... && Type::Constructible<T, Args>)
	&& (!(Type::Equal<T, String> || Type::Equal<T, WideString>)) {
		List<T> lst;
		(lst.push_back(T(args)), ...);
		return lst;
	}

	template<typename T, typename... Args>
	constexpr StringList toList(Args const&... args)
	requires Type::Equal<T, String> {
		StringList lst;
		(lst.push_back(std::to_string(args)), ...);
		return lst;
	}

	template<typename T, typename... Args>
	constexpr List<WideString> toList(Args const&... args)
	requires Type::Equal<T, WideString> {
		List<WideString> lst;
		(lst.push_back(std::to_wstring(args)), ...);
		return lst;
	}

	template<typename T, typename... Args>
	constexpr String concatenate(Args const&... args)
	requires Type::Equal<T, String> {
		return (... + std::to_string(args));
	}

	template<typename T, typename... Args>
	constexpr WideString concatenate(Args const&... args)
	requires Type::Equal<T, WideString> {
		return (... + std::to_wstring(args));
	}

	constexpr String concatenate(StringArguments const& args) {
		String s = "";
		for (String a: args) s += a;
		return s;
	}

	constexpr String concatenate(StringList const& args) {
		String s = "";
		for (String a: args) s += a;
		return s;
	}

	typedef std::regex				Regex;
	typedef std::wregex				WideRegex;
}

#ifdef ENABLE_DEBUG_OUTPUT_
#ifndef DEBUG
#define DEBUG(...)		Helper::print(__VA_ARGS__)
#endif // DEBUG
#ifndef DEBUGLN
#define DEBUGLN(...)	Helper::println(__VA_ARGS__)
#endif // DEBUGLN
#ifndef SYNCDEBUG
#define SYNCDEBUG(...)		Helper::syncprint(__VA_ARGS__)
#endif // SYNCDEBUG
#ifndef SYNCDEBUGLN
#define SYNCDEBUGLN(...)	Helper::syncprintln(__VA_ARGS__)
#endif // SYNCDEBUGLN
#else
#ifndef DEBUG
#define DEBUG(...)
#endif // DEBUG
#ifndef DEBUGLN
#define DEBUGLN(...)
#endif // DEBUGLN
#ifndef SYNCDEBUG
#define SYNCDEBUG(...)
#endif // SYNCDEBUG
#ifndef SYNCDEBUGLN
#define SYNCDEBUGLN(...)
#endif // SYNCDEBUGLN
#endif // ENABLE_DEBUG_OUTPUT_

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
using Helper::Procedure;
using Helper::Poly;
using Helper::Any;
using Helper::Nullable;
using Helper::Regex;
using Helper::Span;
using Helper::Thread;

inline String regexReplace(String const& str, Regex const& expr, String const& fmt) {
	return std::regex_replace(str, expr, fmt);
}

inline bool regexContains(String const& str, Regex const& expr) {
	std::smatch rm;
	return std::regex_search(str, rm, expr);
}

inline bool regexMatches(String const& str, Regex const& expr) {
	std::smatch rm;
	return std::regex_match(str, rm, expr);
}

inline StringList regexFind(String const& str, Regex const& expr) {
	std::smatch rm;
	StringList result;
	std::regex_search(str, rm, expr);
	for SSRANGE(i, 1, rm.size()-1)
		result.push_back(rm[i].str());
	return result;
}

inline String regexFindFirst(String const& str, Regex const& expr) {
	std::smatch rm;
	std::regex_search(str, rm, expr);
	return rm[1].str();
}

inline String		regexReplace(String const& str, String const& expr, String const& fmt)	{return regexReplace(str, Regex(expr), fmt);	}
inline bool			regexContains(String const& str, String const& expr)					{return regexContains(str, Regex(expr));		}
inline bool			regexMatches(String const& str, String const& expr)						{return regexMatches(str, Regex(expr));			}
inline StringList	regexFind(String const& str, String const& expr)						{return regexFind(str, Regex(expr));			}
inline String		regexFindFirst(String const& str, String const& expr)					{return regexFindFirst(str, Regex(expr));		}

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

inline String toString(std::filesystem::path const& p) {
	return p.string();
}

template<typename T>
constexpr inline String toString(const T& val)		{return std::to_string(val);	}
template<typename T>
constexpr inline String toWideString(const T& val)	{return std::to_wstring(val);	}

template<typename... Args>
String toString(Args const&... args)			{return Helper::concatenate<String>(args...);		}
template<typename... Args>
WideString toWideString(Args const&... args)	{return Helper::concatenate<WideString>(args...);	}

inline int					toInt(String s, size_t base = 10)				{return std::stoi(s, nullptr, base);	}
inline long					toLong(String s, size_t base = 10)				{return std::stol(s, nullptr, base);	}
inline long long			toLongLong(String s, size_t base = 10)			{return std::stoll(s, nullptr, base);	}

inline unsigned int			toUnsignedInt(String s, size_t base = 10)		{return std::stoul(s, nullptr, base);	}
inline unsigned long		toUnsignedLong(String s, size_t base = 10)		{return std::stoul(s, nullptr, base);	}
inline unsigned long long	toUnsignedLongLong(String s, size_t base = 10)	{return std::stoull(s, nullptr, base);	}

inline float		toFloat(String s)		{return std::stof(s);	}
inline double		toDouble(String s)		{return std::stod(s);	}
inline long double	toLongDouble(String s)	{return std::stold(s);	}

inline int8		toInt8(String s, size_t base = 10)		{return std::stoi(s, nullptr, base);	}
inline int16	toInt16(String s, size_t base = 10)		{return std::stoi(s, nullptr, base);	}
inline int32	toInt32(String s, size_t base = 10)		{return std::stol(s, nullptr, base);	}
inline int64	toInt64(String s, size_t base = 10)		{return std::stoll(s, nullptr, base);	}

inline uint8	toUInt8(String s, size_t base = 10)		{return std::stoul(s, nullptr, base);	}
inline uint16	toUInt16(String s, size_t base = 10)	{return std::stoul(s, nullptr, base);	}
inline uint32	toUInt32(String s, size_t base = 10)	{return std::stoul(s, nullptr, base);	}
inline uint64	toUInt64(String s, size_t base = 10)	{return std::stoull(s, nullptr, base);	}

inline float32	toFloat32(String s)		{return std::stof(s);	}
inline float64	toFloat64(String s)		{return std::stod(s);	}
inline float128	toFloat128(String s)	{return std::stold(s);	}

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

	template <typename... Args>
	void makeDirectory(Args const&... args) {
		(makeDirectory(toString(args)), ...);
	}

	constexpr String concatenatePath(String const& root, String const& path) {
		if (root.empty()) return path;
		String res = root;
		if (!path.empty()) res += "/" + path;
		return res;
	}

	constexpr String concatenatePath(String const& root, StringList const& paths) {
		String res = root;
		for(auto& path: paths) {
			if (!path.empty()) res = concatenatePath(res, path);
		}
		return res;
	}

	constexpr String concatenatePath(String const& root, StringArguments const& paths) {
		String res = root;
		for(auto& path: paths) {
			if (!path.empty()) res = concatenatePath(res, path);
		}
		return res;
	}

	namespace {
		constexpr String getPathDirectory(String const& s) {
			return s.empty() ? "/" + s : "";
		}
	}

	template <typename... Args>
	constexpr String concatenatePath(String const& root, Args const&... paths) {
		return root + (... + getPathDirectory(toString(paths)));
	}

	inline String getFileName(String const& path, bool removeExtension = false) {
		return toString(removeExtension ? fs::path(path).stem() : fs::path(path).filename());
	}

	inline String getDirectoryFromPath(String const& path) {
		return fs::path(path).remove_filename().string();
	}

	inline String getParentDirectory(String const& path) {
		return splitString(path, {'\\', '/'})[0];
	}

	inline String getChildPath(String const& path) {
		StringList dirs = splitString(path, {'\\', '/'});
		if (dirs.size() == 1) return path;
		String child = dirs[1];
		for SSRANGE(i, 2, dirs.size())
			child += "/" + dirs[i];
		return child;
	}
}

#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__) && !defined(_NO_WINDOWS_PLEASE_)
#include <windows.h>
#include <winapifamily.h>
#include <commdlg.h>
#endif

namespace System {
	int launchApp(String const& path, String const& directory = "") {
		// This is a nightmare, but the other option pops up a command prompt.
		#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__) && !defined(_NO_WINDOWS_PLEASE_)
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
		if (!proc) throw Error::FailedAction(toString("could not find '", path,"!"));
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
		#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__) && !defined(_NO_WINDOWS_PLEASE_)
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
		#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__) && !defined(_NO_WINDOWS_PLEASE_)
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
