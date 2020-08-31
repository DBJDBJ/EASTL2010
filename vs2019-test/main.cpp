#include <stdio.h>
#include <time.h>

#include <string>
#include <vector>

#include "EASTL/string.h"
#include "EASTL/vector.h"

static void performance_test(size_t loop_length, const char * str_specimen_ )
{
    clock_t start{};
    clock_t end{};

#define double_diff_per_second(E_, S_)  (double( double(E_) - double(S_)) / double(CLOCKS_PER_SEC))

    // EA

    printf("EA STL     ");

    eastl::string strEA = str_specimen_;
    eastl::vector<eastl::string> vec_EA;

    start = clock();
    for (size_t i = 0; i < loop_length; i++)
    {
        vec_EA.push_back(strEA);
    }

    end = clock();
    printf("%f sec\n", double_diff_per_second(end,start) );

    // Standard
    printf("\nStandard STL     ");

    std::string strStandard = str_specimen_;
    std::vector<std::string> vec_Standard;

    start = clock();
    for (size_t i = 0; i < loop_length; i++)
    {
        vec_Standard.push_back(strStandard);
    }

    end = clock();
    printf("%f sec\n", double_diff_per_second(end, start) );
}


int main(const int argc, char ** argv)
{

    enum { dbj_test_loop_size_ = 1000000 * 10 };

// making this laqrger than small size optimization (SSO) value 
// makes very large difference in results
// #define DBJ_STRING_SPECIMEN "Hello"
#define DBJ_STRING_SPECIMEN "Hello young fellow from the shallow, why are you so mellow? Perhaps thy guilty conciousness is the badfellow? "

#ifdef _DEBUG
    printf("\nDEBUG build");
#else
    printf("\nRELEASE build");
#endif

    printf("\n [version %s] __cplusplus: %lu, _MSC_FULL_VER : %d\n", 
        __TIMESTAMP__ , __cplusplus, _MSC_FULL_VER);

    printf("\nSpecimen: \"%s\" (length: %d)\n", DBJ_STRING_SPECIMEN, strlen(DBJ_STRING_SPECIMEN));
    printf("\nTest Loop length: %03.1f milions\n\n", double(dbj_test_loop_size_) / 1000000 );

    try {
        performance_test(dbj_test_loop_size_, DBJ_STRING_SPECIMEN);
    }
    catch (std::exception& x_)
    {
        printf("\nstd::exception \"%s\"", x_.what() );
    }
    catch (...)
    {
        printf("\nUnknown exception ...?");
    }

    printf("\n\nDone ...\n\n");


}


// needed by EASTL - taken from EASTL's example/example1.cpp
//

// EASTL expects us to define these, see allocator.h line 194
void* operator new[](size_t size, const char* pName, int flags,
	unsigned debugFlags, const char* file, int line)
{
	// return malloc(size);
	return calloc(1,size);
}
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset,
	const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	// this allocator doesn't support alignment
	EASTL_ASSERT(alignment <= 8);
    // return malloc(size);
    return calloc(1, size);
}

// EASTL also wants us to define this (see string.h line 197)
int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t* pFormat, va_list arguments)
{
#ifdef _MSC_VER
	return _vsnprintf(pDestination, n, pFormat, arguments);
#else
	return vsnprintf(pDestination, n, pFormat, arguments);
#endif
}
