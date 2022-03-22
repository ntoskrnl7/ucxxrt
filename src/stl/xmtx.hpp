// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _XMTX
#define _XMTX
#include <yvals.h>

#include <cstdlib>

#if _KERNEL_MODE
#pragma pack(push, 8)
typedef struct _RTL_CRITICAL_SECTION_DEBUG {
    WORD   Type;
    WORD   CreatorBackTraceIndex;
    struct _RTL_CRITICAL_SECTION* CriticalSection;
    LIST_ENTRY ProcessLocksList;
    DWORD EntryCount;
    DWORD ContentionCount;
    DWORD Flags;
    WORD   CreatorBackTraceIndexHigh;
    WORD   SpareWORD;
} RTL_CRITICAL_SECTION_DEBUG, * PRTL_CRITICAL_SECTION_DEBUG, RTL_RESOURCE_DEBUG, * PRTL_RESOURCE_DEBUG;



typedef struct _RTL_CRITICAL_SECTION {
    PRTL_CRITICAL_SECTION_DEBUG DebugInfo;

    //
    //  The following three fields control entering and exiting the critical
    //  section for the resource
    //

    LONG LockCount;
    LONG RecursionCount;
    HANDLE OwningThread;        // from the thread's ClientId->UniqueThread
    HANDLE LockSemaphore;
    ULONG_PTR SpinCount;        // force size on 64-bit systems when packed
} RTL_CRITICAL_SECTION, * PRTL_CRITICAL_SECTION;

#pragma pack(pop)


typedef RTL_CRITICAL_SECTION CRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION PCRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION LPCRITICAL_SECTION;

typedef RTL_CRITICAL_SECTION_DEBUG CRITICAL_SECTION_DEBUG;
typedef PRTL_CRITICAL_SECTION_DEBUG PCRITICAL_SECTION_DEBUG;
typedef PRTL_CRITICAL_SECTION_DEBUG LPCRITICAL_SECTION_DEBUG;
#else
#include <Windows.h>
#endif

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, 3)
#pragma push_macro("new")
#undef new

_EXTERN_C_UNLESS_PURE

using _Rmtx = CRITICAL_SECTION;

#ifdef _M_CEE_PURE
void __clrcall _Mtxinit(_Rmtx*) noexcept;
void __clrcall _Mtxdst(_Rmtx*) noexcept;
void __clrcall _Mtxlock(_Rmtx*) noexcept;
void __clrcall _Mtxunlock(_Rmtx*) noexcept;

#else // _M_CEE_PURE
_MRTIMP2 void __cdecl _Mtxinit(_Rmtx*) noexcept;
_MRTIMP2 void __cdecl _Mtxdst(_Rmtx*) noexcept;
_MRTIMP2 void __cdecl _Mtxlock(_Rmtx*) noexcept;
_MRTIMP2 void __cdecl _Mtxunlock(_Rmtx*) noexcept;
#endif // _M_CEE_PURE

_END_EXTERN_C_UNLESS_PURE

#pragma pop_macro("new")
#pragma warning(pop)
#pragma pack(pop)
#endif // _XMTX