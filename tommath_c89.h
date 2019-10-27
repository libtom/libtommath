/* please adapt this header to your needs */
typedef enum { MP_NO, MP_YES } mp_bool;
typedef __INT8_TYPE__   mp_i8;
typedef __INT16_TYPE__  mp_i16;
typedef __INT32_TYPE__  mp_i32;
typedef __INT64_TYPE__  mp_i64;
typedef __UINT8_TYPE__  mp_u8;
typedef __UINT16_TYPE__ mp_u16;
typedef __UINT32_TYPE__ mp_u32;
typedef __UINT64_TYPE__ mp_u64;
# if __WORDSIZE == 64
#  define MP_PRI64_PREFIX "l"
# else
#  define MP_PRI64_PREFIX "ll"
# endif
#define MP_PRIi64 MP_PRI64_PREFIX "i"
#define MP_PRIu64 MP_PRI64_PREFIX "u"
#define MP_PRIx64 MP_PRI64_PREFIX "x"
