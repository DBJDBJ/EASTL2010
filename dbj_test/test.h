#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <EASTL/internal/config.h>

// EASTL expects us to define these, see allocator.h line 194
inline void* operator new[](size_t size, const char* /* pName */,
    int /* flags */, unsigned /* debugFlags */,
    const char* /* file */, int /* line */) {
        return malloc(size);
}

inline void* operator new[](size_t size, size_t alignment,
    size_t /* alignmentOffset */, const char* /* pName */,
    int /* flags */, unsigned /* debugFlags */,
    const char* /* file */, int /* line */) {
        // this allocator doesn't support alignment
        EASTL_ASSERT(alignment <= 8);
        return malloc(size);
}


// EASTL also wants us to define this (see string.h line 197)
extern "C" inline int Vsnprintf8(char8_t* pDestination, size_t n,
    const char8_t* pFormat, va_list arguments) {
#ifdef _MSC_VER
    return _vsnprintf(pDestination, n, pFormat, arguments);
#else
    return vsnprintf(pDestination, n, pFormat, arguments);
#endif
}
