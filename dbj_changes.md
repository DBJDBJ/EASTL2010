
# changes made

<a href="https://github.com/uNetworking/uSockets/releases"><img src="https://img.shields.io/github/v/release/uNetworking/uSockets"></a> 

<a href="https://lgtm.com/projects/g/uNetworking/uSockets/context:cpp"><img alt="Language grade: C/C++" src="https://img.shields.io/lgtm/grade/cpp/g/uNetworking/uSockets.svg?logo=lgtm&logoWidth=18"/></a>

## Built with Visual Studio 2019 

C++17

1. using CL
2. using CLANG

No other changed to any project properties have been done.

## vs2019-test main.cpp

has been changed .. not dramatically. Just to make it more functional and informative.

## Details

### eacompilertraits.h

Line 333

```cpp
// Make sure we get no macro naming conflicts
// DBJ added the following line
#if ! defined(EA_COMPILER_CLANG)
#  undef CLANG_ALIGNED 
#  undef CLANG_PACKED 
#endif
```

That makes build possible with CLANG 10.x, as packaged under Visual Studio 2019. Problem was in `fixedpool.h` lines 109 to 145.

### string.h

Line 200

added `extern "C"` 

```cpp
extern "C" {
    extern int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t* pFormat, va_list arguments);
    extern int Vsnprintf16(char16_t* pDestination, size_t n, const char16_t* pFormat, va_list arguments);
    extern int Vsnprintf32(char32_t* pDestination, size_t n, const char32_t* pFormat, va_list arguments);
}
```

### main.cpp

Line 121. 

Excluded that implementation

```cpp
#if 0
extern "C" {
    // EASTL also wants us to define this (see string.h line 197)
    int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t* pFormat, va_list arguments)
    {
#ifdef _MSC_VER
        return _vsnprintf(pDestination, n, pFormat, arguments);
#else
        return vsnprintf(pDestination, n, pFormat, arguments);
#endif
    }
}
#endif // 0
```
