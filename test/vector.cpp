#include <assert.h>
#include <stdio.h>

#include <EASTL/string.h>
#include <EASTL/vector.h>

#ifdef EA_COMPILER_HAS_MOVE_SEMANTICS

void move_push_back() {
  printf ( "\n\n" __FUNCSIG__ );

  eastl::vector<eastl::string> vec;
  vec.push_back(eastl::string("string"));
  assert(vec[0] == "string");

  printf ( "\tsuccess!!" );
}
void move_constructor() {

  printf("\n\n" __FUNCSIG__);

  eastl::vector<eastl::string> vec;
  vec.insert(vec.begin(), 3, eastl::string("string"));
  assert(!vec.empty());
  assert(vec[0] == "string");

  eastl::vector<eastl::string> moved_vec = std::move(vec);
  assert(vec.empty());
  assert(!moved_vec.empty());
  assert(moved_vec[0] == "string");

  printf ( "\tsuccess!!" );
}

#endif

int eastl_test_vector () {
#ifdef EA_COMPILER_HAS_MOVE_SEMANTICS
  move_push_back();
  move_constructor();
#else
#error This compiler has no move semantics?
#endif
  return EXIT_SUCCESS;
}
