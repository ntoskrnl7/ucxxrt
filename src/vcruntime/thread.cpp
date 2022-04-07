#if _KERNEL_MODE

#include <Ldk/windows.h>
#include <process.h>

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
    struct context_t {
        _beginthreadex_proc_type _StartAddress;
        void* _ArgList;
    };

    context_t* ctx = new context_t;
    ctx->_StartAddress = _StartAddress;
    ctx->_ArgList = _ArgList;

    DWORD threadId;
    HANDLE handle = CreateThread(
        (LPSECURITY_ATTRIBUTES)_Security,
        _StackSize,
        [](void* param) -> DWORD {
            context_t* ctx = (context_t*)param;
            return ctx->_StartAddress(ctx->_ArgList);
         },
        ctx,
        _InitFlag,
        &threadId);

    if (handle == NULL) {
        delete ctx;
        return NULL;
    }

    if (_ThrdAddr)
        *_ThrdAddr = threadId;

    return reinterpret_cast<uintptr_t>(handle);
}

_ACRTIMP void __cdecl _endthreadex(
    _In_ unsigned _ReturnCode
)
{
    PsTerminateSystemThread(_ReturnCode);
}
#endif