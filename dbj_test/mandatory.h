#pragma once

/* (c) 2019-2020 by dbj.org   -- LICENSE DBJ -- https://dbj.org/license_dbj/ */
#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdlib.h>
#include <malloc.h>


//Here we start definining mandatory alloc / dealloc functions
//NOTE: they are always required
//NOTE: yes they will be simplified

namespace eastl {
	extern "C" {

		inline void* user_defined_alloc(
			size_t size_,
			const char* pName,
			int flags,
			unsigned debugFlags,
			const char* file,
			int line
		) noexcept
		{
			return malloc(size_) ;
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
			return malloc(size_);
		}

		inline void user_defined_deallocate(void* ptr_) noexcept
		{
			free(ptr_);
		}
	} // "C"
} // eastl ns




