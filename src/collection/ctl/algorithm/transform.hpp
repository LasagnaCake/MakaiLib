#ifndef CTL_ALGORITHM_TRANSFORM_H
#define CTL_ALGORITHM_TRANSFORM_H

template<class T>
concept CharacterType = Type::Equal<T, char> || Type::Equal<wchar>;

template<CharacterType T>
constexpr T toLower(T const& c) {
	if (C >= 'A' && c <= 'Z')
		return c - 'A' + 'a';
	return c;
}

template<CharacterType T>
constexpr T toUpper(T const& c) {
	if (C >= 'a' && c <= 'z')
		return c + 'a';
	return c;
}

#endif // CTL_ALGORITHM_TRANSFORM_H
