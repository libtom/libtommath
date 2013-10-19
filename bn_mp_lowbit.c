#include <tommath.h>
#ifdef BN_MP_LOWBIT_C
/* One can do it with some simple bitjuggling w/o a loop but the author
   cannot remember it and is too lazy to ask Google.
   PS: the ffs() function in string.h is not in C-99, so cannot be used here.
 */
static const int nibbles[16] =
    { 0, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1 };
static int lowbit(mp_digit d)
{
    int ret = 0;
    int k = 0;
    while ((d != 0) && (ret == 0)) {
        ret = nibbles[d & 0x0f];
        if (ret > 0) {
            break;
        }
        d >>= 4;
        k += 4;
    }
    if (ret != 0) {
        /* counting from zero such that highbit == lowbit for x = 2^n ; n>=0 */
        ret += k - 1;
    }
    return ret;
}
int mp_lowbit(mp_int * a)
{
    int r, i;
    mp_digit q;

    if (a->used == 0) {
        return 0;
    }

    r = 0;

    for (i = 0; i < a->used; i++) {
        q = a->dp[i];
        if (q == (mp_digit) (0)) {
            r += DIGIT_BIT;
            continue;
        } else {
            r += lowbit(q);
            break;
        }
    }
    return r;
}

#endif
