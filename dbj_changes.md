
# changes made

## Built with Visual Studio 2019 

1. using CL
2. using CLANG

## vs2019-test main

has been changed .. not dramatically. Just to make it more functional and informative.

## eacompilertraits.h

Line 333

```cpp
// Make sure we get no macro naming conflicts
// DBJ added the following line
#if ! defined(EA_COMPILER_CLANG)
#  undef CLANG_ALIGNED 
#  undef CLANG_PACKED 
#endif
```

That makes build possible with CLANG 10.x, as packaged under Visual Studio 2019. Problem was in `fixedpool.h` lines 109 to 145.