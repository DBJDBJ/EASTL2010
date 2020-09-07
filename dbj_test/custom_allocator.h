#pragma once

/*
(c) 2019-2020 by dbj.org   -- LICENSE DBJ -- https://dbj.org/license_dbj/
*/
#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif

#include "dbj_nano_synchro.h" //  dbj::lock_unlock autolock_ ;

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

//  int posix_memalign(void **memptr, size_t alignment, size_t size);
// we use Strong types concept
typedef struct Alignment { size_t val; } Alignment;
typedef struct Size      { size_t val; } Size ;

// corecrt_malloc.h contains _aligned_malloc
inline int dbj_memalign(void** p, Alignment alignment_, Size size_ ) noexcept
{
    DBJ_LOCAL_LOCK;

    int erc = 0;
    _set_errno(0); 

#ifdef _WIN32
    *p = _aligned_malloc(size_.val, alignment_.val);
    erc = errno;
#else  // ! _WIN32
    erc = posix_memalign(p, size_.val, alignment_.val);
#endif // ! _WIN32
    return erc ;
}

inline int dbj_free( void * p ) noexcept
{
    dbj::local_lock_unlock autolock_;

    int erc = 0;
    _set_errno(0);
#ifdef _WIN32
    _aligned_free(p);
    erc = errno;
#else  // ! _WIN32
    free(p);
    erc = errno;
#endif // ! _WIN32
    p = NULL;
    return erc;
}

#ifdef TEST_MALLOC_ALIGNED

inline int test_malloc_aligned() noexcept
{
    dbj::local_lock_unlock autolock_;

    printf("\n" VT100_LIGHT_BLUE __FUNCSIG__  VT100_RESET );

    char* mem = NULL;
    // CL does not do "compund literals"
    // so we do string types in a pedestrian way
    // that is we can not do
    // int erc = dbj_memalign((void**)&mem, (Alignment) { alignment }, (Size) { n });

    Alignment alignment = { 64 };
    Size n = { 100 } ;

    int erc = dbj_memalign((void**)&mem, alignment , n );

    size_t wrong_alignment = 13;

    printf("\nis %3zd byte aligned = %s", wrong_alignment, (((size_t)mem) % wrong_alignment) ? "no" : "yes");
    printf("\nis %3zd byte aligned = %s", alignment.val, (((size_t)mem) % alignment.val ) ? "no" : "yes");

    dbj_free(mem);

    return EXIT_SUCCESS ;
}

#endif // TEST_MALLOC_ALIGNED

/*******************************************************************************************
here we start definining our own allocator
*/

#define _XOPEN_SOURCE 600
// Must declare this to avoid new[]/delete[] prototypes in allocator.h
#define EASTL_USER_DEFINED_ALLOCATOR 
#define EASTL_ALLOCATOR_DEFAULT_NAME "dbj_alligned_allocator"

// these macros are important
// we use them before we include eabase/eabase.h

#undef EASTLAllocatorType
#define EASTLAllocatorType dbj::allocator

#undef EASTLAllocatorDefault
#define EASTLAllocatorDefault dbj::GetDefaultAllocator

#include <EABase/eabase.h>
#include <EASTL/internal/config.h>

namespace dbj {

/**
* Machine word size. Depending on the architecture,
* can be 4 or 8 bytes.
*/
    using word_t = intptr_t;
    /// ----------------------------------------------------------------------
    /**
     * Aligns the size by the machine word.
     http://dmitrysoshnikov.com/compilers/writing-a-memory-allocator/#memory-alignment
     */
    constexpr inline size_t align(size_t n) {
        return (n + sizeof(word_t) - 1) & ~(sizeof(word_t) - 1);
    }

    /// ----------------------------------------------------------------------
    inline bool is_aligned(void* ptr, size_t alignment) {
        if (((unsigned long long)ptr % alignment) == 0)
            return true;
        return false;
    }

  // Define our allocator class and implement it
  class allocator
  {
  public:
    
