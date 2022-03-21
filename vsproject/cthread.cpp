// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// thread functions

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <process.h>
#include <xthreads.h>

#if _KERNEL_MODE
#else
#include <Windows.h>
#endif

EXTERN_C ULONG_PTR __GetCurrentThreadId();

EXTERN_C
_When_(Timeout == NULL, _IRQL_requires_max_(APC_LEVEL))
_When_(Timeout->QuadPart != 0, _IRQL_requires_max_(APC_LEVEL))
_When_(Timeout->QuadPart == 0, _IRQL_requires_max_(DISPATCH_LEVEL))
NTSYSAPI
NTSTATUS
NTAPI
ZwWaitForSingleObject(
    _In_ HANDLE Handle,
    _In_ BOOLEAN Alertable,
    _In_opt_ PLARGE_INTEGER Timeout
);

typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemBasicInformation,
    SystemProcessorInformation,             // obsolete...delete
    SystemPerformanceInformation,
    SystemTimeOfDayInformation,
    SystemPathInformation,
    SystemProcessInformation,
    SystemCallCountInformation,
    SystemDeviceInformation,
    SystemProcessorPerformanceInformation,
    SystemFlagsInformation,
    SystemCallTimeInformation,
    SystemModuleInformation,
    SystemLocksInformation,
    SystemStackTraceInformation,
    SystemPagedPoolInformation,
    SystemNonPagedPoolInformation,
    SystemHandleInformation,
    SystemObjectInformation,
    SystemPageFileInformation,
    SystemVdmInstemulInformation,
    SystemVdmBopInformation,
    SystemFileCacheInformation,
    SystemPoolTagInformation,
    SystemInterruptInformation,
    SystemDpcBehaviorInformation,
    SystemFullMemoryInformation,
    SystemLoadGdiDriverInformation,
    SystemUnloadGdiDriverInformation,
    SystemTimeAdjustmentInformation,
    SystemSummaryMemoryInformation,
    SystemMirrorMemoryInformation,
    SystemPerformanceTraceInformation,
    SystemObsolete0,
    SystemExceptionInformation,
    SystemCrashDumpStateInformation,
    SystemKernelDebuggerInformation,
    SystemContextSwitchInformation,
    SystemRegistryQuotaInformation,
    SystemExtendServiceTableInformation,
    SystemPrioritySeperation,
    SystemVerifierAddDriverInformation,
    SystemVerifierRemoveDriverInformation,
    SystemProcessorIdleInformation,
    SystemLegacyDriverInformation,
    SystemCurrentTimeZoneInformation,
    SystemLookasideInformation,
    SystemTimeSlipNotification,
    SystemSessionCreate,
    SystemSessionDetach,
    SystemSessionInformation,
    SystemRangeStartInformation,
    SystemVerifierInformation,
    SystemVerifierThunkExtend,
    SystemSessionProcessInformation,
    SystemLoadGdiDriverInSystemSpace,
    SystemNumaProcessorMap,
    SystemPrefetcherInformation,
    SystemExtendedProcessInformation,
    SystemRecommendedSharedDataAlignment,
    SystemComPlusPackage,
    SystemNumaAvailableMemory,
    SystemProcessorPowerInformation,
    SystemEmulationBasicInformation,
    SystemEmulationProcessorInformation,
    SystemExtendedHandleInformation,
    SystemLostDelayedWriteInformation,
    SystemBigPoolInformation,
    SystemSessionPoolTagInformation,
    SystemSessionMappedViewInformation,
    SystemHotpatchInformation,
    SystemObjectSecurityMode,
    SystemWatchdogTimerHandler,
    SystemWatchdogTimerInformation,
    SystemLogicalProcessorInformation,
    SystemWow64SharedInformation,
    SystemRegisterFirmwareTableInformationHandler,
    SystemFirmwareTableInformation,
    SystemModuleInformationEx,
    SystemVerifierTriageInformation,
    SystemSuperfetchInformation,
    SystemMemoryListInformation,
    SystemFileCacheInformationEx,
    MaxSystemInfoClass  // MaxSystemInfoClass should always be the last enum
} SYSTEM_INFORMATION_CLASS;

#if defined(_WIN64)
typedef ULONG SYSINF_PAGE_COUNT;
#else
typedef SIZE_T SYSINF_PAGE_COUNT;
#endif

