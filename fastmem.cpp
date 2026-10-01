#include "std.hpp"
#include "fastmem.hpp"
#include <cstring>
#include <cstdint>

void fmove(void *dest, void *sorg, word bytes)
{
    if (!bytes) return;
    std::memcpy(dest, sorg, bytes);
}

void fwmove(void *dest, void *sorg, word bytes)
{
    if (!bytes) return;
    std::memcpy(dest, sorg, bytes);
}

void fdmove(void *dest, void *sorg, word bytes)
{
    if (!bytes) return;
    std::memcpy(dest, sorg, bytes);
}

void fowmove(void *dest, void *sorg, word bytes)
{
    if (!bytes) return;
    std::memmove(dest, sorg, bytes);
}

void fodmove(void *dest, void *sorg, word bytes)
{
    if (!bytes) return;
    std::memmove(dest, sorg, bytes);
}

void fwfill(void *dest, word val, word bytes)
{
    auto *d = static_cast<std::uint8_t *>(dest);
    const std::uint8_t lo = static_cast<std::uint8_t>(val);
    const std::uint8_t hi = static_cast<std::uint8_t>(val >> 8);

    while (bytes >= 2) {
        d[0] = lo;
        d[1] = hi;
        d += 2;
        bytes = static_cast<word>(bytes - 2);
    }

    if (bytes)
        d[0] = lo;
}

void fdfill(void *dest, long val, word bytes)
{
    auto *d = static_cast<std::uint8_t *>(dest);
    const std::uint32_t v = static_cast<std::uint32_t>(val);

    while (bytes >= 4) {
        d[0] = static_cast<std::uint8_t>(v);
        d[1] = static_cast<std::uint8_t>(v >> 8);
        d[2] = static_cast<std::uint8_t>(v >> 16);
        d[3] = static_cast<std::uint8_t>(v >> 24);
        d += 4;
        bytes = static_cast<word>(bytes - 4);
    }

    while (bytes) {
        *d++ = static_cast<std::uint8_t>(v);
        bytes = static_cast<word>(bytes - 1);
    }
}

char fwcomp(const void *s1, const void *s2, word len)
{
    return std::memcmp(s1, s2, len) == 0;
}

char fdcomp(const void *s1, const void *s2, word len)
{
    return std::memcmp(s1, s2, len) == 0;
}
