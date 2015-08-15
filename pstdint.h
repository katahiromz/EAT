/* pstdint.h --- portable stdint.h */
/* Written by katahiromz <katayama.hirofumi.mz@gmail.com>. */
/* You can use this as replacement of <stdint.h> and <cstdint>. */
/* This file is public domain software (PDS). */

#ifndef KATAHIROMZ_PSTDINT_H
#define KATAHIROMZ_PSTDINT_H    1   /* Version 1 */

#if defined(__cplusplus) && (__cplusplus >= 201103L)
    #include <cstdint>
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
    #include <stdint.h>
#elif defined(_WIN32)
    /* 32-bit or 64-bit Windows C/C++ compiler */
    #ifdef __cplusplus
        #include <climits>
    #else
        #include <limits.h>
    #endif

    /* signed types */
    typedef signed char             int8_t;
    typedef short                   int16_t;
    typedef int                     int32_t;

    typedef int                     int_fast8_t;
    typedef int                     int_fast16_t;
    typedef int                     int_fast32_t;

    typedef signed char             int_least8_t;
    typedef short                   int_least16_t;
    typedef int                     int_least32_t;

    #define INT8_MIN                SCHAR_MIN
    #define INT16_MIN               SHRT_MIN
    #define INT32_MIN               INT_MIN

    #define INT8_MAX                SCHAR_MAX
    #define INT16_MAX               SHRT_MAX
    #define INT32_MAX               INT_MAX

    #define INT_FAST8_MIN           INT_MIN
    #define INT_FAST16_MIN          INT_MIN
    #define INT_FAST32_MIN          INT_MIN

    #define INT_FAST8_MAX           INT_MAX
    #define INT_FAST16_MAX          INT_MAX
    #define INT_FAST32_MAX          INT_MAX

    #define INT_LEAST8_MIN          SCHAR_MIN
    #define INT_LEAST16_MIN         SHRT_MIN
    #define INT_LEAST32_MIN         INT_MIN

    #define INT_LEAST8_MAX          SCHAR_MAX
    #define INT_LEAST16_MAX         SHRT_MAX
    #define INT_LEAST32_MAX         INT_MAX

    /* unsigned types */
    typedef unsigned char           uint8_t;
    typedef unsigned short          uint16_t;
    typedef unsigned int            uint32_t;

    typedef unsigned int            uint_fast8_t;
    typedef unsigned int            uint_fast16_t;
    typedef unsigned int            uint_fast32_t;

    typedef unsigned char           uint_least8_t;
    typedef unsigned short          uint_least16_t;
    typedef unsigned int            uint_least32_t;

    #define UINT8_MIN               UCHAR_MIN
    #define UINT16_MIN              USHRT_MIN
    #define UINT32_MIN              UINT_MIN

    #define UINT8_MAX               UCHAR_MAX
    #define UINT16_MAX              USHRT_MAX
    #define UINT32_MAX              UINT_MAX

    #define UINT_FAST8_MIN          UINT_MIN
    #define UINT_FAST16_MIN         UINT_MIN
    #define UINT_FAST32_MIN         UINT_MIN

    #define UINT_FAST8_MAX          UINT_MAX
    #define UINT_FAST16_MAX         UINT_MAX
    #define UINT_FAST32_MAX         UINT_MAX

    #define UINT_LEAST8_MIN         UCHAR_MIN
    #define UINT_LEAST16_MIN        USHRT_MIN
    #define UINT_LEAST32_MIN        UINT_MIN

    #define UINT_LEAST8_MAX         UCHAR_MAX
    #define UINT_LEAST16_MAX        USHRT_MAX
    #define UINT_LEAST32_MAX        UINT_MAX

    /* 64-bit types */
    typedef __int64                 int64_t;
    typedef __int64                 int_fast64_t;
    typedef __int64                 int_least64_t;

    typedef unsigned __int64        uint64_t;
    typedef unsigned __int64        uint_fast64_t;
    typedef unsigned __int64        uint_least64_t;

    #ifdef _I64_MIN
        #define INT64_MIN           _I64_MIN
        #define INT64_MAX           _I64_MAX
        #define INT_FAST64_MIN      _I64_MIN
        #define INT_FAST64_MAX      _I64_MAX
        #define INT_LEAST64_MIN     _I64_MIN
        #define INT_LEAST64_MAX     _I64_MAX

        #define UINT64_MIN          0
        #define UINT64_MAX          _UI64_MAX
        #define UINT_FAST64_MIN     0
        #define UINT_FAST64_MAX     _UI64_MAX
        #define UINT_LEAST64_MIN    0
        #define UINT_LEAST64_MAX    _UI64_MAX
    #elif defined(LLONG_MIN)
        #define INT64_MIN           LLONG_MIN
        #define INT64_MAX           LLONG_MAX
        #define INT_FAST64_MIN      LLONG_MIN
        #define INT_FAST64_MAX      LLONG_MAX
        #define INT_LEAST64_MIN     LLONG_MIN
        #define INT_LEAST64_MAX     LLONG_MAX

        #define UINT64_MIN          0
        #define UINT64_MAX          ULLONG_MAX
        #define UINT_FAST64_MIN     0
        #define UINT_FAST64_MAX     ULLONG_MAX
        #define UINT_LEAST64_MIN    0
        #define UINT_LEAST64_MAX    ULLONG_MAX
    #else
        #define You lose.
    #endif

    /* max */
    typedef __int64                 intmax_t;
    typedef unsigned __int64        uintmax_t;

    #define INTPTR_MIN              INT64_MIN
    #define INTPTR_MAX              INT64_MAX
    #define UINTPTR_MIN             UINT64_MIN
    #define UINTPTR_MAX             UINT64_MAX
