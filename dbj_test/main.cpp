
#include "dbj_common.h"

#define OTHER_TESTS
#undef SHORT_SPECIMEN
#define ONE_MILLION 1000000

static void performance_test(size_t loop_length, unsigned specimen_index_) noexcept
{
// of course using small string 
// makes very large difference in comparisons with EASTL
	static const char* const specimen[]{
	"Hello",
	"Hello young fellow from the shallow, why are you so mellow?"
	" Perhaps thy friend is a badfellow?"
	};

	PROLOG;

	EASTL_ASSERT(specimen_index_ < 2);

	const char* str_specimen_ = specimen[specimen_index_];

	printf(VT100_LIGHT_GREEN);
	PROMPT("specimen", str_specimen_);
	printf(VT100_RESET);

	char buff[0xFF]{ 0 };
	sprintf_s(buff, 0xFF, "%03.1f", double(loop_length) / ONE_MILLION);

	PROMPT("Test Loop length (millions)", buff);

	auto test_loop = [&](auto str_specimen, auto vec_of_strings, auto rezult_prompt) {

		auto seconds_ = [](double E_, double S_) constexpr ->double
		{
			return ((E_)-(S_)) / (CLOCKS_PER_SEC);
		};

		clock_t start{};
		clock_t end{};

		start = clock();
		for (size_t i = 0; i < loop_length; i++)
		{
			vec_of_strings.push_back(str_specimen);
		}
		end = clock();

		char buff[0xFF]{ 0 };
		sprintf_s(buff, 0xFF, "%f", seconds_(end, start));
		PROMPT(rezult_prompt, buff);
	};

	test_loop(
		eastl::string{ str_specimen_ },
		eastl::vector<eastl::string>{},
		"EA STL (seconds)"
	);

	// no std lib in kernel mode 
#ifndef _KERNEL_MODE

	test_loop(
		std::string{ str_specimen_ },
		std::vector<std::string>{},
		"STD STL (seconds)"
	);

#else  // ! _KERNEL_MODE
	printf("\n Can not use std lib in kernel mode.");
#endif //!  _KERNEL_MODE
} // performance_test

#ifdef OTHER_TESTS

int eastl_test_vector();
int eastl_test_vector_map();
int test_hash_map_string();

#endif // OTHER_TESTS

int main(const int argc, char** argv)
{
	constexpr auto dbj_test_loop_size_ = ONE_MILLION * 2;

#ifdef _WIN32
	win_enable_vt_100_and_unicode();
	// warning: will exit(-1) on mistakes made
	// font name is case sensitive
	// if your font name is not found default is used
	win_set_console_font(L"Consolas", 24);
#endif

	printf(VT100_LIGHT_BLUE);
	PROMPT("DBJ EASTL2010", "*********************************" );
	printf(VT100_RESET);

#ifdef _KERNEL_MODE
	SHOW(_KERNEL_MODE);
#endif // _KERNEL_MODE

#ifdef __clang__
	SHOW(__VERSION__);
#else
	SHOW(DBJ_STRINGIZE(_MSC_FULL_VER));
#endif

	SHOW(DBJ_STRINGIZE(__cplusplus));
	SHOW(__TIMESTAMP__);

#ifdef _DEBUG
	SHOW(DBJ_STRINGIZE(_DEBUG));
#else
	SHOW(DBJ_STRINGIZE(NDEBUG));
#endif
	printf(VT100_RESET);

#ifndef _KERNEL_MODE
	try {
#endif // _KERNEL_MODE
		performance_test(dbj_test_loop_size_, 0);
		performance_test(dbj_test_loop_size_, 1);

#ifdef OTHER_TESTS
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