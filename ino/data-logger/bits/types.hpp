#ifndef BITS_TYPES_H
#define BITS_TYPES_H

using int8		= int8_t;
using int16		= int16_t;
using int32		= int32_t;
using int64		= int64_t;
using ssize		= int16;

using uint8		= uint8_t;
using uint16	= uint16_t;
using uint32	= uint32_t;
using uint64	= uint64_t;
using usize		= uint16;

using i32		= int32;
using i64		= int64;
using imax		= ssize;

using u32		= uint32;
using u64		= uint64;
using umax		= usize;

using sbyte		= u8;
using sword		= u16;
using sdword	= u32;
using sqword	= u64;

using udword	= u32;
using uqword	= u64;

using dword		= udword;
using qword		= uqword;

using eeprom_address	= word;
using avr_pin			= byte;

template <class T>	using ref	= T*;
template <class T>	using owner	= T*;

using pointer = void*;
using address = void const*;

#define PACKED [[gnu::aligned(1)]]

#endif