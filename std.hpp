#pragma once
#ifndef __NICSTD97__
#define __NICSTD97__

#include <cstdint>
#include <cstddef>
#include "platform_compat.hpp"

using byte = std::uint8_t;
using word = std::uint16_t;
using dword = std::uint32_t;
using pointer = void*;

extern const char NicMark[];
extern const std::uint32_t SerialNo;
extern void error(const char *err_fnc,const char *err_on);
extern char ShutDown;

#endif
