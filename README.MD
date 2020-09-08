

<h1>DBJ EASTL 2010</h1>

- [1. Apologies first](#1-apologies-first)
- [2. Why using EASTL 2010 in 2020](#2-why-using-eastl-2010-in-2020)
- [3. How to use](#3-how-to-use)
- [4. Built with Visual Studio 2019](#4-built-with-visual-studio-2019)
	- [4.1. Allocators](#41-allocators)
		- [4.1.1. I want my own Allocator class](#411-i-want-my-own-allocator-class)
		- [4.2. EASTL_ALLOCATOR_EXPLICIT](#42-eastl_allocator_explicit)
	- [4.3. main.cpp](#43-maincpp)
	- [4.4. eacompilertraits.h](#44-eacompilertraitsh)
- [5. What is missing and what to do about it](#5-what-is-missing-and-what-to-do-about-it)
	- [5.1. Array](#51-array)
	- [5.2. iostreams](#52-iostreams)
- [6. IP and License](#6-ip-and-license)
	- [6.1. Original IP and License](#61-original-ip-and-license)

## 1. Apologies first

(eating a lot of humble pie)

Things are advancing too fast. Code is becoming sprinkled with "TODO" and "ROADMAP". Please (if you could) use the Visual Studio SLN in here. Things will be (very) clear that way.

This is not a replacement for documentation. I know and I promise it is on its way!

## 2. Why using EASTL 2010 in 2020

Because it contains a lot of what we need for standard C++ code. We develop mainly Windows Server side components. We are building using the CL compiler `/kernel` switch. We tend to use C11. But we are not C zealots. We use standard C++ too. For C++ code, '/kernel' switch means no MS STL. 

We use [EASTL](https://github.com/electronicarts/EASTL) library instead of C++ std lib. Actually, what was EASTL 10+ years ago. And it works well; it is simple and fast. 

Actually on WIN10 PRO, 8GB RAM, i5, machine it is consistently faster than MS STL. For longer strings. MS STL uses SSO (Small Size Optimization) in their string.

By 2020 **std::allocator** turns out to be (one of the) key issues weighing down the std lib. In here there is `eastl::allocator` which is faster by design. And we have changed it so that extension mechanism is even simpler. No exceptions, no problems.

All such things are infinitely simpler to change vs MS/CLANG/G++ STL. We will update this readme regularly with new "findings".

## 3. How to use

We include the [headers](./include/) and we add the cpp files in the [src folder](./src/) . We do not build a library or a DLL to use this. You know the DLL written in C++ is very elusive prey to catch. And we have no time for that hunt. 

But, our main kind of a project is WIN32 service? Windows service or a driver is kind-of-a DLL. Which is fine since we use this code only internally.

## 4. Built with Visual Studio 2019 

C++ : Standard C++17. Remember: this is already not tne official latest standard.

Compiled with Visual Studio 2019 and

1. Visual C++ 2019   00435-60000-00000-AA768
2. CLANG 10.0.1 (packaged with VS)

No other changes to any project properties have been done. In any case please look into the [VS project provided](./dbj_test/) for an standard build sample, with few tests.

### 4.1. Allocators

During last few years, inability to have noexcept allocators in the std lib [fractures the whole C++ community](https://thephd.github.io/freestanding-noexcept-allocators-vector-memory-hole) into two fractions. The anti std lib half and the pro std lib half. The rift which probably will not heal. 

EASTL (2020) is one known example from the half which is not using std lib. When EASTL 2010 was conceived, that was perhaps not clearly understood, but fortunately "the need for speed" has born the different allocator mechanism.

We have considerably simplified the allocation / deallocation mechanism now available to end users. All you have to do is to provide implementations to three functions. Here is the working example:

```cpp
namespace eastl {
	extern "C" {

		// EASTL expects us to define these, see allocator.h 
		inline void* user_defined_alloc(
			size_t size_,
			const char* pName,
			int flags,
			unsigned debugFlags,
			const char* file,
			int line
		) noexcept
		{
			// note: no checks whatsoever ...
			return malloc(size_);
		}

		// alligned allocation
		inline void* user_defined_alloc_aligned(
			size_t size_,
			size_t alignment_,
			size_t alignmentOffset,
			const char* pName,
			int flags,
			unsigned debugFlags,
			const char* file,
			int line
		) noexcept
		{
			// no alignment
			// note: no checks whatsoever ...
			return malloc(size_);
		}

		inline void user_defined_deallocate(void* ptr_) noexcept
		{
			free(ptr_);
		}
	} // "C"
} // eastl ns
```

Things to note

1. This functions are mandatory in any case. There are no defaults. 
   1. Why? Because this forces the best possible alloc / dealoc solution for your project.
2. there is a design error above. aligned alloc has no aligned delete. 
   1. we have solved it by not doing actual aligned alloc.
3. these functions signatures will be much simplified
4. Roadmap is to have only two functions: alloc and dealoc.
   1. That is doable but is not a trivial change. Stay tuned.
   2. Immediate remedy. As we said: do the same alloc in both `user_defined_alloc_aligned` and `user_defined_alloc` just like in the sample above.

#### 4.1.1. I want my own Allocator class

When using this lib, please think **first** of functions above. As a secondary strategy, you can develop your own custom allocator to be used with this lib. EASTL allocator synopsis is this:

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
The actual mechanism we do not like or use. It is based on "magical" macros. If you are nice and you ask us for an example on how to develop and install your Allocator class we might just do it for you :wink: . 

#### 4.2. EASTL_ALLOCATOR_EXPLICIT

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

### 4.3. main.cpp

It has been changed .. not dramatically. Just to make it more functional and informative.

Original intent was narrow: to compare the speed v.s. MS STL.

### 4.4. eacompilertraits.h

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

## 5. What is missing and what to do about it

### 5.1. Array

There is `eastl::fixed_vector`. But that is not std::array. Not even close. We do use c++ array version of our own. A lot.

There are quite a few std array implementations on the github. We use our own. Ask and we will send it to you.

### 5.2. iostreams

iostreams are the epitome of C++ gone the wrong way. Or they are a god send. It depends on your point of view. What do I think? I think implementation is the problem. Too much inheritance and virtual tables. Exceptions and (even) std::string are in use. I would rather avoid iostreams.

In embedded and all the other mission critical code there is no console whatsoever. Neither there is heap very often. And local filesystem is almost always out of reach.

We use very light and small `printf` version, that uses no heap at all. Again, ask and we will send it to you.

---

## 6. IP and License

Changes in this repo are &copy; 2020 by dbj.systems

Licence : [LICENCE_DBJ](https://dbj.org/license_dbj/)

### 6.1. Original IP and License

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