typedef struct _SYSTEM_BASIC_INFORMATION {
    ULONG Reserved;
    ULONG TimerResolution;
    ULONG PageSize;
    SYSINF_PAGE_COUNT NumberOfPhysicalPages;
    SYSINF_PAGE_COUNT LowestPhysicalPageNumber;
    SYSINF_PAGE_COUNT HighestPhysicalPageNumber;
    ULONG AllocationGranularity;
    ULONG_PTR MinimumUserModeAddress;
    ULONG_PTR MaximumUserModeAddress;
    ULONG_PTR ActiveProcessorsAffinityMask;
    CCHAR NumberOfProcessors;
} SYSTEM_BASIC_INFORMATION, * PSYSTEM_BASIC_INFORMATION;




#define PROCESSOR_ARCHITECTURE_INTEL            0
#define PROCESSOR_ARCHITECTURE_MIPS             1
#define PROCESSOR_ARCHITECTURE_ALPHA            2
#define PROCESSOR_ARCHITECTURE_PPC              3
#define PROCESSOR_ARCHITECTURE_SHX              4
#define PROCESSOR_ARCHITECTURE_ARM              5
#define PROCESSOR_ARCHITECTURE_IA64             6
#define PROCESSOR_ARCHITECTURE_ALPHA64          7
#define PROCESSOR_ARCHITECTURE_MSIL             8
#define PROCESSOR_ARCHITECTURE_AMD64            9
#define PROCESSOR_ARCHITECTURE_IA32_ON_WIN64    10
#define PROCESSOR_ARCHITECTURE_NEUTRAL          11

#define PROCESSOR_INTEL_386     386
#define PROCESSOR_INTEL_486     486
#define PROCESSOR_INTEL_PENTIUM 586
#define PROCESSOR_INTEL_IA64    2200
#define PROCESSOR_AMD_X8664     8664
#define PROCESSOR_MIPS_R4000    4000    // incl R4101 & R3910 for Windows CE
#define PROCESSOR_ALPHA_21064   21064
#define PROCESSOR_PPC_601       601
#define PROCESSOR_PPC_603       603
#define PROCESSOR_PPC_604       604
#define PROCESSOR_PPC_620       620
#define PROCESSOR_HITACHI_SH3   10003   // Windows CE
#define PROCESSOR_HITACHI_SH3E  10004   // Windows CE
#define PROCESSOR_HITACHI_SH4   10005   // Windows CE
#define PROCESSOR_MOTOROLA_821  821     // Windows CE
#define PROCESSOR_SHx_SH3       103     // Windows CE
#define PROCESSOR_SHx_SH4       104     // Windows CE
#define PROCESSOR_STRONGARM     2577    // Windows CE - 0xA11
#define PROCESSOR_ARM720        1824    // Windows CE - 0x720
#define PROCESSOR_ARM820        2080    // Windows CE - 0x820
#define PROCESSOR_ARM920        2336    // Windows CE - 0x920
#define PROCESSOR_ARM_7TDMI     70001   // Windows CE
#define PROCESSOR_OPTIL         0x494f  // MSIL

typedef struct _SYSTEM_PROCESSOR_INFORMATION {
    USHORT ProcessorArchitecture;
    USHORT ProcessorLevel;
    USHORT ProcessorRevision;
    USHORT Reserved;
    ULONG ProcessorFeatureBits;

} SYSTEM_PROCESSOR_INFORMATION, * PSYSTEM_PROCESSOR_INFORMATION;

EXTERN_C
NTSYSAPI
NTSTATUS
NTAPI
NtQuerySystemInformation(
    __in SYSTEM_INFORMATION_CLASS SystemInformationClass,
    __out_bcount_opt(SystemInformationLength) PVOID SystemInformation,
    __in ULONG SystemInformationLength,
    __out_opt PULONG ReturnLength
);



