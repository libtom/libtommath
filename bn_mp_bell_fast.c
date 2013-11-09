int mp_bell_fast(unsigned long n, mp_int * c)
{
  unsigned long i = 0;
  mp_int s, temp;
  if ((e = mp_init_multi(&s, &temp, NULL)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_stirling2_mat(n, n, &temp)) != MP_OKAY) {
    return e;
  }
  while (i++ <= n) {
    if ((e = mp_stirling2_mat(n, i, &temp)) != MP_OKAY) {
      return e;
    }
    mp_add(&s, &temp, &s);
  }
  mp_exch(&s, c);
  mp_clear_multi(&s, &temp, NULL);
  mp_stirling_2_free();
  return MP_OKAY;
}
