#ifndef HELPING_HAND_H
#define HELPING_HAND_H

#ifdef ENABLE_DEBUG_OUTPUT_
#include <iostream>
#endif
#include <syncstream>
#include <string>
#include <vector>
#include <list>
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
#include "ctl/ctypes.hpp"
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
			std::list,
			std::map,
			std::unordered_map,
			std::string,
			std::wstring,
			std::initializer_list,
			std::pair,
			std::function,
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

	template<typename T>
	using LinkedList = list<T>;

	template<typename K, typename V>
	using HashMap = unordered_map<K, V>;

	template<typename K, typename V>
	using SortedMap = map<K, V>;

	template<typename A, typename B>
	using Pair = pair<A, B>;

	template<typename T>
	using Entry = Pair<String, T>;

	template<typename T>
	using Dictionary = HashMap<String, T>;

	template<typename T>
	using SortedDictionary = SortedMap<String, T>;

	template<typename T>
	using Function = function<T>;

	template<typename T>
	using Operation = Function<T(T const&)>;

	template<typename T>
	using Procedure = Function<void(T)>;

	template<typename... Types>
	using Poly = variant<Types...>;

	/*template<typename T>
	using Nullable = optional<T>;*/

	template<typename T, size_t LEN>
	using Span = span<T, LEN>;

	template<typename... Types>
	using Tuple = tuple<Types...>;

	struct Discard {
		template<typename T>constexpr T&& operator=(T&& v)				{return v;}
		template<typename T>constexpr T const& operator=(T const& v)	{return v;}
	} _;

	typedef std::strong_ordering	StrongOrder;
	typedef std::weak_ordering		WeakOrder;
	typedef std::partial_ordering	PartialOrder;

	typedef List<String>			StringList;
	typedef Arguments<String>		StringArguments;
	typedef Pair<String, String>	StringPair;

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
	constexpr List<T> getKeys(SortedMap<T, T2> const& lst) {
		List<T> keys;
		keys.reserve(lst.size());
		for (auto i = lst.begin(); i != lst.end(); i++) {
			keys.push_back(i->first);
		}
		return keys;
	}

	template<typename T, typename T2>
	constexpr List<T2> getValues(SortedMap<T, T2> const& lst) {
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
	String floatString(T const& val, size_t const& precision) {
		return std::format(std::format("{{:.{}f}}", precision), val);
	}
	#else
	template<Type::Number T>
	String floatString(T const& val, size_t const& precision) {
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
	using NumberFormat = Pair<T, size_t>;

	template<Type::Number T>
	String floatString(NumberFormat<T> const& fmt) {
		return floatString(fmt->first, fmt->second);
	}

	constexpr String padString(String str, char const& chr, size_t const& width, bool const& left = true) {
		size_t const strsz = str.size();
		if(width > strsz)
			str.insert(left ? 0 : (strsz-1), width - strsz, chr);
		return str;
	}

	template<Type::Number T>
	constexpr String formatNumber(T const& val, size_t const& leading) {
		return padString(std::to_string(val), '0', leading);
	}

	template<Type::Number T>
	String floatString(T const& val, size_t const& precision, size_t const& leading) {
		return padString(floatString(val, precision), '0', leading);
	}

	constexpr StringList splitString(String const& str, char const& sep) {
		StringList res;
		if (!sep) return StringList{str};
		res.reserve(std::count(str.begin(), str.end(), sep) + 1);
		size_t pos = 0, last = 0;
		while((pos = str.find(sep, last)) != str.npos) {
			res.push_back(last != pos ? str.substr(last, pos-last) : "");
			last = pos+1;
		}
		res.push_back(str.substr(last));
		return res;
	}

	constexpr StringList splitString(String const& str, Arguments<char> const& seps) {
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

	constexpr StringPair splitStringAtFirst(String const& str, char const& sep) {
		auto pos = str.find(sep);
		if (pos == str.npos)
			return StringPair(str, "");
		return StringPair(
			str.substr(0, pos),
			str.substr(pos + 1)
		);
	}

	constexpr StringPair splitStringAtFirst(String const& str, Arguments<char> const& seps) {
		StringPair res = StringPair(str, ""), buf;
		for (char const& sep: seps) {
			buf = splitStringAtFirst(str, sep);
			if (buf.first.size() < res.first.size())
				res = buf;
		}
		return res;
	}

	constexpr StringPair splitStringAtLast(String const& str, char const& sep) {
		auto pos = str.rfind(sep);
		if (pos == str.npos)
			return StringPair("", str);
		return StringPair(
			str.substr(0, pos),
			str.substr(pos + 1)
		);
	}

	constexpr StringPair splitStringAtLast(String const& str, Arguments<char> const& seps) {
		StringPair res = StringPair("", str), buf;
		for (char const& sep: seps) {
			buf = splitStringAtLast(str, sep);
			if (buf.second.size() < res.second.size())
				res = buf;
		}
		if (res.first.empty()) return StringPair(str, "");
		return res;
	}

	constexpr String toLower(String data) {
		std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c){return std::tolower(c);});
		return data;
	}

	constexpr String replace(String str, char const& c, char const& nc) {
		for (char& sc: str)
			if (sc == c) sc = nc;
		return str;
	}

	constexpr String replace(String str, List<char> const& chars, char const& nc) {
		for (char const& c: chars)
			str = replace(str, c, nc);
		return str;
	}

	constexpr String replace(String str, Arguments<char> const& chars, char const& nc) {
		for (char const& c: chars)
			str = replace(str, c, nc);
		return str;
	}

	struct CharacterReplacement {
		List<char>	targets;
		char		replacement;
	};

	constexpr String replace(String str, CharacterReplacement const& rep) {
		for (char const& c: rep.targets)
			str = replace(str, c, rep.replacement);
		return str;
	}

	constexpr String replace(String str, List<CharacterReplacement> const& reps) {
		for (CharacterReplacement const& rep: reps)
			str = replace (str, rep);
		return str;
	}

	constexpr String replace(String str, Arguments<CharacterReplacement> const& reps) {
		for (CharacterReplacement const& rep: reps)
			str = replace (str, rep);
		return str;
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

	template<Type::Integer T>
	constexpr T reflect(T const& data) {
		T result = 0;
		constexpr size_t BIT_WIDTH = sizeof(T) * 8;
		for (size_t i = 0; i < BIT_WIDTH; i++)
			if ((data >> i) & 1) result |= (T(1) << (BIT_WIDTH-1-i));
		return (result);
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
using Helper::LinkedList;
using Helper::HashMap;
using Helper::SortedMap;
using Helper::StringList;
using Helper::StringPair;
using Helper::StringArguments;
using Helper::Arguments;
using Helper::Dictionary;
using Helper::SortedDictionary;
using Helper::Pair;
using Helper::Entry;
using Helper::Tuple;
using Helper::Function;
using Helper::Operation;
using Helper::Procedure;
using Helper::Poly;
//using Helper::Nullable;
using Helper::Regex;
using Helper::Span;
using Helper::Thread;

using Helper::_;

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
constexpr String toString(Args const&... args)			{return Helper::concatenate<String>(args...);		}
template<typename... Args>
constexpr WideString toWideString(Args const&... args)	{return Helper::concatenate<WideString>(args...);	}

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

	enum class PathSeparator: char {
		PS_POSIX	= '/',
		PS_WINDOWS	= '\\'
	};

	#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__) && !defined(_NO_WINDOWS_PLEASE_)
	constexpr PathSeparator SEPARATOR = PathSeparator::PS_WINDOWS;
	#else
	constexpr PathSeparator SEPARATOR = PathSeparator::PS_POSIX;
	#endif

	bool exists(String const& path) {
		return fs::exists(path);
	}

	bool isDirectory(String const& dir) {
		return fs::is_directory(dir);
	}

	constexpr String standardizePath(String const& path, PathSeparator const& sep) {
		return replace(path, {'\\','/'}, (char)sep);
	}

	constexpr String standardizePath(String const& path) {
		return standardizePath(path, SEPARATOR);
	}

	void makeDirectory(String const& dir) {
		if (dir.empty()) return;
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
			if (s.empty()) return "";
			return (s[0] == '/' ? "" : "/") + s;
		}
	}

	template <typename... Args>
	constexpr String concatenatePath(String const& root, Args const&... paths) {
		return root + (... + getPathDirectory(toString(paths)));
	}

	inline String getFileName(String const& path, bool removeExtension = false) {
		/*String result = splitStringAtLast(path, {'\\', '/'}).second;
		if (removeExtension)
			return splitStringAtLast(result, '.').first;
		return result;*/
		return toString(removeExtension ? fs::path(path).stem() : fs::path(path).filename());
	}

	constexpr String getFileExtension(String const& path) {
		return splitStringAtLast(path, '.').second;
	}

	constexpr String getParentDirectory(String const& path) {
		StringPair splitPath = splitStringAtFirst(path, {'\\', '/'});
		if (!splitPath.second.empty()) return splitPath.first;
		return "";
	}

	inline String getDirectoryFromPath(String const& path) {
		return fs::path(path).remove_filename().string();
	}

	constexpr String getChildPath(String const& path) {
		StringPair dirs = splitStringAtFirst(path, {'\\', '/'});
		if (dirs.second.empty())
			return path;
		return dirs.second;
	}

	struct FileTree {
		struct Entry {
			constexpr Entry(String const& name, String const& path):
				name(name),
				path(path)
			{}

			constexpr Entry(String const& name, String const& path, List<Entry>	children):
				name(name),
				path(path),
				children(children),
				folder(true)
			{}

			constexpr Entry(Entry const& other):
				name(other.name),
				path(other.path),
				children(other.children),
				folder(other.folder)
			{}

			/*constexpr*/ Entry& forEach(Operation<Entry const&> const& op) {
				for (Entry& e: children)
					op(e);
				return *this;
			}

			/*constexpr*/ Entry const& forEach(Operation<Entry const&> const& op) const {
				for (Entry const& e: children)
					op(e);
				return *this;
			}

			constexpr StringList getAllFiles() const {
				if (!folder) return StringList{path};
				StringList files;
				for (Entry const& e: children) {
					if (!e.folder)
						files.push_back(e.path);
					else {
						StringList subfiles = e.getAllFiles();
						files.insert(files.end(), subfiles.begin(), subfiles.end());
					}
				}
				return files;
			}

			constexpr List<Entry> getChildren() const {return children;}

			constexpr operator List<Entry>() const	{return getChildren();	}
			constexpr operator String() const		{return path;			}

			constexpr bool isFolder() const {return folder;}

			String const name;
			String const path;

		private:
			List<Entry>	children	= List<Entry>();
			bool const	folder		= false;
		};

		FileTree(String const& path): tree(getStructure(path)) {}

		constexpr FileTree(Entry const& entry): tree(entry) {}

		constexpr FileTree(FileTree const& other): tree(other.tree) {}

		constexpr operator Entry() const {return tree;}

		static Entry getStructure(String const& path) {
			if (!exists(path))
				throw Error::InvalidValue("Path does not exist!");
			if (!isDirectory(path)) return Entry(getFileName(path), path);
			return Entry(
				fs::path(path).stem().string(),
				path,
				getFolderContents(path)
			);
		}

		static List<Entry> getFolderContents(String const& folder) {
			if (!isDirectory(folder)) return List<Entry>();
			List<Entry> entries;
			for (auto const& e: fs::directory_iterator(folder)) {
				String name = (e.is_directory()) ? e.path().stem().string() : e.path().filename().string();
				String path = concatenatePath(folder, name);
				if (e.is_directory())
					entries.push_back(Entry(name, path, getFolderContents(path)));
				else
					entries.push_back(Entry(name, path));
			}
			return entries;
		}

		Entry tree;
	};
}

#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__) && !defined(_NO_WINDOWS_PLEASE_)
#include <windows.h>
#include <winapifamily.h>
#include <commdlg.h>
#endif

namespace System {
	String sanitized(String arg) {
		arg = regexReplace(arg, "\\\\+", "\\\\");
		#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__) && !defined(_NO_WINDOWS_PLEASE_)
		arg = regexReplace(arg, "\\\\+\"", "\\\"");
		return "\"" + arg + "\"";
		#else
		arg = regexReplace(arg, "\\\\+'", "\\'");
		arg = regexReplace(arg, "'", "\\'");
		return "'" + arg + "'";
		#endif
	}

	int launchApp(String const& path, String const& directory = "", StringList args = StringList()) {
		if (!FileSystem::exists(path))
			throw Error::InvalidValue(
				"File [" + path + "] does not exist!",
				__FILE__,
				toString(__LINE__),
				"System::launchApp"
			);
		String prgArgs = "";
		if (!args.empty())
			for (String const& arg: args)
				prgArgs += sanitized(arg) + " ";
		#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__) && !defined(_NO_WINDOWS_PLEASE_)
		prgArgs = sanitized(path) + (args.empty() ? "" : (" " + prgArgs));
		// This is a nightmare, but the other option pops up a command prompt.
		STARTUPINFO sInfo;
		PROCESS_INFORMATION pInfo;
		memset(&sInfo, 0, sizeof(sInfo));
		sInfo.cb = sizeof(sInfo);
		memset(&pInfo, 0, sizeof(pInfo));
		auto proc = CreateProcess(
			path.c_str(),
			(LPSTR)prgArgs.c_str(),
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
		if (!args.empty())
			return system(path + " " + prgArgs);
		return system(path);
		#endif
	}

	String openFileDialog(String filter = "All\0*.*\0") {
		#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__) && !defined(_NO_WINDOWS_PLEASE_)
		OPENFILENAME ofn;
		char szFile[260] = {0};
		memset(&ofn, 0, sizeof(ofn));
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
		char szFile[260] = {0};
		memset(&ofn, 0, sizeof(ofn));
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