#pragma warning(disable:4201)
typedef struct _SYSTEM_INFO {
    union {
        DWORD dwOemId;          // Obsolete field...do not use
        struct {
            WORD wProcessorArchitecture;
            WORD wReserved;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
    DWORD dwPageSize;
    LPVOID lpMinimumApplicationAddress;
    LPVOID lpMaximumApplicationAddress;
    DWORD_PTR dwActiveProcessorMask;
    DWORD dwNumberOfProcessors;
    DWORD dwProcessorType;
    DWORD dwAllocationGranularity;
    WORD wProcessorLevel;
    WORD wProcessorRevision;
} SYSTEM_INFO, * LPSYSTEM_INFO;
#pragma warning(default:4201)

WINBASEAPI
VOID
WINAPI
GetNativeSystemInfo(
    _Out_ LPSYSTEM_INFO lpSystemInfo
)
{
    NTSTATUS Status;
    SYSTEM_BASIC_INFORMATION BasicInfo;
    SYSTEM_PROCESSOR_INFORMATION ProcessorInfo;

    RtlZeroMemory(lpSystemInfo, sizeof(SYSTEM_INFO));

    Status = NtQuerySystemInformation(
        SystemBasicInformation,
        &BasicInfo,
        sizeof(BasicInfo),
        NULL
    );
    if (!NT_SUCCESS(Status)) {
        return;
    }

    Status = NtQuerySystemInformation(
        SystemProcessorInformation,
        &ProcessorInfo,
        sizeof(ProcessorInfo),
        NULL
    );
    if (!NT_SUCCESS(Status)) {
        return;
    }

    lpSystemInfo->wProcessorArchitecture = ProcessorInfo.ProcessorArchitecture;
    lpSystemInfo->wReserved = 0;
    lpSystemInfo->dwPageSize = BasicInfo.PageSize;
    lpSystemInfo->lpMinimumApplicationAddress = (LPVOID)BasicInfo.MinimumUserModeAddress;
    lpSystemInfo->lpMaximumApplicationAddress = (LPVOID)BasicInfo.MaximumUserModeAddress;
    lpSystemInfo->dwActiveProcessorMask = BasicInfo.ActiveProcessorsAffinityMask;
    lpSystemInfo->dwNumberOfProcessors = BasicInfo.NumberOfProcessors;
    lpSystemInfo->wProcessorLevel = ProcessorInfo.ProcessorLevel;
    lpSystemInfo->wProcessorRevision = ProcessorInfo.ProcessorRevision;

    if (ProcessorInfo.ProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL) {
        if (ProcessorInfo.ProcessorLevel == 3) {
            lpSystemInfo->dwProcessorType = PROCESSOR_INTEL_386;
        }
        else if (ProcessorInfo.ProcessorLevel == 4) {
            lpSystemInfo->dwProcessorType = PROCESSOR_INTEL_486;
        }
        else {
            lpSystemInfo->dwProcessorType = PROCESSOR_INTEL_PENTIUM;
        }
    }
    else if (ProcessorInfo.ProcessorArchitecture == PROCESSOR_ARCHITECTURE_MIPS) {
        lpSystemInfo->dwProcessorType = PROCESSOR_MIPS_R4000;
    }
    else if (ProcessorInfo.ProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA) {
        lpSystemInfo->dwProcessorType = PROCESSOR_ALPHA_21064;
    }
    else if (ProcessorInfo.ProcessorArchitecture == PROCESSOR_ARCHITECTURE_PPC) {
        lpSystemInfo->dwProcessorType = 604;  // backward compatibility
    }
    else if (ProcessorInfo.ProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) {
        lpSystemInfo->dwProcessorType = PROCESSOR_INTEL_IA64;
    }
    else {
        lpSystemInfo->dwProcessorType = 0;
    }

    lpSystemInfo->dwAllocationGranularity = BasicInfo.AllocationGranularity;
}



BOOL GetExitCodeThread(
    HANDLE  hThread,
    LPDWORD lpExitCode
    )
{
    PETHREAD thread;
    if (NT_SUCCESS(ObReferenceObjectByHandle(hThread, MAXIMUM_ALLOWED, NULL, KernelMode, (PVOID *)&thread, NULL))) {
        *lpExitCode = (DWORD)PsGetThreadExitStatus(thread);
        return TRUE;
    }
    return FALSE;
}


#define INFINITE            0xFFFFFFFF  // Infinite timeout


PLARGE_INTEGER
BaseFormatTimeOut(
    _Inout_ PLARGE_INTEGER TimeOut,
    _In_ DWORD Milliseconds
)
{
    if (Milliseconds == INFINITE) {
        return NULL;
    }

    TimeOut->QuadPart = UInt32x32To64(Milliseconds, 10000);
    TimeOut->QuadPart *= -1;
    return TimeOut;
}


#define WAIT_FAILED ((DWORD)0xFFFFFFFF)


WINBASEAPI
DWORD
WINAPI
WaitForSingleObjectEx(
    _In_ HANDLE hHandle,
    _In_ DWORD dwMilliseconds,
    _In_ BOOL bAlertable
)
{
    NTSTATUS Status;
    LARGE_INTEGER TimeOut;
    PLARGE_INTEGER pTimeOut;

    pTimeOut = BaseFormatTimeOut(&TimeOut, dwMilliseconds);

    Status = ZwWaitForSingleObject(hHandle, (BOOLEAN)bAlertable, pTimeOut);
    if (!NT_SUCCESS(Status)) {
        Status = (NTSTATUS)0xffffffff;
    }
    return (DWORD)Status;
}

WINBASEAPI
DWORD
WINAPI
WaitForSingleObject(
    _In_ HANDLE hHandle,
    _In_ DWORD dwMilliseconds
)
{
    return WaitForSingleObjectEx(hHandle, dwMilliseconds, FALSE);
}

WINBASEAPI
DWORD
WINAPI
SleepEx(
    _In_ DWORD dwMilliseconds,
    _In_ BOOL bAlertable
)
{
    NTSTATUS Status;
    PLARGE_INTEGER pTimeOut;
    LARGE_INTEGER TimeOut;

    pTimeOut = BaseFormatTimeOut(&TimeOut, dwMilliseconds);

    if (!pTimeOut) {
        TimeOut.LowPart = 0x0;
        TimeOut.HighPart = 0x80000000;
        pTimeOut = &TimeOut;
    }

    do {

        Status = KeDelayExecutionThread(KernelMode,
            (BOOLEAN)bAlertable,
            pTimeOut);

    } while (bAlertable && (Status == STATUS_ALERTED));

    return Status == STATUS_USER_APC ? STATUS_USER_APC : 0;
}

WINBASEAPI
VOID
WINAPI
Sleep(
    _In_ DWORD dwMilliseconds
)
{
    SleepEx(dwMilliseconds,
        FALSE);
}


namespace {
    using _Thrd_start_t = int (*)(void*);

    struct _Thrd_binder { // bind function pointer and data to pass to thread entry point
        _Thrd_start_t func;
        void* data;
        _Cnd_t* cond;
        _Mtx_t* mtx;
        int* started;
    };

    using _Thrd_callback_t = unsigned int(__stdcall*)(void*);

    unsigned int __stdcall _Thrd_runner(void* d) { // call thread function
        _Thrd_binder b = *static_cast<_Thrd_binder*>(d);
        _Mtx_lock(*b.mtx);
        *b.started = 1;
        _Cnd_signal(*b.cond);
        _Mtx_unlock(*b.mtx);
        const unsigned int res = b.func(b.data);
        _Cnd_do_broadcast_at_thread_exit();
        return res;
    }
} // unnamed namespace

_EXTERN_C

_Success_(return != 0)
_ACRTIMP uintptr_t __cdecl _beginthreadex(
    _In_opt_  void* _Security,
    _In_      unsigned                 _StackSize,
    _In_      _beginthreadex_proc_type _StartAddress,
    _In_opt_  void* _ArgList,
    _In_      unsigned                 _InitFlag,
    _Out_opt_ unsigned* _ThrdAddr
)
{
    _Security;

    HANDLE handle;
    struct context_t{
        unsigned                 _StackSize;
        _beginthreadex_proc_type _StartAddress;
        void* _ArgList;
        unsigned                 _InitFlag;
    };

    context_t* ctx = new context_t;
    ctx->_StartAddress = _StartAddress;
    ctx->_ArgList = _ArgList;
    ctx->_StackSize = _StackSize;
    ctx->_InitFlag = _InitFlag;

    PsCreateSystemThread(&handle, MAXIMUM_ALLOWED, NULL, NULL, NULL, [](PVOID StartContext) {
        context_t* ctx = (context_t*)StartContext;
        if (ctx->_StackSize)        {
            KeExpandKernelStackAndCallout([](PVOID context) {
                context_t* ctx = (context_t*)context;
                ctx->_StartAddress(ctx->_ArgList);
                delete ctx;
                }, ctx, ctx->_StackSize);
        }
        else {
            ctx->_StartAddress(ctx->_ArgList);
            delete ctx;
        }
    }, _ArgList);
    if (_ThrdAddr) {
        PETHREAD thread;
        if (NT_SUCCESS(ObReferenceObjectByHandle(handle, MAXIMUM_ALLOWED, NULL, KernelMode, (PVOID*)&thread, NULL))) {
            *_ThrdAddr = PtrToInt(PsGetThreadId(thread));
        }
        
    }
    return reinterpret_cast<uintptr_t>(handle);
}

_ACRTIMP void __cdecl _endthreadex(
    _In_ unsigned _ReturnCode
)
{
    PsTerminateSystemThread(_ReturnCode);
}

// TRANSITION, ABI: _Thrd_exit() is preserved for binary compatibility
_CRTIMP2_PURE void _Thrd_exit(int res) { // terminate execution of calling thread
    _endthreadex(res);
}

// TRANSITION, ABI: _Thrd_start() is preserved for binary compatibility
_CRTIMP2_PURE int _Thrd_start(_Thrd_t* thr, _Thrd_callback_t func, void* b) { // start a thread
    thr->_Hnd = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, func, b, 0, &thr->_Id));
    return thr->_Hnd == nullptr ? _Thrd_error : _Thrd_success;
}

