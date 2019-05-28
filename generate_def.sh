echo "; libtommath" >tommath.def
echo ";" >>tommath.def
echo "; Use this command to produce a 32-bit .lib file, for use in any MSVC version" >>tommath.def
echo ";   lib -machine:X86 -name:libtommath.dll -def:tommath.def -out:tommath.lib" >>tommath.def
echo "; Use this command to produce a 64-bit .lib file, for use in any MSVC version" >>tommath.def
echo ";   lib -machine:X64 -name:libtommath.dll -def:tommath.def -out:tommath.lib" >>tommath.def
echo ";" >>tommath.def
echo "EXPORTS" >>tommath.def
git ls-files|grep \\.c|sed -e 's/bn_mp_rand/bn_mp_rand\nbn_mp_rand_digit/'|sed -e 's/bn_conversion/bn_mp_set_i32\nbn_mp_set_i64\nbn_mp_set_u32\nbn_mp_set_u64\nbn_mp_set_int\nbn_mp_set_long\nbn_mp_set_long_long\nbn_mp_get_i32\nbn_mp_get_i64\nbn_mp_get_mag32\nbn_mp_get_mag64\nbn_mp_get_int\nbn_mp_get_long\nbn_mp_get_long_long\nbn_mp_init_i32\nbn_mp_init_i64\nbn_mp_init_u32\nbn_mp_init_u64\nbn_mp_init_set_int/'|grep -v bn_mp_radix_smap|grep bn_mp_|sort|sed -e 's/bn_/    /g'|sed -e 's/\.c//g'>>tommath.def
