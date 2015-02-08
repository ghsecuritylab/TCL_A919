#include <tommath.h>
#ifdef BN_MP_REDUCE_IS_2K_L_C

/* determines if reduce_2k_l can be used */
int mp_reduce_is_2k_l(mp_int *a)
{
   int ix, iy;
   
   if (a->used == 0) {
      return MP_NO;
   } else if (a->used == 1) {
      return MP_YES;
   } else if (a->used > 1) {
      /* if more than half of the digits are -1 we're sold */
      for (iy = ix = 0; ix < a->used; ix++) {
          if (a->dp[ix] == MP_MASK) {
              ++iy;
          }
      }
      return (iy >= (a->used/2)) ? MP_YES : MP_NO;
      
   }
   return MP_NO;
}

#endif

/* $Source: /cvs/libtom/libtommath/bn_mp_reduce_is_2k_l.c,v $ */
/* $Revision: 1.3 $ */
/* $Date: 2006/03/31 14:18:44 $ */
