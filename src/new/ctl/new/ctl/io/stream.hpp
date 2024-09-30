#ifndef CTL_IO_STREAM_H
#define CTL_IO_STREAM_H

#include <stream>
#include "../namespace.hpp"
#include "buffer.hpp"

CTL_NAMESPACE_BEGIN

template<class T>
using OutputStream	= std::basic_ostream<T>;
template<class T>
using InputStream	= std::basic_istream<T>;

using FileStream = std::fstream;
using DataStream = std::iostream;

struct MemoryStream: DataStream {
	MemoryBuffer buffer;

	MemoryStream(char* const& data, usize const& size):
		buffer(data, size),
		DataStream(buffer) {}
};

template<class TData>
struct Streamable {
	typedef InputStream<TData>	InputStreamType;
	typedef OutputStream<TData>	OutputStreamType;
};

CTL_NAMESPACE_END

#endif // CTL_IO_STREAM_H
