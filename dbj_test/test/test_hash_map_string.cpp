
// basic usage of some EASTL containers.

#include "../dbj_common.h"

#include <EASTL/hash_map.h>
#include <EASTL/string.h>

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
	printf("\n------------------------------------------------------\n" VT100_LIGHT_BLUE __FUNCSIG__  VT100_RESET);

  test_hash_map();
  test_string();
  return EXIT_SUCCESS;
}