    // Constructors
    EASTL_ALLOCATOR_EXPLICIT inline allocator(const char* EASTL_NAME(name) = EASTL_ALLOCATOR_DEFAULT_NAME) noexcept
    {
#if EASTL_NAME_ENABLED
      mpName = name;
#endif
    }
     allocator(const allocator& EASTL_NAME(alloc)) noexcept 
    {
#if EASTL_NAME_ENABLED
      mpName = alloc.mpName;
#endif
    }
     allocator(const allocator&, const char* EASTL_NAME(name)) noexcept
    {
#if EASTL_NAME_ENABLED
      mpName = name ? name : EASTL_ALLOCATOR_DEFAULT_NAME;
#endif
    }
    
    // Assignment
     allocator& operator=(const allocator& EASTL_NAME(alloc)) noexcept
    {
#if EASTL_NAME_ENABLED
      mpName = alloc.mpName;
#endif
      return *this; // All considered equal since they're global malloc/free
    }
    
    // Allocation & Deallocation
     void* allocate(size_t n, int flags = 0) noexcept
    {
#if defined(EA_DEBUG)
      printf("Malloc'ing %lu bytes\n",n);
#endif
      return allocate( n, 8 /* alignment */ , 0 /* offset */ , flags );
    }

    // obviously aligned allocation is required here
     void* allocate(size_t n_, size_t alignment_, size_t offset, int flags = 0) noexcept
    {
      char* mem = NULL;
      
#if defined(EA_DEBUG)
      printf("Align Malloc'ing %lu bytes\n",n);
#endif
      
      Alignment alignment = { alignment_ };
      Size      n = {n_};
      dbj_memalign((void**)&mem, alignment, n ) ;
      return mem;
    }
    
     void deallocate(void* p, size_t n) noexcept
    {
#if defined(EA_DEBUG)
      printf("Freeing %lu bytes\n",n);
#endif
      dbj_free(p);
    }
    
    // Name info
     const char* get_name() const noexcept
    {
#if EASTL_NAME_ENABLED
      return mpName;
#else
      return EASTL_ALLOCATOR_DEFAULT_NAME;
#endif
    }

     void set_name(const char* EASTL_NAME(name)) noexcept
    {
#if EASTL_NAME_ENABLED
      mpName = name;
#endif
    }
    
    
  private:
#if EASTL_NAME_ENABLED
    const char* mpName;
#endif
  }; // allocator
  
  // EASTL expects us to define these operators (allocator.h L103)
  inline bool operator==(const allocator& a, const allocator& b)
  {
    return true; // All are considered equal since they are global malloc/free
  }
  inline bool operator!=(const allocator& a, const allocator& b)
  {
    return false; // All are considered equal since they are global malloc/free
  }
  
  // Defines the EASTL API glue, so we can set our allocator as the global default allocator

  namespace detail {
      inline  allocator  default_allocator_global_instance_;
      inline  allocator* ptr_to_default_allocator_global_ = &default_allocator_global_instance_;
  } // detail

  inline  allocator* GetDefaultAllocator()
  { 
      DBJ_LOCAL_LOCK;

      return detail::ptr_to_default_allocator_global_;
  }
  
  inline allocator* SetDefaultAllocator(allocator* pNewAlloc)
  {
      DBJ_LOCAL_LOCK;

    allocator* pOldAlloc = detail::ptr_to_default_allocator_global_;
    detail::ptr_to_default_allocator_global_ = pNewAlloc;
    return pOldAlloc;
  }

} // namespace dbj


#ifdef TEST_CUSTOM_ALLOCATOR
#include <EASTL/string.h>

inline int test_custom_allocator()
{
    printf("\n" VT100_LIGHT_BLUE __FUNCSIG__  VT100_RESET);
  eastl::string str;

  str += "o";
  str += "m";
  str += "f";
  str += "g";
  str += "o";
  str += "m";
  str += "f";
  str += "g";
  str += "o";
  str += "m";
  str += "f";
  str += "g";
  str += "o";
  str += "m";
  str += "f";
  str += "g";
  str += "o";
  str += "m";
  str += "f";
  str += "g";
  str += "o";
  str += "m";
  str += "f";
  str += "g";
  // EASTL_ASSERT(str == "omfg");

  return EXIT_SUCCESS;
}

#endif // TEST_CUSTOM_ALLOCATOR