int _Thrd_join(_Thrd_t thr, int* code) { // return exit code when thread terminates
    unsigned long res;
    if (WaitForSingleObjectEx(thr._Hnd, INFINITE, FALSE) == WAIT_FAILED || GetExitCodeThread(thr._Hnd, &res) == 0) {
        return _Thrd_error;
    }

    if (code) {
        *code = static_cast<int>(res);
    }

    return _Thrd_detach(thr);
}

int _Thrd_detach(_Thrd_t thr) { // tell OS to release thread's resources when it terminates
#if _KERNEL_MODE
    return ZwClose(thr._Hnd) == 0 ? _Thrd_error : _Thrd_success;
#else
    return CloseHandle(thr._Hnd) == 0 ? _Thrd_error : _Thrd_success;
#endif
}

void _Thrd_sleep(const xtime* xt) { // suspend thread until time xt
 
    xtime now;
    xtime_get(&now, TIME_UTC);
    do { // sleep and check time
        Sleep(_Xtime_diff_to_millis2(xt, &now));
        xtime_get(&now, TIME_UTC);
    } while (now.sec < xt->sec || now.sec == xt->sec && now.nsec < xt->nsec);
}

void _Thrd_yield() { // surrender remainder of timeslice
    YieldProcessor();
    // SwitchToThread();
}

