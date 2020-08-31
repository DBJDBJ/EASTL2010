//**********************************

Test project for stackoverflow question "EASTL performance":

	http://stackoverflow.com/q/28839083/12711

//**********************************

Open the solution file vs2010-test/eastl-test/eastl-test.sln

The only source file that wasn't part of the original EASTL distribution is:

	vs2010-test/main.cpp

It contains the code in the stackoverflow question plus the various scaffolding to make it compilable.

The project is contained in a directory tree that consists of the EASTL distrbution from:

	https://github.com/paulhodge/EASTL


The project does incorporate by reference several necessary EASTL source files that make up the 
non-header portion of the EASTL library:

	src/allocator.cpp
	src/assert.cpp
	src/fixed_pool.cpp
	src/hashtable.cpp
	src/red_black_tree.cpp
	src/string.cpp

Build the project in "Release" mode and run it outside the debugger (for example, in the IDE use Ctrl-F5
to run it).  You'll get results similar to the following:

	EA       0.075000
	Standard 0.081000

Then run the **same** **Release** build of the program under the debugger (use the F5 key in the IDE, or
you can use the "cdb" debugger to run it from the command line with similar results). Now the EASTL results 
will be an order of magnitude worse:

	EA       0.870000
	Standard 0.080000

The results are similar for both 32-bit and 64-bit builds.
