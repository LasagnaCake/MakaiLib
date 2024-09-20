#ifndef MAKAILIB_STD_CPPTYPES_H
#define MAKAILIB_STD_CPPTYPES_H

#define NAMEOF(TYPEID) abi::__cxa_demangle((TYPEID).name(),0,0,NULL)

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
#include "ctl/typetraits/traits.hpp"
#include "ctl/ctypes.hpp"

using String = std::string;

usingWideString = std::wstring;

template<typename T>
using Arguments = std::initializer_list<T>;

template<typename T>
using List = std::vector<T>;

template<typename T>
using LinkedList = std::list<T>;

template<typename K, typename V>
using HashMap = std::unordered_map<K, V>;

template<typename K, typename V>
using SortedMap = std::map<K, V>;

template<typename A, typename B>
using Pair = std::pair<A, B>;

template<typename T>
using Entry = Pair<String, T>;

template<typename T>
using Dictionary = HashMap<String, T>;

template<typename T>
using SortedDictionary = SortedMap<String, T>;

template<typename T>
using Function = std::function<T>;

template<typename T>
using Operation = Function<T(T const&)>;

template<typename T>
using Procedure = Function<void(T)>;

template<typename... Types>
using Poly = variant<Types...>;

template<typename T, size_t LEN = std::dynamic_extent>
using Span = span<T, LEN>;

template<typename... Types>
using Tuple = std::tuple<Types...>;

typedef std::strong_ordering	StrongOrder;
typedef std::weak_ordering		WeakOrder;
typedef std::partial_ordering	PartialOrder;

typedef List<String>			StringList;
typedef Arguments<String>		StringArguments;
typedef Pair<String, String>	StringPair;

typedef std::thread			Thread;

#endif // MAKAILIB_STD_CPPTYPES_H
