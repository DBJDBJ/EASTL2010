#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#ifdef _WIN32
// corecrt_malloc.h contains _aligned_malloc
#define posix_memalign(p, a, s) (((*(p)) = _aligned_malloc((s), (a))), *(p) ?0 :errno)
#define posix_free _aligned_free
#else
#define posix_free free
#endif // _WIN32

#ifdef TEST_MALLOC_ALIGNED

inline int test_malloc_aligned()
{
    printf("\n"  __FUNCSIG__ "\n" );

    char* mem = NULL;
    int alignment = 64;
    int n = 100;

    int erc = posix_memalign((void**)&mem, alignment, n);

    printf("\nis %3d byte aligned = %s\n", 32, (((size_t)mem) % 32) ? "no" : "yes");
    printf("\nis %3d byte aligned = %s\n", alignment, (((size_t)mem) % alignment ) ? "no" : "yes");

    posix_free(mem);

    return EXIT_SUCCESS ;
}

#endif // TEST_MALLOC_ALIGNED

/*
here we start definining our own allocator
*/

#define _XOPEN_SOURCE 600
// Must declare this to avoid new[]/delete[] prototypes in allocator.h
#define EASTL_USER_DEFINED_ALLOCATOR 
#define EASTL_ALLOCATOR_DEFAULT_NAME "dbj_alligned_allocator"

// these macros are important
// we use them before we include eabase/eabase.h

#undef EASTLAllocatorType
#define EASTLAllocatorType CustomAllocator::allocator

#undef EASTLAllocatorDefault
#define EASTLAllocatorDefault CustomAllocator::GetDefaultAllocator

#include <EABase/eabase.h>
#include <EASTL/internal/config.h>

namespace CustomAllocator {

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
     void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0) noexcept
    {
      char* mem = NULL;
      
#if defined(EA_DEBUG)
      printf("Align Malloc'ing %lu bytes\n",n);
#endif
      
      posix_memalign((void**)&mem, alignment, n);
      return mem;
    }
    
     void deallocate(void* p, size_t n) noexcept
    {
#if defined(EA_DEBUG)
      printf("Freeing %lu bytes\n",n);
#endif
      posix_free(p);
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
  // WARNING: in the presence of MT this will not work well. these are process wide globals
  // this here is just a sampling code ... please use your own form of locking in real life.
  inline  allocator  gDefaultAllocator;
  inline  allocator* gpDefaultAllocator = &gDefaultAllocator;
  
  inline  allocator* GetDefaultAllocator()
  { 
      return gpDefaultAllocator; 
  }
  
  inline allocator* SetDefaultAllocator(allocator* pNewAlloc)
  {
    allocator* pOldAlloc = gpDefaultAllocator;
    gpDefaultAllocator = pNewAlloc;
    return pOldAlloc;
  }

} // namespace CustomAllocator


#if 0
// EASTL also wants us to define this (see string.h line 197)
int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t* pFormat, va_list arguments)
{
#ifdef _MSC_VER
  return _vsnprintf(pDestination, n, pFormat, arguments);
#else
  return vsnprintf(pDestination, n, pFormat, arguments);
#endif
}
#endif // 0

#include <EASTL/string.h>

inline int test_custom_allocator()
{
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
