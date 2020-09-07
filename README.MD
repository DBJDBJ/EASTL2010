# Why using EASTL 2010 in 2020

We develop mainly Windows Server side components. We are building using the CL compiler `/kernel` switch. We tend to use C11. But we are not C zealots. We use standard C++ too. For C++ code, '/kernel' switch means no MS STL. 

We use [EASTL](https://github.com/electronicarts/EASTL) library instead of C++ std lib. Actually, what was EASTL 10+ years ago. And it works well; it is simple and fast. 

Actually on WIN10 PRO, 8GB RAM, i5, machine it is consistently faster than MS STL. For longer strings. MS STL uses SSO (Small Size Optimization) in their string.

By 2020 **std::allocator** turns out to be (one of the) key issues weighing down the std lib. In here there is `eastl::allocator` which is faster by design.  And you are free to use your own. In a very easy way. No exceptions, no problems.

All such things are infinitely simpler to change vs MS STL. We will update this readme regularly with new "findings".

## How to use

We include the [headers](./include/) and we add the cpp files in the [src folder](./src/) . We do not build a library or a DLL to use this. You know the DLL written in C++ is very elusive prey to catch. And we have no time for that hunt. 

But, our main kind of a project is WIN32 service? Windows service or a driver is kind-of-a DLL. Which is fine since we use this code only internally.

## Built with Visual Studio 2019 

C++ : Standard C++17. Remember: this is already not tne official latest standard.

Compiled with Visual Studio 2019 and

1. Visual C++ 2019   00435-60000-00000-AA768
2. CLANG 10.0.1 (packaged with VS)

No other changes to any project properties have been done. In any case please look into the [VS project provided](./dbj_test/) for an standard build sample, with few tests.

### Noexcept Allocators

This is *very* important. During last few years, inability to do this in the std lib [breaks the whole C++ community](https://thephd.github.io/freestanding-noexcept-allocators-vector-memory-hole) into two fractions. The ones anti std lib and the ones pro std lib. The rift which probably will not heal. EASTL 2020 is one known example from the half which is not using std lib. When EASTL 2010 was conceived, that was still already understood.

We simply do not care. Please think of your custom allocator to be used with this lib. Synopsis:

```cpp
namespace dbj
{
    // all noexcept allocator "interface"
    // the std lib dream ...
class allocator
    {
    explicit allocator(const char* pName = NULL) noexcept ;

    void* allocate(size_t n, int flags = 0) noexcept ;
    void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0) noexcept ;
    void  deallocate(void* p, size_t n) noexcept ;

    const char* get_name() const noexcept ;
    void        set_name(const char* pName) noexcept ;
};

// This is part of EASTL mechanism to replace the default allocator
    allocator* GetDefaultAllocator(); 
}
```
Look into `./custom/custom_allocator.h` for an example. Note: default allocator is so default it is best avoided. It uses new and delete operators.

### EASTL_ALLOCATOR_EXPLICIT

Minor change. File config.h , line # 1118

```cpp
#ifndef EASTL_ALLOCATOR_EXPLICIT_ENABLED
// DBJ changed from 0 to 1
    #define EASTL_ALLOCATOR_EXPLICIT_ENABLED 1
#endif

#if EASTL_ALLOCATOR_EXPLICIT_ENABLED
    #define EASTL_ALLOCATOR_EXPLICIT explicit
#else
    #define EASTL_ALLOCATOR_EXPLICIT 
#endif
```

### main.cpp

It has been changed .. not dramatically. Just to make it more functional and informative.

Original intent was to compare the speed v.s. MS STL.

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
