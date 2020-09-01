#include <stdio.h>
#include <time.h>

#include <string>
#include <vector>

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
}


#define SHORT_SPECIMEN

#define OTHER_TESTS
#ifdef OTHER_TESTS

int eastl_test_vector();
int eastl_test_vector_map();

#endif // OTHER_TESTS


int main(const int argc, char ** argv)
{
#ifdef OTHER_TESTS

    eastl_test_vector();
    eastl_test_vector_map();

#endif // OTHER_TESTS

    constexpr auto dbj_test_loop_size_ = 1000000 * 10;

// making this laqrger than small size optimization (SSO) value 
// makes very large difference in comparisons with EASTL
#ifdef SHORT_SPECIMEN
#define DBJ_STRING_SPECIMEN "Hello"
#else
#define DBJ_STRING_SPECIMEN "Hello young fellow from the shallow, why are you so mellow? Perhaps thy friend is the badfellow? "
#endif

#ifdef __clang__
    printf("\nCLANG ");
    printf("\n\n __VERSION__ : %d\n", __VERSION__);
#else
    printf("\nCL    ");
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