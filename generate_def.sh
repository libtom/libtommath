echo "; libtommath" >tommath.def
echo ";" >>tommath.def
echo "; Use this command to produce a 32-bit .lib file, for use in any MSVC version" >>tommath.def
echo ";   lib -machine:X86 -name:libtommath.dll -def:tommath.def -out:tommath.lib" >>tommath.def
echo "; Use this command to produce a 64-bit .lib file, for use in any MSVC version" >>tommath.def
echo ";   lib -machine:X64 -name:libtommath.dll -def:tommath.def -out:tommath.lib" >>tommath.def
echo ";" >>tommath.def
echo "EXPORTS" >>tommath.def
git ls-files|grep bn_mp_|grep \\.c|grep -v 'mp_toom'|grep -v 'mp_kara'|grep -v '_fast'|grep -v '_slow'|grep -v '_smap'|sed -e 's/bn_mp_rand/bn_mp_rand\nbn_mp_rand_digit/'|sort|sed -e 's/bn_/    /g'|sed -e 's/\.c//g'>>tommath.def
