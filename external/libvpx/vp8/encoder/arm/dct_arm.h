


#ifndef DCT_ARM_H
#define DCT_ARM_H

#if HAVE_ARMV6
extern prototype_fdct(vp8_short_walsh4x4_armv6);

#if !CONFIG_RUNTIME_CPU_DETECT
#undef  vp8_fdct_walsh_short4x4
#define vp8_fdct_walsh_short4x4 vp8_short_walsh4x4_armv6
#endif
#endif

#if HAVE_ARMV7
extern prototype_fdct(vp8_short_fdct4x4_neon);
extern prototype_fdct(vp8_short_fdct8x4_neon);
extern prototype_fdct(vp8_fast_fdct4x4_neon);
extern prototype_fdct(vp8_fast_fdct8x4_neon);
extern prototype_fdct(vp8_short_walsh4x4_neon);

#if !CONFIG_RUNTIME_CPU_DETECT
#undef  vp8_fdct_short4x4
#define vp8_fdct_short4x4 vp8_short_fdct4x4_neon

#undef  vp8_fdct_short8x4
#define vp8_fdct_short8x4 vp8_short_fdct8x4_neon

#undef  vp8_fdct_fast4x4
#define vp8_fdct_fast4x4 vp8_fast_fdct4x4_neon

#undef  vp8_fdct_fast8x4
#define vp8_fdct_fast8x4 vp8_fast_fdct8x4_neon

#undef  vp8_fdct_walsh_short4x4
#define vp8_fdct_walsh_short4x4 vp8_short_walsh4x4_neon
#endif

#endif

#endif