#elif defined(__x86_64__) || defined(__ppc64__)
    /* 64-bit non-Windows C/C++ compiler */
    #ifdef __cplusplus
        #include <climits>
    #else
        #include <limits.h>
    #endif

    /* signed types */
    typedef signed char             int8_t;
    typedef short                   int16_t;
    typedef int                     int32_t;

    typedef long                    int_fast8_t;
    typedef long                    int_fast16_t;
    typedef long                    int_fast32_t;

    typedef signed char             int_least8_t;
    typedef short                   int_least16_t;
    typedef int                     int_least32_t;

    #define INT8_MIN                SCHAR_MIN
    #define INT16_MIN               SHRT_MIN
    #define INT32_MIN               INT_MIN

    #define INT8_MAX                SCHAR_MAX
    #define INT16_MAX               SHRT_MAX
    #define INT32_MAX               INT_MAX

    #define INT_FAST8_MIN           INT_MIN
    #define INT_FAST16_MIN          INT_MIN
    #define INT_FAST32_MIN          INT_MIN

    #define INT_FAST8_MAX           INT_MAX
    #define INT_FAST16_MAX          INT_MAX
    #define INT_FAST32_MAX          INT_MAX

    #define INT_LEAST8_MIN          SCHAR_MIN
    #define INT_LEAST16_MIN         SHRT_MIN
    #define INT_LEAST32_MIN         INT_MIN

    #define INT_LEAST8_MAX          SCHAR_MAX
    #define INT_LEAST16_MAX         SHRT_MAX
    #define INT_LEAST32_MAX         INT_MAX

    /* unsigned types */
    typedef unsigned char           uint8_t;
    typedef unsigned short          uint16_t;
    typedef unsigned int            uint32_t;

    typedef unsigned long           uint_fast8_t;
    typedef unsigned long           uint_fast16_t;
    typedef unsigned long           uint_fast32_t;

    typedef unsigned char           uint_least8_t;
    typedef unsigned short          uint_least16_t;
    typedef unsigned int            uint_least32_t;

    #define UINT8_MIN               UCHAR_MIN
    #define UINT16_MIN              USHRT_MIN
    #define UINT32_MIN              UINT_MIN

    #define UINT8_MAX               UCHAR_MAX
    #define UINT16_MAX              USHRT_MAX
    #define UINT32_MAX              UINT_MAX

    #define UINT_FAST8_MIN          UINT_MIN
    #define UINT_FAST16_MIN         UINT_MIN
    #define UINT_FAST32_MIN         UINT_MIN

    #define UINT_FAST8_MAX          UINT_MAX
    #define UINT_FAST16_MAX         UINT_MAX
    #define UINT_FAST32_MAX         UINT_MAX

    #define UINT_LEAST8_MIN         UCHAR_MIN
    #define UINT_LEAST16_MIN        USHRT_MIN
    #define UINT_LEAST32_MIN        UINT_MIN

    #define UINT_LEAST8_MAX         UCHAR_MAX
    #define UINT_LEAST16_MAX        USHRT_MAX
    #define UINT_LEAST32_MAX        UINT_MAX

    /* 64-bit types */
    typedef long                    int64_t;
    typedef long                    int_fast64_t;
    typedef long                    int_least64_t;

    #define INT64_MIN               LONG_MIN
    #define INT64_MAX               LONG_MAX
    #define INT_FAST64_MIN          LONG_MIN
    #define INT_FAST64_MAX          LONG_MAX
    #define INT_LEAST64_MIN         LONG_MIN
    #define INT_LEAST64_MAX         LONG_MAX

    typedef unsigned long           uint64_t;
    typedef unsigned long           uint_fast64_t;
    typedef unsigned long           uint_least64_t;

    #define UINT64_MIN              0
    #define UINT64_MAX              ULONG_MAX
    #define UINT_FAST64_MIN         0
    #define UINT_FAST64_MAX         ULONG_MAX
    #define UINT_LEAST64_MIN        0
    #define UINT_LEAST64_MAX        ULONG_MAX

    /* max */
    typedef long                    intmax_t;
    typedef unsigned long           uintmax_t;

    #define INTPTR_MIN              INT64_MIN
    #define INTPTR_MAX              INT64_MAX
    #define UINTPTR_MIN             UINT64_MIN
    #define UINTPTR_MAX             UINT64_MAX
