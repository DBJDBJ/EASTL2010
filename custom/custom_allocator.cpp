#define _XOPEN_SOURCE 600
#define EASTL_USER_DEFINED_ALLOCATOR // Must declare this to avoid new[]/delete[] prototypes in allocator.h
#define EASTL_ALLOCATOR_DEFAULT_NAME "dbj_alligned_allocator"

#define EASTLAllocatorType CustomAllocator::allocator
#define EASTLAllocatorDefault CustomAllocator::GetDefaultAllocator

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#ifdef _WIN32
// corecrt_malloc.h contains _aligned_malloc
#define posix_memalign(p, a, s) (((*(p)) = _aligned_malloc((s), (a))), *(p) ?0 :errno)
#endif

#if 0
#ifdef _WIN32
extern "C" {

    // https://stackoverflow.com/a/33696858
    inline int check_align(size_t align)
    {
        for (size_t i = sizeof(void*); i != 0; i *= 2)
            if (align == i)
                return true ;
        return false;
    }

    // https://stackoverflow.com/a/6563989
    inline void* malloc_aligned(size_t alignment, size_t bytes)
    {
        // we need to allocate enough storage for the requested bytes, some 
        // book-keeping (to store the location returned by malloc) and some extra
        // padding to allow us to find an aligned byte.  im not entirely sure if 
        // 2 * alignment is enough here, its just a guess.
        const size_t total_size = bytes + (2 * alignment) + sizeof(size_t);

        // use malloc to allocate the memory.
        char* data = (char*)malloc(sizeof(char) * total_size);

        if (data)
        {
            // store the original start of the malloc'd data.
            const void* const data_start = data;

            // dedicate enough space to the book-keeping.
            data += sizeof(size_t);

            // find a memory location with correct alignment.  the alignment minus 
            // the remainder of this mod operation is how many bytes forward we need 
            // to move to find an aligned byte.
            const size_t offset = alignment - (((size_t)data) % alignment);

            // set data to the aligned memory.
            data += offset;

            // write the book-keeping.
            size_t* book_keeping = (size_t*)(data - sizeof(size_t));
            *book_keeping = (size_t)data_start;
        }

        return data;
    }

    inline void free_aligned(void* raw_data)
    {
        if (raw_data)
        {
            char* data = (char *)raw_data;

            // we have to assume this memory was allocated with malloc_aligned.  
            // this means the sizeof(size_t) bytes before data are the book-keeping 
            // which points to the location we need to pass to free.
            data -= sizeof(size_t);

            // set data to the location stored in book-keeping.
            data = (char*)(*((size_t*)data));

            // free the memory.
            free(data);
        }
    }

} // "C"
#endif // _WIN32
#endif // 0

int test_malloc_aligned()
{
    printf("\n"  __FUNCSIG__ "\n" );

    char* mem = NULL;
    int alignment = 64;
    int n = 100;

    posix_memalign((void**)&mem, alignment, n);

    printf("\nis %3d byte aligned = %s\n", 32, (((size_t)mem) % 32) ? "no" : "yes");
    printf("\nis %3d byte aligned = %s\n", alignment, (((size_t)mem) % alignment ) ? "no" : "yes");

    _aligned_free(mem);

    return EXIT_SUCCESS ;
}

#include <EABase/eabase.h>
#include <EASTL/internal/config.h>

namespace CustomAllocator {

  // Define our allocator class and implement it
  class allocator
  {
  public:
    
    // Constructors
    EASTL_ALLOCATOR_EXPLICIT inline allocator(const char* EASTL_NAME(name) = EASTL_ALLOCATOR_DEFAULT_NAME)
    {
#if EASTL_NAME_ENABLED
      mpName = name;
#endif
    }
    inline allocator(const allocator& EASTL_NAME(alloc))
    {
#if EASTL_NAME_ENABLED
      mpName = alloc.mpName;
#endif
    }
    inline allocator(const allocator&, const char* EASTL_NAME(name))
    {
#if EASTL_NAME_ENABLED
      mpName = name ? name : EASTL_ALLOCATOR_DEFAULT_NAME;
#endif
    }
    
    // Assignment
    inline allocator& operator=(const allocator& EASTL_NAME(alloc))
    {
#if EASTL_NAME_ENABLED
      mpName = alloc.mpName;
#endif
      return *this; // All considered equal since they're global malloc/free
    }
    
    // Allocation & Deallocation
    inline void* allocate(size_t n, int flags = 0)
    {
#if defined(EA_DEBUG)
      printf("Malloc'ing %lu bytes\n",n);
#endif
      return allocate( n, 8 /* alignment */ , 0 /* offset */ , flags );
    }

    // obviously aligned allocation is required here
    inline void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0)
    {
      char* mem = NULL;
      
#if defined(EA_DEBUG)
      printf("Align Malloc'ing %lu bytes\n",n);
#endif
      
      posix_memalign((void**)&mem, alignment, n);
      return mem;
    }
    
    inline void deallocate(void* p, size_t n)
    {
#if defined(EA_DEBUG)
      printf("Freeing %lu bytes\n",n);
#endif
      _aligned_free(p);
    }
    
    // Name info
    inline const char* get_name() const
    {
#if EASTL_NAME_ENABLED
      return mpName;
#else
      return EASTL_ALLOCATOR_DEFAULT_NAME;
#endif
    }
    inline void set_name(const char* EASTL_NAME(name))
    {
#if EASTL_NAME_ENABLED
      mpName = name;
#endif
    }
    
    
  private:
#if EASTL_NAME_ENABLED
    const char* mpName;
#endif
  };
  
  // EASTL expects us to define these operators (allocator.h L103)
  bool operator==(const allocator& a, const allocator& b)
  {
    return true; // All are considered equal since they are global malloc/free
  }
  bool operator!=(const allocator& a, const allocator& b)
  {
    return false; // All are considered equal since they are global malloc/free
  }
  
  // Defines the EASTL API glue, so we can set our allocator as the global default allocator
  allocator  gDefaultAllocator;
  allocator* gpDefaultAllocator = &gDefaultAllocator;
  
  allocator* GetDefaultAllocator()
  { 
      return gpDefaultAllocator; 
  }
  
  allocator* SetDefaultAllocator(allocator* pNewAlloc)
  {
    allocator* pOldAlloc = gpDefaultAllocator;
    gpDefaultAllocator = pNewAlloc;
    return pOldAlloc;
  }

}; // CustomAllocator


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

int test_custom_allocator()
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
  EASTL_ASSERT(str == "omfg");

  return EXIT_SUCCESS;
}
