/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      sys_main.cpp
 * DATA:      2021/05/03
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#include <corecrt_startup.h>

#include <vcstartup_internal.h>
#include <vcruntime_internal.h>
#include <ldk.h>

#ifdef _KERNEL_MODE

_CRT_BEGIN_C_HEADER

int  __cdecl _do_onexit();
int  __cdecl _do_quick_onexit();

void __cdecl __initialize_memory();
void __cdecl __acrt_initialize_new_handler(_In_opt_ void* encoded_null);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Client Entry Point Declarations
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

NTSTATUS DriverMain(
    _In_ PDRIVER_OBJECT     DriverObject,
    _In_ PUNICODE_STRING    Registry
);

static int __cdecl invoke_main(PDRIVER_OBJECT DriverObject, PUNICODE_STRING Registry)
{
    return DriverMain(DriverObject, Registry);
}

static _tls_callback_type callback;

static void _cdecl run_callback(void)
{
    if (callback) {
        callback(NULL, DLL_PROCESS_DETACH, 0);
        callback = NULL;
    }
}

void __cdecl _register_thread_local_exe_atexit_callback(_tls_callback_type cb)
{
    callback = cb;
    /* This should guarantee that the callback is called. It won't be run in the
     * exact right spot as intended to, but it will be run. */
    atexit(run_callback);
}

static PDRIVER_UNLOAD __DriverUnload = nullptr;
static VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
    if (__DriverUnload)
    {
        __DriverUnload(DriverObject);
    }

    // do exit() of atexit()
    _do_onexit();

    // do pre terminations
    _initterm(__xp_a, __xp_z);

    // do terminations
    _initterm(__xt_a, __xt_z);

    LdkTerminate();
}

// :-(
CHAR CrtpTlsBuffer[1024 * 1024];
PVOID CrtpTlsSlots[1024] = { &CrtpTlsBuffer, };

VOID
CrtSetTebThreadLocalStoragePointer (
    _In_ PVOID ThreadLocalStoragePointer
    )
{
#if _WIN64
    __writegsqword(0x58, (ULONG_PTR)ThreadLocalStoragePointer);
#else
    __writefsdword(0x18, (ULONG_PTR)ThreadLocalStoragePointer);
#endif
}

VOID
CrtpSetTebThreadLocalStoragePointerDpcRoutine (
    _In_ struct _KDPC *Dpc,
    _In_opt_ PVOID DeferredContext,
    _In_opt_ PVOID ThreadLocalStoragePointer,
    _In_opt_ PVOID RemainingCount
    )
{
    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(DeferredContext);

    CrtSetTebThreadLocalStoragePointer(ThreadLocalStoragePointer);
    InterlockedDecrement((PLONG)RemainingCount);
}

NTSTATUS
CrtInitializeTebThreadLocalStoragePointer(
    VOID
    )
{
    KdBreakPoint();

    LONG remainingCount = KeNumberProcessors - 1;
    if (remainingCount == 0) {
        CrtSetTebThreadLocalStoragePointer(&CrtpTlsSlots);
        return STATUS_SUCCESS;
    }

    PKDPC dpcs = new(NonPagedPool) KDPC[KeNumberProcessors - 1];
    if (dpcs == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    KIRQL oldIrql = KeRaiseIrqlToDpcLevel();
    CCHAR currentProcessor = (CCHAR)KeGetCurrentProcessorNumber();
    PKDPC dpc = dpcs;
    for (CCHAR i = 0; i < KeNumberProcessors; i++) {
        if (i == currentProcessor) {
            continue;
        }        
        KeInitializeDpc(dpc, CrtpSetTebThreadLocalStoragePointerDpcRoutine, NULL);
        KeSetTargetProcessorDpc(dpc, i);
        KeInsertQueueDpc(dpc, &CrtpTlsSlots, &remainingCount);
        dpc++;
    }
    CrtSetTebThreadLocalStoragePointer(&CrtpTlsSlots);
    while (InterlockedCompareExchange(&remainingCount, 0, 0) != 0) {
        YieldProcessor();
    }
    KeLowerIrql(oldIrql);

    delete[] dpcs;

    return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING Registry)
{
    NTSTATUS status = CrtInitializeTebThreadLocalStoragePointer();
    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = LdkInitialize(DriverObject, Registry, 0);
    if (!NT_SUCCESS(status))
        return status;

    // do feature initializions
    __isa_available_init();

    // do memory initializions
    __initialize_memory();

    // do pointer initializions
    void* const encoded_null = __crt_fast_encode_pointer(nullptr);
    __acrt_initialize_new_handler(encoded_null);

    __try
    {
        // do C initializions
        if (_initterm_e(__xi_a, __xi_z) != 0)
            return STATUS_UNSUCCESSFUL;

        // do C++ initializions
        _initterm(__xc_a, __xc_z);

        // If this module has any dynamically initialized __declspec(thread)
        // variables, then we invoke their initialization for the primary thread
        // used to start the process:
        _tls_callback_type const* const tls_init_callback = __scrt_get_dyn_tls_init_callback();
        if (*tls_init_callback != nullptr && __scrt_is_nonwritable_in_current_image(tls_init_callback))
        {
            (*tls_init_callback)(nullptr, DLL_THREAD_ATTACH, nullptr);
        }

        // If this module has any thread-local destructors, register the
        // callback function with the Unified CRT to run on exit.
        _tls_callback_type const* const tls_dtor_callback = __scrt_get_dyn_tls_dtor_callback();
        if (*tls_dtor_callback != nullptr && __scrt_is_nonwritable_in_current_image(tls_dtor_callback))
        {
            _register_thread_local_exe_atexit_callback(*tls_dtor_callback);
        }

        //
        // Initialization is complete; invoke main...
        //
        status = invoke_main(DriverObject, Registry);
        if (NT_SUCCESS(status)) {
            if (DriverObject->DriverUnload) {
                __DriverUnload = DriverObject->DriverUnload;
                DriverObject->DriverUnload = DriverUnload;
            }
        } else {
            // do exit() of atexit()
            _do_onexit();

            // do pre terminations
            _initterm(__xp_a, __xp_z);

            // do terminations
            _initterm(__xt_a, __xt_z);
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        status = GetExceptionCode();
    }

    return status;
}

_CRT_END_C_HEADER

#endif
