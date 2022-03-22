// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// mutex support

#include "xmtx.hpp"


#if _KERNEL_MODE
WINBASEAPI
VOID
WINAPI
InitializeCriticalSection(
	_Out_ LPCRITICAL_SECTION lpCriticalSection
)
{
	RtlZeroMemory(lpCriticalSection, sizeof(CRITICAL_SECTION));

	lpCriticalSection->DebugInfo = (PRTL_CRITICAL_SECTION_DEBUG)ExAllocatePoolWithTag(NonPagedPool, sizeof(ERESOURCE), 'rsrc');
	if (lpCriticalSection->DebugInfo) {
		ExInitializeResourceLite((PERESOURCE)lpCriticalSection->DebugInfo);
	}
}

WINBASEAPI
VOID
WINAPI
EnterCriticalSection(
	_Inout_ LPCRITICAL_SECTION lpCriticalSection
)
{
	if (lpCriticalSection->DebugInfo) {

		ULONG_PTR SpinCount = lpCriticalSection->SpinCount;
		PERESOURCE Resource = (PERESOURCE)lpCriticalSection->DebugInfo;
		ULONG WaiterCount;

		KeEnterCriticalRegion();

		WaiterCount = ExGetExclusiveWaiterCount(Resource);

		if (WaiterCount) {
			if (!ExIsResourceAcquiredLite(Resource)) {

				KdBreakPoint();

				//
				// 스핀 카운트 횟수만큼 Busy-Waiting을 시도합니다.
				//

				for (; SpinCount; SpinCount--) {
					if (ExAcquireResourceExclusiveLite((PERESOURCE)lpCriticalSection->DebugInfo, FALSE)) {
						return;
					}
				}
			}
		}

		ExAcquireResourceExclusiveLite((PERESOURCE)lpCriticalSection->DebugInfo, TRUE);

	}
}

WINBASEAPI
VOID
WINAPI
LeaveCriticalSection(
	_Inout_ LPCRITICAL_SECTION lpCriticalSection
)
{
	if (lpCriticalSection->DebugInfo) {
		ExReleaseResourceLite((PERESOURCE)lpCriticalSection->DebugInfo);
		KeLeaveCriticalRegion();
	}
}

WINBASEAPI
_Must_inspect_result_
BOOL
WINAPI
InitializeCriticalSectionAndSpinCount(
	_Out_ LPCRITICAL_SECTION lpCriticalSection,
	_In_ DWORD dwSpinCount
)
{
	InitializeCriticalSection(lpCriticalSection);

	if (KeQueryMaximumProcessorCount() <= 1) {
		lpCriticalSection->SpinCount = 0;
	}
	else if (dwSpinCount) {
		lpCriticalSection->SpinCount = dwSpinCount & 0xFFFFFF;
	}
	else {
		lpCriticalSection->SpinCount = 0x20007D0;
	}

	return TRUE;
}


WINBASEAPI
VOID
WINAPI
DeleteCriticalSection(
	_Inout_ LPCRITICAL_SECTION lpCriticalSection
)
{
	if (lpCriticalSection->DebugInfo) {
		ExDeleteResourceLite((PERESOURCE)lpCriticalSection->DebugInfo);
		ExFreePoolWithTag(lpCriticalSection->DebugInfo, 'rsrc');
	}

}
#endif


_EXTERN_C_UNLESS_PURE

// Win32 critical sections are recursive

void __CLRCALL_PURE_OR_CDECL _Mtxinit(_Rmtx* _Mtx) noexcept { // initialize mutex
    InitializeCriticalSectionAndSpinCount(_Mtx, 4000);
}

void __CLRCALL_PURE_OR_CDECL _Mtxdst(_Rmtx* _Mtx) noexcept { // delete mutex
    DeleteCriticalSection(_Mtx);
}

_RELIABILITY_CONTRACT
void __CLRCALL_PURE_OR_CDECL _Mtxlock(_Rmtx* _Mtx) noexcept { // lock mutex
#ifdef _M_CEE
    System::Threading::Thread::BeginThreadAffinity();
#endif // _M_CEE
    EnterCriticalSection(_Mtx);
}

_RELIABILITY_CONTRACT
void __CLRCALL_PURE_OR_CDECL _Mtxunlock(_Rmtx* _Mtx) noexcept { // unlock mutex
    LeaveCriticalSection(_Mtx);
#ifdef _M_CEE
    System::Threading::Thread::EndThreadAffinity();
#endif // _M_CEE
}

_END_EXTERN_C_UNLESS_PURE