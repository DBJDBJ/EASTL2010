#pragma once

#define VT100_ESC "\x1b["
#define VT100_RESET VT100_ESC "0m"
#define VT100_LIGHT_GRAY VT100_ESC "90m"
#define VT100_LIGHT_BLUE VT100_ESC "94m"
#define VT100_LIGHT_CYAN VT100_ESC "36m"
#define VT100_LIGHT_YELLOW VT100_ESC "33m"
#define VT100_LIGHT_GREEN VT100_ESC "32m"
#define VT100_LIGHT_RED VT100_ESC "31m"
#define VT100_LIGHT_MAGENTA VT100_ESC "35m"

/* (c) 2019-2020 by dbj.org   -- LICENSE DBJ -- https://dbj.org/license_dbj/ */

#undef  DBJ_PERROR 
#ifdef _DEBUG
#define DBJ_PERROR (perror(__FILE__ " # " _CRT_STRINGIZE(__LINE__))) 
#else
#define DBJ_PERROR
#endif // _DEBUG

#undef DBJ_FERROR
#ifdef _DEBUG
#define DBJ_FERROR( FP_) \
do { \
if (ferror(FP_) != 0) {\
	DBJ_PERROR ;\
	clearerr_s(FP_);\
} \
} while(0)
#else
#define DBJ_FERROR( FP_ )
#endif // _DEBUG

#define SX(fmt_, x_) fprintf(stderr, "\n%s " fmt_, #x_, (x_))

#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif

#ifdef _WIN32
#include "win_console.h"
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <EASTL/internal/config.h>

// alligned allocation
#ifndef NDEBUG
#define TEST_MALLOC_ALIGNED
#endif
#include "custom_allocator.h"

// EASTL expects us to define these, see allocator.h line 194
inline void* operator new[](size_t size_, const char* /* pName */,
    int /* flags */, unsigned /* debugFlags */,
    const char* /* file */, int /* line */)
{
    void* p{};
    Alignment ali = { sizeof( dbj::word_t) };
    Size      size = { size_ };

    // note: no checks whatsoever ...
        int errc = dbj_memalign( &p,ali,size);
     return p;
}

inline void* operator new[](size_t size_, size_t alignment_,
    size_t /* alignmentOffset */, const char* /* pName */,
    int /* flags */, unsigned /* debugFlags */,
    const char* /* file */, int /* line */) {
        // this allocator doesn't support alignment
        EASTL_ASSERT(alignment_ <= 8);
        void* p{};
        Alignment ali = { alignment_ };
        Size      size = { size_ };
        // note: no checks whatsoever ...
        int errc = dbj_memalign(&p, ali, size );
        return p;
}


// EASTL also wants us to define this (see string.h line 197)
extern "C" inline int Vsnprintf8(char * pDestination, size_t count_ ,
    const char8_t* pFormat, va_list arguments) {
#ifdef _MSC_VER
    // return _vsnprintf(pDestination, count_ , pFormat, arguments);
    return _vsnprintf_s(pDestination,  strlen(pDestination), count_ , pFormat, arguments);
#else
    return vsnprintf(pDestination, count_ , pFormat, arguments);
#endif
}
