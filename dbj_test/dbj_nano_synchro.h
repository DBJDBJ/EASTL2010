#ifndef _DBJ_NANO_CYNCHRO_INC_
#define _DBJ_NANO_CYNCHRO_INC_

/*
(c) 2019-2020 by dbj.org   -- LICENSE DBJ -- https://dbj.org/license_dbj/
*/
#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif

#define NOMINMAX

#undef  min
#define min(x, y) ((x) < (y) ? (x) : (y))

#undef  max
#define max(x, y) ((x) > (y) ? (x) : (y))

#define STRICT 1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifndef WIN32_LEAN_AND_MEAN
#error please include windows before dbj_nano_synchro.h
#endif // WIN32_LEAN_AND_MEAN

/*
ONE SINGLE PER PROCESS dbj nano critical section
Thus using it in one place locks eveything else using it in every other place!

Note: this is obviously WIN32 only
*/

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// /kernel CL switch macro
#ifdef _KERNEL_MODE
#define DBJ_NANO_KERNEL_BUILD
#else
#undef DBJ_NANO_KERNEL_BUILD
#endif


#if defined(__GNUC__) || defined(__INTEL_COMPILER)
#define DBJ_NANO_THREADLOCAL __thread
#elif defined(_MSC_VER)
#define DBJ_NANO_THREADLOCAL __declspec(thread)
#else
#error can not create DBJ_NANO_THREADLOCAL ?
#endif

/*
NOTE! __declspec(thread) is not supported with /kernel
*/
#ifdef DBJ_NANO_KERNEL_BUILD
#undef DBJ_NANO_THREADLOCAL
#define DBJ_NANO_THREADLOCAL
#endif 

/// --------------------------------------------------------------------------------------------
    extern "C" {
        int __cdecl atexit(void(__cdecl*)(void));
    }

    typedef struct
    {
        bool initalized;
        CRITICAL_SECTION crit_sect;
    } dbj_nano_synchro_type;

    dbj_nano_synchro_type* dbj_nano_crit_sect_initor();

    inline void dbj_nano_crit_sect_exit(void)
    {
        dbj_nano_synchro_type crit_ = *dbj_nano_crit_sect_initor();

        if (crit_.initalized)
        {
            DeleteCriticalSection(&crit_.crit_sect);
            crit_.initalized = false;
        }
    }

    inline dbj_nano_synchro_type* dbj_nano_crit_sect_initor()
    {
        // this means: one per process
        static dbj_nano_synchro_type synchro_ = { false };
        if (!synchro_.initalized)
        {
            InitializeCriticalSection(&synchro_.crit_sect);
            synchro_.initalized = true;
            atexit(dbj_nano_crit_sect_exit);
        }

        return &synchro_;
    }

    inline void dbj_synchro_enter() { EnterCriticalSection(&dbj_nano_crit_sect_initor()->crit_sect); }
    inline void dbj_synchro_leave() { LeaveCriticalSection(&dbj_nano_crit_sect_initor()->crit_sect); }

#ifdef DBJ_NANO_LIB_MT
#define DBJ_NANO_LIB_SYNC_ENTER dbj_synchro_enter()
#define DBJ_NANO_LIB_SYNC_LEAVE dbj_synchro_leave()
#else
#define DBJ_NANO_LIB_SYNC_ENTER 
#define DBJ_NANO_LIB_SYNC_LEAVE 
#endif

#ifdef __cplusplus
} // extern "C" 
#endif // __cplusplus

///	-----------------------------------------------------------------------------------------
#ifdef __cplusplus
#pragma region cpp oo sinchronisation
namespace dbj {
    /*
    Be warned. This uses process wide but single instance critical section!

    usage:	void thread_safe_fun()
    {
    dbj::lock_unlock autolock_ ;

    . . .
    wahtever happens here is not entered before it is finished
    . . .

    }
    */
    struct no_copy_no_move
    {
        no_copy_no_move() = default;

        no_copy_no_move(no_copy_no_move const&) = delete;
        no_copy_no_move& operator = (no_copy_no_move const&) = delete;

        no_copy_no_move(no_copy_no_move&&) = delete;
        no_copy_no_move& operator = (no_copy_no_move&&) = delete;
    };

    struct global_lock_unlock final : private no_copy_no_move
    {
        explicit global_lock_unlock() noexcept {
            dbj_synchro_enter();
        }
        ~global_lock_unlock() noexcept {
            dbj_synchro_leave();
        }
    };


    struct local_lock_unlock final : private no_copy_no_move
    {
        explicit local_lock_unlock() noexcept {
            InitializeCriticalSection(&crit_sect_);
            EnterCriticalSection(&crit_sect_);
        }
        ~local_lock_unlock() noexcept {
            DeleteCriticalSection(&crit_sect_);
            LeaveCriticalSection(&crit_sect_);
        }
    private:
        CRITICAL_SECTION crit_sect_{};
    };

} // dbj ns
#pragma endregion
#endif // __cplusplus

#ifdef DBJ_NANO_LIB_MT
#define DBJ_GLOBAL_LOCK dbj::global_lock_unlock padlock_
#define DBJ_LOCAL_LOCK  dbj::local_lock_unlock  padlock_
#else
#define DBJ_GLOBAL_LOCK 
#define DBJ_LOCAL_LOCK  
#endif 

#endif // !_DBJ_NANO_CYNCHRO_INC_
