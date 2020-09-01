#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <time.h>

#ifndef _KERNEL_MODE
#include <string>
#include <vector>
#endif // _KERNEL_MODE

#include "test.h"

#include "EASTL/string.h"
#include "EASTL/vector.h"

static void performance_test(size_t loop_length, const char * str_specimen_ )
{
    clock_t start{};
    clock_t end{};

    auto diff_per_second = [](double E_, double S_) constexpr ->double
    {
        return ((E_) - (S_)) / (CLOCKS_PER_SEC);
    };

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
    printf("%f sec\n", diff_per_second(end,start) );


#ifndef _KERNEL_MODE
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
    printf("%f sec\n", diff_per_second(end, start) );
#endif // _KERNEL_MODE
}


#define SHORT_SPECIMEN

#define OTHER_TESTS
#ifdef OTHER_TESTS

int test_custom_allocator();
int eastl_test_vector();
int eastl_test_vector_map();
int test_hash_map_string();
int test_malloc_aligned();

#endif // OTHER_TESTS


int main(const int argc, char ** argv)
{
    constexpr auto dbj_test_loop_size_ = 1000000 / 10;

// making this laqrger than small size optimization (SSO) value 
// makes very large difference in comparisons with EASTL
#ifdef SHORT_SPECIMEN
#define DBJ_STRING_SPECIMEN "Hello"
#else
#define DBJ_STRING_SPECIMEN "Hello young fellow from the shallow, why are you so mellow? Perhaps thy friend is the badfellow? "
#endif

#ifdef _KERNEL_MODE
    printf("\n\n******************* KERNEL MODE ****************************\n\n");
#endif // _KERNEL_MODE


#ifdef __clang__
    printf("\n\nCLANG ");
    printf("\n\n __VERSION__ : %d\n", __VERSION__);
#else
    printf("\n\nCL    ");
    printf("\n\n _MSC_FULL_VER : %d\n", _MSC_FULL_VER);
#endif

    printf("\n% s[version % s]  \n\n__cplusplus: %lu ", argv[0], __TIMESTAMP__, __cplusplus);

#ifdef _DEBUG
    printf("\tDEBUG build");
#else
    printf("\tRELEASE build");
#endif


    printf("\n\nSpecimen: \"%s\" (length: %zd)\n", DBJ_STRING_SPECIMEN, strlen(DBJ_STRING_SPECIMEN));
    printf("\n\nTest Loop length: %03.1f milions\n\n", double(dbj_test_loop_size_) / 1000000 );

#ifndef _KERNEL_MODE
    try {
#endif // _KERNEL_MODE
        performance_test(dbj_test_loop_size_, DBJ_STRING_SPECIMEN);
#ifndef _KERNEL_MODE
    }
    catch (std::exception& x_)
    {
        printf("\nstd::exception \"%s\"", x_.what() );
    }
    catch (...)
    {
        printf("\nUnknown exception ...?");
    }
#endif // _KERNEL_MODE

#ifdef OTHER_TESTS

    test_malloc_aligned();
    test_custom_allocator();
    eastl_test_vector();
    eastl_test_vector_map();
    test_hash_map_string();

#endif // OTHER_TESTS

    printf("\n\nDone ...\n\n");


}