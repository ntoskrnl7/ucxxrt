// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _XMTX
#define _XMTX
#include <yvals.h>

#if _MSC_VER <= 1916
#ifdef _M_CEE_PURE
#define _EXTERN_C_UNLESS_PURE
#define _END_EXTERN_C_UNLESS_PURE
#define _STATIC_UNLESS_PURE // Avoid warning C4640: construction of local static object is not thread-safe (/Wall)
#else // ^^^ _M_CEE_PURE / !_M_CEE_PURE vvv
#define _EXTERN_C_UNLESS_PURE     _EXTERN_C
#define _END_EXTERN_C_UNLESS_PURE _END_EXTERN_C
#define _STATIC_UNLESS_PURE       static
#endif // _M_CEE_PURE
#endif

#include <cstdlib>

#if _KERNEL_MODE
#include <Ldk/windows.h>
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
