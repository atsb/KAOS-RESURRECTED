#pragma once
 






#include <cstdint>

#define FIXSHIFT	16
#define	FIXONE		(std::int32_t(1) << FIXSHIFT)

#define SGN(x) 		((x)>0?1:-1)
 

#define ABS(x) 		((std::int16_t)(x)>0?(x):-(x))
#define LABS(x) 	((std::int32_t)(x)>0?(x):-(x))

 




using fixed = std::int32_t;
static_assert(sizeof(fixed) == 4, "DOS Q16.16 fixed must stay 32-bit");

extern fixed lshr6(fixed);
extern fixed lshl6(fixed);
extern fixed lshr8(fixed);
extern fixed lshl8(fixed);
extern std::int16_t lshr16(fixed);
extern fixed lshl16(std::int16_t);

extern fixed fixmul(fixed,fixed);
extern fixed fixdiv(fixed,fixed);
extern fixed fixdiv64(fixed a_l,fixed a_h, fixed b);
extern fixed fixdiv64shl16(fixed a, fixed b);
extern fixed fixmuldiv64(fixed a, fixed b, fixed c);
extern fixed fixsqrt(fixed a);
extern fixed fixdist(fixed dx, fixed dy);
extern std::int16_t intdist(std::int16_t dex, std::int16_t dey);

 


static inline std::int32_t kaos_asr32(std::int32_t value, unsigned bits)
{
	if (bits == 0) return value;
	if (bits >= 32) return value < 0 ? -1 : 0;
	const std::int64_t divisor = std::int64_t(1) << bits;
	const std::int64_t wide = value;
	if (wide >= 0) return static_cast<std::int32_t>(wide / divisor);
	return static_cast<std::int32_t>(-((-wide + divisor - 1) / divisor));
}

 
static inline fixed kaos_fshl(fixed v, int bits)
{
	if (bits <= 0) {
		if (bits == 0) return v;
		if (bits <= -32) return v < 0 ? -1 : 0;
		return kaos_asr32(v, static_cast<unsigned>(-bits));
	}
	return static_cast<fixed>(static_cast<std::uint32_t>(v) << bits);
}