#elif defined(MSDOS) || defined(DOS) || defined(_WIN16) || defined(LSI_C)
    /* MS-DOS C/C++ compiler */
    #ifdef __cplusplus
        #include <climits>
    #else
        #include <limits.h>
    #endif

    /* signed types */
    typedef signed char             int8_t;
    typedef short                   int16_t;
    typedef long                    int32_t;

    typedef short                   int_fast8_t;
    typedef short                   int_fast16_t;
    typedef long                    int_fast32_t;

    typedef signed char             int_least8_t;
    typedef short                   int_least16_t;
    typedef long                    int_least32_t;

    #define INT8_MIN                SCHAR_MIN
    #define INT16_MIN               SHRT_MIN
    #define INT32_MIN               LONG_MIN

    #define INT8_MAX                SCHAR_MAX
    #define INT16_MAX               SHRT_MAX
    #define INT32_MAX               LONG_MAX

    #define INT_FAST8_MIN           INT_MIN
    #define INT_FAST16_MIN          INT_MIN
    #define INT_FAST32_MIN          LONG_MIN

    #define INT_FAST8_MAX           INT_MAX
    #define INT_FAST16_MAX          INT_MAX
    #define INT_FAST32_MAX          LONG_MAX

    #define INT_LEAST8_MIN          SCHAR_MIN
    #define INT_LEAST16_MIN         SHRT_MIN
    #define INT_LEAST32_MIN         LONG_MIN

    #define INT_LEAST8_MAX          SCHAR_MAX
    #define INT_LEAST16_MAX         SHRT_MAX
    #define INT_LEAST32_MAX         LONG_MAX

    /* unsigned types */
    typedef unsigned char           uint8_t;
    typedef unsigned short          uint16_t;
    typedef unsigned long           uint32_t;

    typedef unsigned short          uint_fast8_t;
    typedef unsigned short          uint_fast16_t;
    typedef unsigned long           uint_fast32_t;

    typedef unsigned char           uint_least8_t;
    typedef unsigned short          uint_least16_t;
    typedef unsigned long           uint_least32_t;

    #define UINT8_MIN               UCHAR_MIN
    #define UINT16_MIN              USHRT_MIN
    #define UINT32_MIN              ULONG_MIN

    #define UINT8_MAX               UCHAR_MAX
    #define UINT16_MAX              USHRT_MAX
    #define UINT32_MAX              ULONG_MAX

    #define UINT_FAST8_MIN          UINT_MIN
    #define UINT_FAST16_MIN         UINT_MIN
    #define UINT_FAST32_MIN         ULONG_MIN

    #define UINT_FAST8_MAX          UINT_MAX
    #define UINT_FAST16_MAX         UINT_MAX
    #define UINT_FAST32_MAX         ULONG_MAX

    #define UINT_LEAST8_MIN         UCHAR_MIN
    #define UINT_LEAST16_MIN        USHRT_MIN
    #define UINT_LEAST32_MIN        ULONG_MIN

    #define UINT_LEAST8_MAX         UCHAR_MAX
    #define UINT_LEAST16_MAX        USHRT_MAX
    #define UINT_LEAST32_MAX        ULONG_MAX

    /* max */
    typedef long                    intmax_t;
    typedef unsigned long           uintmax_t;

    #define INTPTR_MIN              INT32_MIN
    #define INTPTR_MAX              INT32_MAX
    #define UINTPTR_MIN             UINT32_MIN
    #define UINTPTR_MAX             UINT32_MAX
#else
    #error Not supported yet. You lose.
#endif

#if defined(__cplusplus) && (__cplusplus >= 201103L)
    /* OK */
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
    /* OK */
