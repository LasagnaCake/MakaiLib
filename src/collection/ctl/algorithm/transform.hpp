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
	if (c >= 'A' && c <= 'Z')
		return c - ('a' - 'A');
	return c;
}

template<ASCIIType T>
constexpr T toUpper(T const& c) {
	if (c >= 'a' && c <= 'z')
		return c + ('a' - 'A');
	return c;
}

#endif // CTL_ALGORITHM_TRANSFORM_H
