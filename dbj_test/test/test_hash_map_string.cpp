
// basic usage of some EASTL containers.

#include "../dbj_common.h"

#include <EASTL/hash_map.h>
#include <EASTL/string.h>

#if 0

// for some reason this is required here ... but not elsewhere?
// EASTL also wants us to define this (see string.h line 197)
extern "C" static int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t* pFormat, va_list arguments)
{
#ifdef _MSC_VER
	return _vsnprintf(pDestination, n, pFormat, arguments);
#else
	return vsnprintf(pDestination, n, pFormat, arguments);
#endif
}

#endif // 0

static void test_hash_map()
{
  eastl::hash_map<eastl::string, eastl::string> map;
  map["key1"] = "value1";
  map["key2"] = "value2";
  EASTL_ASSERT(map.find("key1")->second == "value1");
}

static void test_string()
{
  eastl::string str;
  str += "testing ";
  str += "simple ";
  str += "string ";
  str += "concat";
  EASTL_ASSERT(str == "testing simple string concat");
  
  str.sprintf("%d", 20);
  EASTL_ASSERT(str == "20");
}

int test_hash_map_string( )
{
  test_hash_map();
  test_string();
  return EXIT_SUCCESS;
}
