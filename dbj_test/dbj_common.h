#pragma once

#define VT100_ESC "\x1b["
#define VT100_RESET VT100_ESC "0m"
#define VT100_LIGHT_GRAY VT100_ESC "90m"
#define VT100_LIGHT_BLUE VT100_ESC "94m"
#define VT100_LIGHT_CYAN VT100_ESC "36m"
#define VT100_LIGHT_YELLOW VT100_ESC "33m"
#define VT100_LIGHT_GREEN VT100_ESC "32m"
#define VT100_LIGHT_RED VT100_ESC "31m"
#define VT100_LIGHT_MAGENTA VT100_ESC "35m"

/* (c) 2019-2020 by dbj.org   -- LICENSE DBJ -- https://dbj.org/license_dbj/ */

#undef DBJ_STRINGIZE_
#undef DBJ_STRINGIZE
#define DBJ_STRINGIZE_(x) #x
#define DBJ_STRINGIZE(x) DBJ_STRINGIZE_(x)

#undef  DBJ_PERROR 
#ifdef _DEBUG
#define DBJ_PERROR (perror(__FILE__ " # " DBJ_STRINGIZE(__LINE__))) 
#else
#define DBJ_PERROR
#endif // _DEBUG

#define SX(fmt_, x_) fprintf(stderr, "\n%s " fmt_, #x_, (x_))

#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif

#define PROLOG printf("\n------------------------------------------------------\n" VT100_LIGHT_BLUE __FUNCSIG__  VT100_RESET)

#ifdef _WIN32
#include "win_console.h"
#endif

#include <EASTL/internal/config.h>

// the mandatory user delivered alloc / dealloc functions
#include "mandatory.h"

#include <stdio.h>
#include <time.h>

#ifndef _KERNEL_MODE
#include <string>
#include <vector>
#endif // _KERNEL_MODE

#include "EASTL/string.h"
#include "EASTL/fixed_string.h"
#include "EASTL/vector.h"
#include "EASTL/fixed_vector.h"

#define PROMPT(P_, S_) printf("\n%-24s%24s", P_, S_)
#define SHOW(X_) PROMPT(#X_, X_)
