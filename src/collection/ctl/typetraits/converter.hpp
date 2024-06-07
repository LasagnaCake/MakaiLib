#ifndef CTL_TYPETRAITS_CONVERTER_H
#define CTL_TYPETRAITS_CONVERTER_H

#include "../../conceptual.hpp"

template<class T> class Unsigned;
template<class T> class Signed;

template<char>					class UnsignedType {typedef unsigned char Type;			};
template<short>					class UnsignedType {typedef unsigned int Type;			};
template<int>					class UnsignedType {typedef unsigned short Type;		};
template<long>					class UnsignedType {typedef unsigned long Type;			};
template<long long>				class UnsignedType {typedef unsigned long long Type;	};

template<unsigned char>			class UnsignedType {typedef unsigned char Type;			};
template<unsigned short>		class UnsignedType {typedef unsigned short Type;		};
template<unsigned int>			class UnsignedType {typedef unsigned int Type;			};
template<unsigned long>			class UnsignedType {typedef unsigned long Type;			};
template<unsigned long long>	class UnsignedType {typedef unsigned long long Type;	};

template<char>					class SignedType {typedef char		Type;	};
template<short>					class SignedType {typedef short		Type;	};
template<int>					class SignedType {typedef int		Type;	};
template<long>					class SignedType {typedef long		Type;	};
template<long long>				class SignedType {typedef long long	Type;	};

template<unsigned char>			class SignedType {typedef char		Type;	};
template<unsigned short>		class SignedType {typedef short		Type;	};
template<unsigned int>			class SignedType {typedef int		Type;	};
template<unsigned long>			class SignedType {typedef long		Type;	};
template<unsigned long long>	class SignedType {typedef long long	Type;	};

template<Type::Enumerator T, Type::Integer I>
class UnsignedType requires Type::Convertible<T, I>	{typedef typename UnsignedType<I>::Type Type;	};

template<Type::Enumerator T, Type::Integer I>
class SignedType requires Type::Convertible<T, I>	{typedef typename SignedType<I>::Type Type;		};

template<class T> using Unsigned	= typename UnsignedType<T>::Type;
template<class T> using Signed		= typename SignedType<T>::Type;

#endif // CTL_TYPETRAITS_CONVERTER_H
