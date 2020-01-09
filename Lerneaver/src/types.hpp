/**
	\file

	Module that describes main types.
*/

#ifndef HEADER_TYPES
#define HEADER_TYPES

#include <cstdint>
#include <cstddef>
#include <string>

using TYPE_BYTE = std::uint8_t;
using TYPE_WORD = std::uint16_t;
using TYPE_DWORD = std::uint32_t;
using TYPE_QWORD = std::uint64_t;

using TYPE_SIZE=std::size_t;

#if defined PLATFORM_WINDOWS
	using TYPE_FILESYSTEMPATH = std::wstring;
#elif defined PLATFORM_UNIX
	using TYPE_FILESYSTEMPATH = std::string;
#elif defined PLATFORM_MACOS

#endif

#endif