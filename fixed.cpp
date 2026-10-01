#include "fixed.h"
#include <cstdint>
#include <cmath>
#include <limits>

static inline fixed fixed_sqrt_u64(std::uint64_t n)
{
    std::uint64_t res = 0;
    std::uint64_t bit = std::uint64_t(1) << 62;

    while (bit > n)
        bit >>= 2;

    while (bit != 0) {
        if (n >= res + bit) {
            n -= res + bit;
            res = (res >> 1) + bit;
        }
        else {
            res >>= 1;
        }
        bit >>= 2;
    }

    return static_cast<fixed>(res);
}

 
fixed lshr6(fixed x) { return kaos_asr32(x, 6); }
fixed lshl6(fixed x) {
    return static_cast<fixed>(static_cast<std::uint32_t>(x) << 6);
}
fixed lshr8(fixed x) { return kaos_asr32(x, 8); }
fixed lshl8(fixed x) {
    return static_cast<fixed>(static_cast<std::uint32_t>(x) << 8);
}
std::int16_t lshr16(fixed x)
{
	return static_cast<std::int16_t>(kaos_asr32(x, 16));
}

fixed lshl16(std::int16_t x)
{
	return static_cast<fixed>(static_cast<std::uint32_t>(
		static_cast<std::int32_t>(x)) << 16);
}

fixed fixmul(fixed a, fixed b)
{
    const std::int64_t p = static_cast<std::int64_t>(a) *
        static_cast<std::int64_t>(b);
     
    const std::uint64_t rounded =
        static_cast<std::uint64_t>(p) + std::uint64_t(0x8000);
    return static_cast<fixed>(static_cast<std::int32_t>(rounded >> 16));
}

fixed fixdiv(fixed a, fixed b)
{
     

    if (b == 0)
        return (a < 0) ? std::numeric_limits<fixed>::min()
        : std::numeric_limits<fixed>::max();
    return static_cast<fixed>(a / b);
}

fixed fixdiv64(fixed a_l, fixed a_h, fixed b)
{
    if (b == 0)
        return (a_h < 0) ? std::numeric_limits<fixed>::min()
        : std::numeric_limits<fixed>::max();

    const std::uint64_t raw =
        (static_cast<std::uint64_t>(static_cast<std::uint32_t>(a_h)) << 32) |
        static_cast<std::uint32_t>(a_l);
    const std::int64_t n = static_cast<std::int64_t>(raw);
     
    return static_cast<fixed>(n / b);
}

fixed fixdiv64shl16(fixed a, fixed b)
{
    if (b == 0)
        return (a < 0) ? std::numeric_limits<fixed>::min()
        : std::numeric_limits<fixed>::max();

     
     
    const std::int64_t n = static_cast<std::int64_t>(a) * std::int64_t(1 << 16);
    return static_cast<fixed>(n / b);
}

fixed fixmuldiv64(fixed a, fixed b, fixed c)
{
    if (c == 0)
        return ((a < 0) ^ (b < 0)) ? std::numeric_limits<fixed>::min()
        : std::numeric_limits<fixed>::max();

    const std::int64_t p = static_cast<std::int64_t>(a) *
        static_cast<std::int64_t>(b);
     
     
    const std::int64_t rounded = p + std::int64_t(0x8000);
    return static_cast<fixed>(rounded / c);
}

fixed fixsqrt(fixed a)
{
     









    const std::uint64_t scaled =
        static_cast<std::uint64_t>(static_cast<std::uint32_t>(a)) << 16;
    return fixed_sqrt_u64(scaled);
}

fixed fixdist(fixed dex, fixed dey)
{
     

    if (dex != std::numeric_limits<fixed>::min() &&
        (dex < 0 ? -static_cast<std::int64_t>(dex) > (255LL << 16)
                 : static_cast<std::int64_t>(dex) > (255LL << 16)))
        dex = 255L << 16;
    if (dey != std::numeric_limits<fixed>::min() &&
        (dey < 0 ? -static_cast<std::int64_t>(dey) > (255LL << 16)
                 : static_cast<std::int64_t>(dey) > (255LL << 16)))
        dey = 255L << 16;

     
     
     
     
     
    const std::uint64_t x = static_cast<std::int64_t>(dex);
    const std::uint64_t y = static_cast<std::int64_t>(dey);
    const std::uint32_t xsq = static_cast<std::uint32_t>((x * x + 0x8000u) >> 16);
    const std::uint32_t ysq = static_cast<std::uint32_t>((y * y + 0x8000u) >> 16);

    std::uint32_t sum = xsq + ysq;
    if (sum < xsq)
        sum = 0x7fffffffu;

    return fixed_sqrt_u64(static_cast<std::uint64_t>(sum) << 16);
}

std::int16_t intdist(std::int16_t dex, std::int16_t dey)
{
     


    if (dex != std::numeric_limits<std::int16_t>::min() &&
        (dex < 0 ? -static_cast<std::int32_t>(dex) > 255
                 : static_cast<std::int32_t>(dex) > 255))
        dex = 255;
    if (dey != std::numeric_limits<std::int16_t>::min() &&
        (dey < 0 ? -static_cast<std::int32_t>(dey) > 255
                 : static_cast<std::int32_t>(dey) > 255))
        dey = 255;

     



    const std::uint32_t ux = static_cast<std::uint32_t>(
        static_cast<std::int32_t>(dex));
    const std::uint32_t uy = static_cast<std::uint32_t>(
        static_cast<std::int32_t>(dey));
    std::uint32_t cx = ux * ux + uy * uy;
    std::uint32_t ax = 0;
    std::uint32_t bx = 0x40000000u;

    do {
        if (static_cast<std::int32_t>(cx) >=
            static_cast<std::int32_t>(bx)) {
            std::uint32_t dx = cx - bx;
            if (static_cast<std::int32_t>(dx) >=
                static_cast<std::int32_t>(ax)) {
                cx = dx - ax;
                ax >>= 1;
                ax |= bx;
                bx >>= 2;
                continue;
            }
        }
        ax >>= 1;
        bx >>= 2;
    } while (bx != 0);

    const std::uint16_t low = static_cast<std::uint16_t>(ax);
    if (low <= static_cast<std::uint16_t>(
                   std::numeric_limits<std::int16_t>::max()))
        return static_cast<std::int16_t>(low);
    return static_cast<std::int16_t>(static_cast<std::int32_t>(low) - 0x10000);
}
