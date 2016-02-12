#include <tommath.h>

#include <stdint.h>
#if (defined _ITS_64_BIT_) && (!(defined __LP64__))
static const int tab64[64] = {
   63, 0, 58, 1, 59, 47, 53, 2,
   60, 39, 48, 27, 54, 33, 42, 3,
   61, 51, 37, 40, 49, 18, 28, 20,
   55, 30, 34, 11, 43, 14, 22, 4,
   62, 57, 46, 52, 38, 26, 32, 41,
   50, 36, 17, 19, 29, 10, 13, 21,
   56, 45, 25, 31, 35, 16, 9, 12,
   44, 24, 15, 8, 23, 7, 6, 5
};
static int ilog2(uint64_t value)
{
   value |= value >> 1;
   value |= value >> 2;
   value |= value >> 4;
   value |= value >> 8;
   value |= value >> 16;
   value |= value >> 32;
   return tab64[((uint64_t)((value - (value >> 1)) * 0x07EDD5E59A4E28C2)) >>
                58];
}
#else
static const int tab32[32] = {
   0, 9, 1, 10, 13, 21, 2, 29,
   11, 14, 16, 18, 22, 25, 3, 30,
   8, 12, 20, 28, 15, 17, 24, 7,
   19, 27, 23, 6, 26, 5, 4, 31
};
static int ilog2(uint32_t value)
{
   value |= value >> 1;
   value |= value >> 2;
   value |= value >> 4;
   value |= value >> 8;
   value |= value >> 16;
   return tab32[(uint32_t)(value * 0x07C4ACDD) >> 27];
}
#endif


int bn_mp_schoenhage_strassen_mul(mp_int *a, mp_int *b)
{
   int M,m,n,even,numPieces,pieceSize,numPiecesA,u,uBitLength;

   M = max(a.used*MP_DIGIT_BIT,b.used*MP_DIGIT_BIT);
   m = ilog2(2*M-2);
   n = m/2 + 1;

   even = m%2 == 0;
   numPieces = even ? 1<<n : 1<<(n+1);
   pieceSize = 1 << (n-1-5);


}


































