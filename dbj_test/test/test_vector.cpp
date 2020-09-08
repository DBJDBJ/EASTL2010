#include "../dbj_common.h"

void move_push_back() {
    PROLOG;

  eastl::vector<eastl::string> vec;
  vec.push_back(eastl::string("string"));
  EASTL_ASSERT(vec[0] == "string");

  PROMPT ("Success", "!!" );
}
void move_constructor() {

    PROLOG;

  eastl::vector<eastl::string> vec;
  vec.insert(vec.begin(), 3, eastl::string("string"));
  EASTL_ASSERT(!vec.empty());
  EASTL_ASSERT(vec[0] == "string");

  eastl::vector<eastl::string> moved_vec = std::move(vec);
  EASTL_ASSERT(vec.empty());
  EASTL_ASSERT(!moved_vec.empty());
  EASTL_ASSERT(moved_vec[0] == "string");

  PROMPT("Success", "!!");
}

int eastl_test_vector () {
    PROLOG; 
  move_push_back();
  move_constructor();
  return EXIT_SUCCESS;
}
