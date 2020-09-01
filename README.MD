# Using EASTL 2010 in 2020

We develop mainly Windows Server side components. We are building using the CL `/kernel` switch. We tend to use C11. But we are not C zealots. We use standard C++ to. For C++ code un-fortunately, '/kernel' switch means no MS STL. 

We use [EASTL](https://github.com/electronicarts/EASTL) library instead of C++ std lib. Actually, what was EASTL 10+ years ago. And it works well; it is simple and fast. 

Actually on WIN10 PRO, 8GB RAM, i5, machine is it consistently faster than MS STL. For longer strings. ( more than 16 chars in length ).

std::allocator turns out to be (on of the) key issue. Here there is `eastl::allocator` which is faster by design.  And you are free to use your own.

All such things are infinitely simpler to change vs MS STL.

## How to use

We include the [headers](./include/) and we add the cpp files in the [src folder](./src/) . We do not build a library or a DLL to use this. Anyway you know the DLL written in C++ is very elusive prey to catch. And we have no time for that hunt.

## Built with Visual Studio 2019 

Standard C++17 (although [dbj_test](./dbj_test/) is C++14)

Compiled with

1. Visual C++ 2019   00435-60000-00000-AA768
2. CLANG 10.0.1 (packaged with VS)

No other changed to any project properties have been done.

## main.cpp

It has been changed .. not dramatically. Just to make it more functional and informative.

Original intent was to compare the speed v.s. MS STL.

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

### Vsnprintf requirement

It seems not required. We excluded that implementation. In any case [it is very advisable](https://dbj.org/c-char8_t-is-broken/) to steer away from `char8_t`. 

```cpp
// not in use any more
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

---

(rare) changes in this repo are &copy; 2020 by dbj.systems

Licence : [LICENCE_DBJ](https://dbj.org/license_dbj/)

---

<font size="1" >
EASTL in this repo is:

Copyright (C) 2009 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
</font>