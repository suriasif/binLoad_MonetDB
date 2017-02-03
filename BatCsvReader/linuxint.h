// linux int redefination

#include <stdint.h>

#if !defined(__int8)
  #define __int8 int8_t
#endif

#if !defined(__int16)
  #define __int16 int16_t
#endif

#if !defined(__int32)
  #define __int32 int32_t
#endif

#if !defined(__int64)
  #define __int64 int64_t
#endif

#if !defined(__uint8)
  #define __uint8 uint8_t
#endif

#if !defined(__uint16)
  #define __uint16 uint16_t
#endif

#if !defined(__uint32)
  #define __uint32 uint32_t
#endif

#if !defined(__uint64)
  #define __uint64 uint64_t
#endif


#if !defined(NULL)
 #define NULL 0L
#endif
