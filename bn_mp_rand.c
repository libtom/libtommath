#include "tommath_private.h"
#ifdef BN_MP_RAND_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* First the OS-specific special cases
 * - *BSD
 * - Windows
 */
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
#  define MP_ARC4RANDOM
#endif

#if defined(_WIN32) || defined(_WIN32_WCE)
#define MP_WIN_CSP

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#ifdef _WIN32_WCE
#define UNDER_CE
#define ARM
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincrypt.h>

static HCRYPTPROV hProv = 0;

static void s_cleanup_win_csp(void)
{
   CryptReleaseContext(hProv, 0);
   hProv = 0;
}

static int s_read_win_csp(void *p, size_t n)
{
   if (hProv == 0) {
      if (!CryptAcquireContext(&hProv, NULL, MS_DEF_PROV, PROV_RSA_FULL,
                               (CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET)) &&
          !CryptAcquireContext(&hProv, NULL, MS_DEF_PROV, PROV_RSA_FULL,
                               CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET | CRYPT_NEWKEYSET)) {
         hProv = 0;
         return MP_ERR;
      }
      atexit(s_cleanup_win_csp);
   }
   return CryptGenRandom(hProv, (DWORD)n, (BYTE *)p) == TRUE ? MP_OKAY : MP_ERR;
}
#endif /* WIN32 */

#if !defined(MP_WIN_CSP) && defined(__linux__) && defined(__GLIBC_PREREQ)
#if __GLIBC_PREREQ(2, 25)
#define MP_GETRANDOM
#include <sys/random.h>
#include <errno.h>

static int s_read_getrandom(void *p, size_t n)
{
   char *q = (char *)p;
   while (n > 0) {
      ssize_t ret = getrandom(q, n, 0);
      if (ret < 0) {
         if (errno == EINTR) {
            continue;
         }
         return MP_ERR;
      }
      q += ret;
      n -= (size_t)ret;
   }
   return MP_OKAY;
}
#endif
#endif

/* We assume all platforms besides windows provide "/dev/urandom".
 * In case yours doesn't, define MP_NO_DEV_URANDOM at compile-time.
 */
#if !defined(MP_WIN_CSP) && !defined(MP_NO_DEV_URANDOM)
#ifndef MP_DEV_URANDOM
#define MP_DEV_URANDOM "/dev/urandom"
#endif
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

static int s_read_dev_urandom(void *p, size_t n)
{
   int fd;
   char *q = (char *)p;

   do {
      fd = open(MP_DEV_URANDOM, O_RDONLY);
   } while ((fd == -1) && (errno == EINTR));
   if (fd == -1) return MP_ERR;

   while (n > 0) {
      ssize_t ret = read(fd, p, n);
      if (ret < 0) {
         if (errno == EINTR) {
            continue;
         }
         close(fd);
         return MP_ERR;
      }
      q += ret;
      n -= (size_t)ret;
   }

   close(fd);
   return MP_OKAY;
}
#endif

#if defined(MP_PRNG_ENABLE_LTM_RNG)
unsigned long (*ltm_rng)(unsigned char *out, unsigned long outlen, void (*callback)(void));
void (*ltm_rng_callback)(void);

static int s_read_ltm_rng(void *p, size_t n)
{
   unsigned long ret;
   if (ltm_rng == NULL) return MP_ERR;
   ret = ltm_rng(p, n, ltm_rng_callback);
   if (ret != n) return MP_ERR;
   return MP_OKAY;
}
#endif

static int s_mp_rand_source_platform(void *p, size_t n)
{
   int ret = MP_ERR;

#if defined(MP_ARC4RANDOM)
   arc4random_buf(p, n);
   return MP_OKAY;
#endif

#if defined(MP_WIN_CSP)
   ret = s_read_win_csp(p, n);
   if (ret == MP_OKAY) return ret;
#else

#if defined(MP_GETRANDOM)
   ret = s_read_getrandom(p, n);
   if (ret == MP_OKAY) return ret;
#endif
#if defined(MP_DEV_URANDOM)
   ret = s_read_dev_urandom(p, n);
   if (ret == MP_OKAY) return ret;
#endif

#endif /* MP_WIN_CSP */

#if defined(MP_PRNG_ENABLE_LTM_RNG)
   ret = s_read_ltm_rng(p, n);
   if (ret == MP_OKAY) return ret;
#endif

   return ret;
}

static int (*s_rand_source)(void *, size_t) = s_mp_rand_source_platform;

void mp_rand_source(int (*get)(void *out, size_t size))
{
   s_rand_source = get;
}

/* makes a pseudo-random int of a given size */
int mp_rand_digit(mp_digit *r)
{
   int ret = s_rand_source(r, sizeof(mp_digit));
   *r &= MP_MASK;
   return ret;
}

int mp_rand(mp_int *a, int digits)
{
   int ret, i;

   mp_zero(a);

   if (digits <= 0) {
      return MP_OKAY;
   }

   if ((ret = mp_grow(a, digits)) != MP_OKAY) {
      return ret;
   }

   if ((ret = s_rand_source(a->dp, (size_t)digits * sizeof(mp_digit))) != MP_OKAY) {
      return ret;
   }

   /* TODO: We ensure that the highest digit is nonzero. Should this be removed? */
   while ((a->dp[digits - 1] & MP_MASK) == 0) {
      if ((ret = s_rand_source(a->dp + digits - 1, sizeof(mp_digit))) != MP_OKAY) {
         return ret;
      }
   }

   a->used = digits;
   for (i = 0; i < digits; ++i) {
      a->dp[i] &= MP_MASK;
   }

   mp_clamp(a);
   return MP_OKAY;
}
#endif
