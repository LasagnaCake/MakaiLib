#ifndef CTL_IO_BUFFER_H
#define CTL_IO_BUFFER_H

#include "../namespace.hpp"

CTL_NAMESPACE_BEGIN

using DataBuffer	= std::streambuf;
using FileBuffer	= std::filebuf;
using StringBuffer	= std::stringbuf;

struct MemoryBuffer: public DataBuffer {
	MemoryBuffer(char* const& data, usize const& size) {
		setg(data, data, data + size);
		setp(data, data + size);
	}
};

CTL_NAMESPACE_END

#endif // CTL_IO_BUFFER_H
