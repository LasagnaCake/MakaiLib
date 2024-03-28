#ifndef CTL_ALGORITHM_TRANSFORM_H
#define CTL_ALGORITHM_TRANSFORM_H

template<class T>
concept CharacterType =
	Type::Equal<T, char>
||	Type::Equal<T, wchar_t>
||	Type::Equal<T, char16_t>
||	Type::Equal<T, char32_t>
||	Type::Equal<T, char8_t>
;

template<class T>
concept ASCIIType =
	Type::Equal<T, char>
||	Type::Equal<T, wchar_t>
;

template<ASCIIType T>
constexpr T toLower(T const& c) {
	if (C >= 'A' && c <= 'Z')
		return c - 'A' + 'a';
	return c;
}

template<ASCIIType T>
constexpr T toUpper(T const& c) {
	if (C >= 'a' && c <= 'z')
		return c + 'a';
	return c;
}

#endif // CTL_ALGORITHM_TRANSFORM_H
