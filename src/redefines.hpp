#ifndef STD_REDEFINITIONS_H
#define STD_REDEFINITIONS_H

typedef std::string			String;
typedef	std::vector<String>	StringList;

template <typename K, typename V>
using StdMap = std::unordered_map<K, V>;

#endif // STD_REDEFINITIONS_H
