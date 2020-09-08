

#include <stdio.h>
#include <time.h>

#ifndef _KERNEL_MODE
#include <string>
#include <vector>
#endif // _KERNEL_MODE

#define PROMPT(P_, S_) printf("\n%-24s%24s", P_, S_)
#define SHOW(X_) PROMPT(#X_, X_)

#include "dbj_common.h"

#include "EASTL/string.h"
#include "EASTL/vector.h"

#undef SHORT_SPECIMEN

// making this larger than small size optimization (SSO) STD STL value 
// makes very large difference in comparisons with EASTL
static const char * const specimen[]{
"Hello",
"Hello young fellow from the shallow, why are you so mellow? Perhaps thy friend is the badfellow?"
};


static void performance_test(size_t loop_length, unsigned specimen_index_  )
{

    EASTL_ASSERT( specimen_index_ < 2 );

    const char* str_specimen_ = specimen[specimen_index_];

    printf("\n------------------------------------------------------\n" VT100_LIGHT_BLUE __FUNCSIG__  VT100_RESET);

    printf( VT100_LIGHT_CYAN );
    printf( "\n\nSpecimen: " VT100_LIGHT_GREEN " '%s' " VT100_LIGHT_CYAN 
        "(length: %zd)", str_specimen_, strlen(str_specimen_));

    printf("\nTest Loop length: %03.1f milions" VT100_RESET, double(loop_length) / 1000000);

    clock_t start{};
    clock_t end{};

    auto diff_per_second = [](double E_, double S_) constexpr ->double
    {
        return ((E_) - (S_)) / (CLOCKS_PER_SEC);
    };

    // EA

    eastl::string strEA = str_specimen_;
    eastl::vector<eastl::string> vec_EA;

    start = clock();
    for (size_t i = 0; i < loop_length; i++)
    {
        vec_EA.push_back(strEA);
    }

    end = clock();

    char buff[0xFF]{0};
    sprintf_s( buff, 0xFF, "%f" , diff_per_second(end, start) );
    PROMPT("EA STL seconds", buff  );


    // no std lib in kernel mode 
#ifndef _KERNEL_MODE
    // Standard
    {
        std::string strStandard = str_specimen_;
        std::vector<std::string> vec_Standard;

        start = clock();
        for (size_t i = 0; i < loop_length; i++)
        {
            vec_Standard.push_back(strStandard);
        }

        end = clock();
        char buff[0xFF]{ 0 };
        sprintf_s(buff, 0xFF, "%f", diff_per_second(end, start));
        PROMPT("STD STL seconds", buff );
    }

#else  // ! _KERNEL_MODE
    printf("\n Can not used std lib in kernel mode.");
#endif //!  _KERNEL_MODE
}

#define OTHER_TESTS
#ifdef OTHER_TESTS

#ifdef TEST_CUSTOM_ALLOCATOR
int test_custom_allocator();
#endif

int eastl_test_vector();
int eastl_test_vector_map();
int test_hash_map_string();

#ifdef TEST_MALLOC_ALIGNED
int test_malloc_aligned() noexcept ;
#endif

#endif // OTHER_TESTS

int main(const int argc, char ** argv)
{
    constexpr auto dbj_test_loop_size_ = 1000000 * 2 ;

#ifdef _WIN32
    win_enable_vt_100_and_unicode();

    // warning: will exit(-1) on mistakes made
    // font name is case sensitive
    // if your font name is not found default is used
    win_set_console_font(L"Consolas", 24);
#endif

    printf(VT100_RESET "\n\n" );

#ifdef _KERNEL_MODE
    SHOW(_KERNEL_MODE);
#endif // _KERNEL_MODE

#ifdef __clang__
    SHOW(__VERSION__);
#else
    SHOW(_CRT_STRINGIZE(_MSC_FULL_VER));
#endif

    SHOW( _CRT_STRINGIZE( __cplusplus )  );
    SHOW(__TIMESTAMP__);

#ifdef _DEBUG
    SHOW(_CRT_STRINGIZE(_DEBUG));
#else
    SHOW(_CRT_STRINGIZE(NDEBUG));
#endif
    printf(VT100_RESET);

#ifndef _KERNEL_MODE
    try {
#endif // _KERNEL_MODE
        performance_test(dbj_test_loop_size_, 0); 
        performance_test(dbj_test_loop_size_, 1);


#ifdef OTHER_TESTS
#ifdef TEST_MALLOC_ALIGNED
    test_malloc_aligned();
#endif
#ifdef TEST_CUSTOM_ALLOCATOR
    test_custom_allocator();
#endif
    eastl_test_vector();
    eastl_test_vector_map();
    test_hash_map_string();

#endif // OTHER_TESTS

#ifndef _KERNEL_MODE
    }
    catch (std::exception& x_)
    {
        printf("\nstd::exception \"%s\"", x_.what());
    }
    catch (...)
    {
        printf("\nUnknown exception ...?");
    }
#endif // _KERNEL_MODE

    printf(VT100_LIGHT_GREEN "\n\nDone ...\n\n" VT100_RESET);


}