#elif defined(__cplusplus)
    namespace std {
        using ::int8_t;
        using ::int16_t;
        using ::int32_t;

        using ::int_fast8_t;
        using ::int_fast16_t;
        using ::int_fast32_t;

        using ::int_least8_t;
        using ::int_least16_t;
        using ::int_least32_t;

        using ::uint8_t;
        using ::uint16_t;
        using ::uint32_t;

        using ::uint_fast8_t;
        using ::uint_fast16_t;
        using ::uint_fast32_t;

        using ::uint_least8_t;
        using ::uint_least16_t;
        using ::uint_least32_t;

        using ::intmax_t;
        using ::uintmax_t;

        #ifdef UINT64_MAX
            using ::int64_t;
            using ::int_fast64_t;
            using ::int_least64_t;
            using ::uint64_t;
            using ::uint_fast64_t;
            using ::uint_least64_t;
        #endif  /* not 16-bit */
    } /* namespace std */
#endif  /* __cplusplus */

typedef char KATAHIROMZ_PSTDINT_TEST_00[(sizeof(int8_t) == 1) ? 1 : -1];
typedef char KATAHIROMZ_PSTDINT_TEST_01[(sizeof(uint8_t) == 1) ? 1 : -1];
typedef char KATAHIROMZ_PSTDINT_TEST_02[(sizeof(int16_t) == 2) ? 1 : -1];
typedef char KATAHIROMZ_PSTDINT_TEST_03[(sizeof(uint16_t) == 2) ? 1 : -1];
typedef char KATAHIROMZ_PSTDINT_TEST_04[(sizeof(int32_t) == 4) ? 1 : -1];
typedef char KATAHIROMZ_PSTDINT_TEST_05[(sizeof(uint32_t) == 4) ? 1 : -1];

typedef char KATAHIROMZ_PSTDINT_TEST_06[(sizeof(int_least8_t) >= 1) ? 1 : -1];
typedef char KATAHIROMZ_PSTDINT_TEST_07[(sizeof(uint_least8_t) >= 1) ? 1 : -1];
typedef char KATAHIROMZ_PSTDINT_TEST_08[(sizeof(int_least16_t) >= 2) ? 1 : -1];
typedef char KATAHIROMZ_PSTDINT_TEST_09[(sizeof(uint_least16_t) >= 2) ? 1 : -1];
typedef char KATAHIROMZ_PSTDINT_TEST_10[(sizeof(int_least32_t) >= 4) ? 1 : -1];
typedef char KATAHIROMZ_PSTDINT_TEST_11[(sizeof(uint_least32_t) >= 4) ? 1 : -1];

typedef char KATAHIROMZ_PSTDINT_TEST_12[(sizeof(int_fast8_t) >= 1) ? 1 : -1];
typedef char KATAHIROMZ_PSTDINT_TEST_13[(sizeof(uint_fast8_t) >= 1) ? 1 : -1];
typedef char KATAHIROMZ_PSTDINT_TEST_14[(sizeof(int_fast16_t) >= 2) ? 1 : -1];
typedef char KATAHIROMZ_PSTDINT_TEST_15[(sizeof(uint_fast16_t) >= 2) ? 1 : -1];
typedef char KATAHIROMZ_PSTDINT_TEST_16[(sizeof(int_fast32_t) >= 4) ? 1 : -1];
typedef char KATAHIROMZ_PSTDINT_TEST_17[(sizeof(uint_fast32_t) >= 4) ? 1 : -1];

#ifdef UINT64_MAX
    typedef char KATAHIROMZ_PSTDINT_TEST_18[(sizeof(int64_t) == 8) ? 1 : -1];
    typedef char KATAHIROMZ_PSTDINT_TEST_19[(sizeof(uint64_t) == 8) ? 1 : -1];
    typedef char KATAHIROMZ_PSTDINT_TEST_20[(sizeof(int_least64_t) >= 8) ? 1 : -1];
    typedef char KATAHIROMZ_PSTDINT_TEST_21[(sizeof(uint_least64_t) >= 8) ? 1 : -1];
    typedef char KATAHIROMZ_PSTDINT_TEST_22[(sizeof(int_fast64_t) >= 8) ? 1 : -1];
    typedef char KATAHIROMZ_PSTDINT_TEST_23[(sizeof(uint_fast64_t) >= 8) ? 1 : -1];

    typedef char KATAHIROMZ_PSTDINT_TEST_24[(sizeof(intmax_t) >= 8) ? 1 : -1];
    typedef char KATAHIROMZ_PSTDINT_TEST_25[(sizeof(uintmax_t) >= 8) ? 1 : -1];
#else
    typedef char KATAHIROMZ_PSTDINT_TEST_26[(sizeof(intmax_t) >= 4) ? 1 : -1];
    typedef char KATAHIROMZ_PSTDINT_TEST_27[(sizeof(uintmax_t) >= 4) ? 1 : -1];
#endif

#endif  /* ndef KATAHIROMZ_PSTDINT_H */