// TRANSITION, ABI: _Thrd_equal() is preserved for binary compatibility
_CRTIMP2_PURE int _Thrd_equal(_Thrd_t thr0, _Thrd_t thr1) { // return 1 if thr0 and thr1 identify same thread
    return thr0._Id == thr1._Id;
}

// TRANSITION, ABI: _Thrd_current() is preserved for binary compatibility
_CRTIMP2_PURE _Thrd_t _Thrd_current() { // return _Thrd_t identifying current thread
    _Thrd_t result;
    result._Hnd = nullptr;
    result._Id = _Thrd_id();
    return result;
}

_Thrd_id_t _Thrd_id() { // return unique id for current thread
    return (_Thrd_id_t)__GetCurrentThreadId();
}

unsigned int _Thrd_hardware_concurrency() { // return number of processors
    SYSTEM_INFO info;
    GetNativeSystemInfo(&info);
    return info.dwNumberOfProcessors;
}

// TRANSITION, ABI: _Thrd_create() is preserved for binary compatibility
_CRTIMP2_PURE int _Thrd_create(_Thrd_t* thr, _Thrd_start_t func, void* d) { // create thread
    int res;
    _Thrd_binder b;
    int started = 0;
    _Cnd_t cond;
    _Mtx_t mtx;
    _Cnd_init(&cond);
    _Mtx_init(&mtx, _Mtx_plain);
    b.func = func;
    b.data = d;
    b.cond = &cond;
    b.mtx = &mtx;
    b.started = &started;
    _Mtx_lock(mtx);
    if ((res = _Thrd_start(thr, _Thrd_runner, &b)) == _Thrd_success) { // wait for handshake
        while (!started) {
            _Cnd_wait(cond, mtx);
        }
    }
    _Mtx_unlock(mtx);
    _Cnd_destroy(cond);
    _Mtx_destroy(mtx);
    return res;
}

_END_EXTERN_C

/*
 * This file is derived from software bearing the following
 * restrictions:
 *
 * (c) Copyright William E. Kempf 2001
 *
 * Permission to use, copy, modify, distribute and sell this
 * software and its documentation for any purpose is hereby
 * granted without fee, provided that the above copyright
 * notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting
 * documentation. William E. Kempf makes no representations
 * about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 */