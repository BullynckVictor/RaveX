#pragma once
#include <cinttypes>

namespace rv
{
	namespace types
	{
		typedef int8_t				int8;
		typedef int16_t				int16;
		typedef int32_t				int32;
		typedef int64_t				int64;

		typedef uint8_t				uint8;
		typedef uint16_t			uint16;
		typedef uint32_t			uint32;
		typedef uint64_t			uint64;

		typedef int8				i8;
		typedef int16				i16;
		typedef int32				i32;
		typedef int64				i64;

		typedef uint8				u8;
		typedef uint16				u16;
		typedef uint32				u32;
		typedef uint64				u64;

		typedef u8					byte;

		typedef unsigned char		uchar;
		typedef unsigned short		ushort;
		typedef unsigned int		uint;
		typedef unsigned long		ulong;
		typedef unsigned long long	ulonglong;
	}
	using namespace types;

	template<size_t S>
	struct of_size
	{
	};

	template<> struct of_size<1> { using type = u8; };
	template<> struct of_size<2> { using type = u16; };
	template<> struct of_size<4> { using type = u32; };
	template<> struct of_size<8> { using type = u64; };
}

#ifndef RV_NO_TYPES
using namespace rv::types;
#endif