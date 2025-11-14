#ifndef INTS_HH
#define INTS_HH

#ifdef __cplusplus
	#include <cstdint>
	#include <climits>
#else
	#include <stdint.h>
	#include <limits.h>
#endif

#define ____(a, b) a ## b
#define ___(a, b) ____(a, b)

#define is ==
#define isnt !=
#define nonzero 0 !=
#define notzero != 0
#define zero 0 ==
#define no !
#ifndef __cplusplus
	#define not !
#endif

#define null 0

#define let auto

#define until(...) while ( not (__VA_ARGS__) )

#define byte char

typedef ___(___ ( int_fast, WORD_BIT), _t) fast;
typedef ___(___ (int_least, WORD_BIT), _t) least;

typedef ___(___ ( uint_fast, WORD_BIT), _t) ufast;
typedef ___(___ (uint_least, WORD_BIT), _t) uleast;

typedef intmax_t   most;
typedef uintmax_t umost;

#define fastN_t(N, S)  typedef ___(___ (  int_fast, N),_t) f ## S; \
                       typedef ___(___ ( uint_fast, N),_t) uf ## S;
#define leastN_t(N, S) typedef ___(___ ( int_least, N),_t) l ## S; \
                       typedef ___(___ (uint_least, N),_t) ul ## S;
 
 fastN_t(8,1)  fastN_t(16,2)  fastN_t(32,4)  fastN_t(64,8)
leastN_t(8,1) leastN_t(16,2) leastN_t(32,4) leastN_t(64,8)

#undef fastN_t
#undef leastN_t

#endif
